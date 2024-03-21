#ifndef K_MER_H
#define K_MER_H

#include <stdlib.h>

enum _KMerPos
{
	K_MER_END = 0,
	K_MER_LEFT_EDGE,
	K_MER_RIGHT_EDGE,
	K_MER_INTERNAL
};

typedef enum _KMerPos KMerPos;

struct _KMerIter
{
	size_t    dummy1;
	void    * dummy2;
	size_t    dummy3;
	size_t    dummy4;
	size_t    dummy5;
	size_t    dummy6;
};

typedef struct _KMerIter KMerIter;

void     k_mer_iter_init (KMerIter *iter, const char *seq, size_t k);
KMerPos  k_mer_iter_next (KMerIter *iter, char *k_mer);

#endif /* k_mer.h */
