#ifndef K_MER_H
#define K_MER_H

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
	int    dummy1;
	void * dummy2;
	int    dummy3;
	int    dummy4;
	int    dummy5;
	int    dummy6;
};

typedef struct _KMerIter KMerIter;

void     k_mer_iter_init (KMerIter *iter, const char *seq, int k);
KMerPos  k_mer_iter_next (KMerIter *iter, char *k_mer);

#endif /* k_mer.h */
