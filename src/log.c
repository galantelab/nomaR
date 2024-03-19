#include <stdarg.h>
#include <time.h>
#include "log.h"

struct Log
{
	void       *udata;
	LogLockFun  lock;
	FILE       *fp;
	int         level;
	int         quiet;
	int         color;
};

static struct Log L = { .color = 1 };

static const char *level_names[] = {
	"TRACE", "DEBUG", "INFO", "WARN", "ERROR", "FATAL"
};

static const char *level_colors[] = {
	"\x1b[94m", "\x1b[36m", "\x1b[32m", "\x1b[33m", "\x1b[31m", "\x1b[35m"
};

static void
lock (void)
{
	if (L.lock)
		L.lock (L.udata, 1);
}

static void
unlock (void)
{
	if (L.lock)
		L.lock (L.udata, 0);
}


void
log_set_udata (void *udata)
{
	L.udata = udata;
}


void
log_set_lock (LogLockFun fn)
{
	L.lock = fn;
}


void
log_set_fp (FILE *fp)
{
	L.fp = fp;
}


void
log_set_level (int level)
{
	L.level = level;
}


void
log_set_quiet (int enable)
{
	L.quiet = enable ? 1 : 0;
}

void
log_set_color (int enable)
{
	L.color = enable ? 1 : 0;
}

void
log_log (int level, const char *file, int line,
		const char *fmt, ...)
{
	if (level < L.level)
		return;

	/* Acquire lock */
	lock ();

	/* Get current time */
	time_t t = time (NULL);
	struct tm *lt = localtime (&t);

	/* Log to stderr */
	if (!L.quiet)
		{
			va_list args;
			char buf[16];

			buf[strftime (buf, sizeof (buf),
					"%H:%M:%S", lt)] = '\0';

			if (L.color)
				{
					fprintf (stderr, "%s %s%-5s\x1b[0m \x1b[90m%s:%d:\x1b[0m ",
						buf, level_colors[level], level_names[level], file, line);
				}
			else
				{
					fprintf (stderr, "%s %-5s %s:%d: ",
						buf, level_names[level], file, line);
				}

			va_start (args, fmt);
			vfprintf (stderr, fmt, args);
			va_end (args);

			fprintf (stderr, "\n");
			fflush( stderr);
		}

	/* Log to file */
	if (L.fp)
		{
			va_list args;
			char buf[32];

			buf[strftime (buf, sizeof (buf),
					"%Y-%m-%d %H:%M:%S", lt)] = '\0';

			fprintf (L.fp, "%s %-5s %s:%d: ",
				buf, level_names[level], file, line);

			va_start (args, fmt);
			vfprintf (L.fp, fmt, args);
			va_end (args);

			fprintf (L.fp, "\n");
			fflush (L.fp);
		}

	/* Release lock */
	unlock ();
}
