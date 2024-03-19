#ifndef LOG_H
#define LOG_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>

typedef void (*LogLockFun) (void *udata, int lock);

enum { LOG_TRACE, LOG_DEBUG, LOG_INFO, LOG_WARN, LOG_ERROR, LOG_FATAL };

#define __FILENAME__ (strrchr("/" __FILE__, '/') + 1)

#define log_trace(...) log_log (LOG_TRACE, __FILENAME__, __LINE__, __VA_ARGS__)
#define log_debug(...) log_log (LOG_DEBUG, __FILENAME__, __LINE__, __VA_ARGS__)
#define log_info(...)  log_log (LOG_INFO,  __FILENAME__, __LINE__, __VA_ARGS__)
#define log_warn(...)  log_log (LOG_WARN,  __FILENAME__, __LINE__, __VA_ARGS__)
#define log_error(...) log_log (LOG_ERROR, __FILENAME__, __LINE__, __VA_ARGS__)

#define log_fatal(...) \
	do { \
		log_log (LOG_FATAL, __FILENAME__, __LINE__, __VA_ARGS__); \
		abort (); \
	} while (0)

#define log_errno_error(fmt, ...) log_error (fmt ": %s", ##__VA_ARGS__, strerror (errno))
#define log_errno_fatal(fmt, ...) log_fatal (fmt ": %s", ##__VA_ARGS__, strerror (errno))

void log_set_udata (void *udata);
void log_set_lock  (LogLockFun fn);
void log_set_fp    (FILE *fp);
void log_set_level (int level);
void log_set_quiet (int enable);
void log_set_color (int enable);

void log_log (int level, const char *file, int line, const char *fmt, ...)
	__attribute__((format (printf, 4, 5)));

#endif
