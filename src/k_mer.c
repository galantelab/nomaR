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
	size_t          seq_len;
	const char    * seq;

	size_t          x, y, z;

	size_t          k;
};

typedef struct _RealIter RealIter;

void
k_mer_iter_init (KMerIter *iter, const char *seq, size_t k)
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

KMerPos
k_mer_iter_next (KMerIter *iter, char *k_mer)
{
	assert (iter != NULL);

	RealIter *ri = (RealIter *) iter;
	KMerPos rc = K_MER_END;

	if (ri->x < ri->k)
		{
			if (k_mer != NULL)
				SAFE_COPY (k_mer, &ri->seq[0], ri->x);
			ri->x++;
			rc = K_MER_LEFT_EDGE;
		}
	else if (ri->y < (ri->seq_len - ri->k + 1))
		{
			if (k_mer != NULL)
				SAFE_COPY (k_mer, &ri->seq[ri->y], ri->k);
			ri->y++;
			rc = K_MER_INTERNAL;
		}
	else if (ri->z > 0)
		{
			if (k_mer != NULL)
				SAFE_COPY (k_mer,
						&ri->seq[ri->seq_len - ri->z], ri->z);
			ri->z--;
			rc = K_MER_RIGHT_EDGE;
		}

	return rc;
}
