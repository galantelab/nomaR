#ifndef K_MER_H
#define K_MER_H

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

void k_mer_iter_init (KMerIter *iter, const char *seq, int k);
int  k_mer_iter_next (KMerIter *iter, char *k_mer);

#endif /* k_mer.h */
