#ifndef COUNT_K_MER_H
#define COUNT_K_MER_H

#include <stdlib.h>

struct _Count
{
	size_t  x_len;
	size_t  y_len;
	size_t *data;
};

typedef struct _Count Count;

#define COUNT_GET(count,x,y) ((count)->data[x * (count)->y_len + y])

Count * count_k_mer (const char *file, const char **label, size_t num_label, size_t k);
void    count_free  (Count *c);

#endif /* count_k_mer.h */
