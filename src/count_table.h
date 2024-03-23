#ifndef COUNT_TABLE_H
#define COUNT_TABLE_H

#include <stdlib.h>

typedef struct _CountTable CountTable;

CountTable * count_table_new       (const size_t nrows, const size_t ncols);
void         count_table_free      (CountTable *ct);
void         count_table_add_row   (CountTable *ct, const size_t nrows);
size_t       count_table_get_nrows (const CountTable *ct);
size_t       count_table_get_ncols (const CountTable *ct);
void         count_table_get_dim   (const CountTable *ct, size_t *dim);
size_t *     count_table_data      (const CountTable *ct);
void         count_table_set       (CountTable *ct, size_t row, size_t col, const size_t value);
size_t       count_table_get       (const CountTable *ct, size_t row, size_t col);
void         count_table_sum       (CountTable *ct, size_t row, size_t col, const size_t value);

#endif /* count_table.h */
