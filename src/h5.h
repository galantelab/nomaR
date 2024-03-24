#ifndef H5_H
#define H5_H

#include <stdlib.h>
#include "count_table.h"

typedef struct _H5 H5;

H5          * h5_create              (const char *file);
void          h5_close               (H5 *h5);
void          h5_write_count_dataset (H5 *h5, const CountTable *table, const size_t k);
void          h5_write_label_dataset (H5 *h5, const char **label);

H5          * h5_open                (const char *file);
CountTable  * h5_read_count_dataset  (H5 *h5, size_t *k);
char       ** h5_read_label_dataset  (H5 *h5);

#endif /* h5.h */
