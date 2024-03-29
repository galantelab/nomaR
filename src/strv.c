#include <stdio.h>
#include <string.h>
#include <assert.h>

#include "utils.h"
#include "wrapper.h"
#include "strv.h"

#define STRV_ALLOC_CHUNK 8

struct _StrvBuilder
{
	char    **strv;
	size_t    len;
	size_t    alloc;
};


StrvBuilder *
strv_builder_new (void)
{
	StrvBuilder *b = NULL;
	char **strv = NULL;

	strv = xcalloc (STRV_ALLOC_CHUNK, sizeof (char *));
	b = xcalloc (1, sizeof (StrvBuilder));

	*b = (StrvBuilder) {
		.strv  = strv,
		.len   = 0,
		.alloc = STRV_ALLOC_CHUNK
	};

	return b;
}

char **
strv_builder_end (StrvBuilder *b)
{
	if (b == NULL)
		return NULL;

	char **strv = b->strv;
	b->strv = NULL;

	xfree (b);

	return strv;
}

size_t
strv_builder_length (StrvBuilder *b)
{
	return b->len;
}

void
strv_builder_add (StrvBuilder *b, const char *value)
{
	assert (b != NULL);

	if (value == NULL)
		return;

	if ((b->len + 1) >= b->alloc)
		b->alloc = buf_expand ((void **) &b->strv, sizeof (char *),
				b->alloc, STRV_ALLOC_CHUNK);

	b->strv[b->len++] = xstrdup (value);
}

int
strv_builder_contains (StrvBuilder *b, const char *value, size_t *i)
{
	return strv_contains ((const char * const *) b->strv, value, i);
}

void
strv_free (char **strv)
{
	if (strv == NULL)
		return;

	char **strp = NULL;

	for (strp = strv; *strp != NULL; strp++)
		xfree (*strp);

	xfree (strv);
}

size_t
strv_length (char **strv)
{
	size_t len = 0;

	for (; *strv != NULL; strv++)
		len++;

	return len;
}

int
strv_contains (const char *const *strv, const char *str, size_t *i)
{
	assert (strv != NULL);

	if (str == NULL)
		return 0;

	size_t j = 0;

	for (; strv[j] != NULL; j++)
		{
			if (strcmp (strv[j], str) == 0)
				{
					if (i != NULL)
						*i = j;
					return 1;
				}
		}

	return 0;
}
