#ifndef COUNT_K_MER_H
#define COUNT_K_MER_H

#include <stdlib.h>

#include "count_table.h"

struct _CountKMer
{
	size_t        k;
	char        **label;
	CountTable   *table;
};

typedef struct _CountKMer CountKMer;

CountKMer * count_k_mer      (const char *file, const size_t k);
void        count_k_mer_free (CountKMer *ck);

#endif /* count_k_mer.h */
