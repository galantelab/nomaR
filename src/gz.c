#include <stdio.h>
#include <string.h>
#include <assert.h>
#include <error.h>
#include <errno.h>

#include "wrapper.h"
#include "utils.h"
#include "gz.h"

GzFile *
gz_open_for_reading (const char *path)
{
	assert (path != NULL);

	GzFile *gz = NULL;

	gz = xcalloc (1, sizeof (GzFile));
	gz->fp = gzopen (path, "rb");

	if (gz->fp == NULL)
		error (EXIT_FAILURE, errno,
				"Could not open '%s' for reading", path);

	gz->filename = xstrdup (path);
	gz->buf = xcalloc (BUFSIZ, sizeof (char));
	gz->buf_size = BUFSIZ;

	return gz;
}

void
gz_close (GzFile *gz)
{
	if (gz == NULL)
		return;

	int rc;

	rc = gzclose (gz->fp);
	if (rc != Z_OK)
		error (EXIT_FAILURE, 0,
				"Could not close file '%s': %s", gz->filename,
				gzerror (gz->fp, &rc));

	xfree (gz->buf);
	xfree ((void *) gz->filename);
	xfree (gz);
}

static void
gz_check_error (const GzFile *gz)
{
	assert (gz != NULL);

	const char *err_msg = NULL;
	int rc = 0;

	err_msg = gzerror (gz->fp, &rc);
	if (rc != Z_OK)
		error (EXIT_FAILURE, 0,
				"Could not read entry from '%s': %s",
				gz->filename, err_msg);
}

int
gz_getline (GzFile *gz, char **lineptr, size_t *n)
{
	assert (gz != NULL && lineptr != NULL && n != NULL);

	char *line = NULL;
	size_t len = 0;

	line = gzgets (gz->fp, gz->buf, gz->buf_size);

	if (line == NULL)
		{
			gz_check_error (gz);
			return 0;
		}

	for (len = strlen (line); line[len - 1] != '\n'; len = strlen (line))
		{
			size_t old_size = gz->buf_size;

			gz->buf_size = buf_expand ((void **) &gz->buf,
					sizeof (char), gz->buf_size, BUFSIZ);

			line = gzgets (gz->fp, &gz->buf[old_size - 1],
					gz->buf_size - old_size + 1);

			if (line == NULL)
				{
					gz_check_error (gz);
					break;
				}
		}

	len = strlen (gz->buf);

	if (*lineptr == NULL || *n < (len + 1))
		{
			*n = len + 1;
			*lineptr = xrealloc (*lineptr, *n);
		}

	strncpy (*lineptr, gz->buf, *n);
	gz->num_line ++;

	return 1;
}
