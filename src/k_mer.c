#include <string.h>
#include <assert.h>

#include "k_mer.h"

#define SAFE_COPY(to,from,n) \
	do { \
		strncpy (to,from,n); \
		to[n] = '\0'; \
	} while (0)

struct _RealIter
{
	int          seq_len;
	const char * seq;

	int          x, y, z;

	int          k;
};

typedef struct _RealIter RealIter;

void
k_mer_iter_init (KMerIter *iter, const char *seq, int k)
{
	assert (iter != NULL);
	assert (seq != NULL && seq[0] != '\0');
	assert (k > 0);

	RealIter *ri = (RealIter *) iter;

	*ri = (RealIter) {
		.seq_len = strlen (seq),
		.seq     = seq,
		.x       = 1,
		.y       = 0,
		.z       = k - 1,
		.k       = k
	};
}

int
k_mer_iter_next (KMerIter *iter, char *k_mer)
{
	assert (iter != NULL);

	RealIter *ri = (RealIter *) iter;
	int rc = 0;

	if (ri->x < ri->k)
		{
			if (k_mer != NULL)
				SAFE_COPY (k_mer, &ri->seq[0], ri->x);
			ri->x++;
			rc = 1;
		}
	else if (ri->y < (ri->seq_len - ri->k + 1))
		{
			if (k_mer != NULL)
				SAFE_COPY (k_mer, &ri->seq[ri->y], ri->k);
			ri->y++;
			rc = 1;
		}
	else if (ri->z > 0)
		{
			if (k_mer != NULL)
				SAFE_COPY (k_mer,
						&ri->seq[ri->seq_len - ri->z], ri->z);
			ri->z--;
			rc = 1;
		}

	return rc;
}
