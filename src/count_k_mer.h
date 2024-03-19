#ifndef COUNT_K_MER_H
#define COUNT_K_MER_H

#include <stdint.h>

struct _Count
{
	uint32_t  x_len;
	uint32_t  y_len;
	uint32_t *data;
};

typedef struct _Count Count;

#define COUNT_GET(count,x,y) ((count)->data[x * (count)->y_len + y])

Count * count_k_mer (const char *file, const char **label, int num_label, int k);
void    count_free  (Count *c);

#endif /* count_k_mer.h */
