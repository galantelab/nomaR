#include <string.h>
#include <assert.h>

#include "wrapper.h"
#include "count_table.h"

#define COUNT_TABLE_GET(ct,x,y) ((ct)->data[x * (ct)->ncols + y])

struct _CountTable
{
	size_t  nrows;
	size_t  ncols;
	size_t *data;
};

CountTable *
count_table_new (const size_t nrows, const size_t ncols)
{
	assert (ncols > 0);

	CountTable *ct = NULL;

	ct = xcalloc (1, sizeof (CountTable));

	if (nrows > 0)
		ct->data = xcalloc (nrows * ncols, sizeof (size_t));

	ct->nrows = nrows;
	ct->ncols = ncols;

	return ct;
}

void
count_table_free (CountTable *ct)
{
	if (ct == NULL)
		return;

	xfree (ct->data);
	xfree (ct);
}

void
count_table_add_row (CountTable *ct, const size_t nrows)
{
	assert (ct != NULL);
	assert (nrows > 0);

	size_t old_nrows = ct->nrows;
	size_t final_nrows = ct->nrows + nrows;

	ct->data = xrealloc (ct->data,
			final_nrows * ct->ncols * sizeof (size_t));

	memset (&ct->data[old_nrows * ct->ncols], 0,
			(final_nrows - old_nrows) * ct->ncols * sizeof (size_t));

	ct->nrows = final_nrows;
}

size_t
count_table_get_nrows (const CountTable *ct)
{
	assert (ct != NULL);
	return ct->nrows;
}

size_t
count_table_get_ncols (const CountTable *ct)
{
	assert (ct != NULL);
	return ct->ncols;
}

void
count_table_get_dim (const CountTable *ct, size_t *dim)
{
	assert (ct != NULL);
	assert (dim != NULL);

	dim[0] = ct->nrows;
	dim[1] = ct->ncols;
}

size_t *
count_table_data (const CountTable *ct)
{
	assert (ct != NULL);
	return ct->data;
}

void
count_table_set (CountTable *ct, size_t row, size_t col, const size_t value)
{
	assert (ct != NULL);
	assert (row < ct->nrows);
	assert (col < ct->ncols);

	COUNT_TABLE_GET (ct, row, col) = value;
}

size_t
count_table_get (const CountTable *ct, size_t row, size_t col)
{
	assert (ct != NULL);
	assert (row < ct->nrows);
	assert (col < ct->ncols);

	return COUNT_TABLE_GET (ct, row, col);
}

void
count_table_sum (CountTable *ct, size_t row, size_t col, const size_t value)
{
	assert (ct != NULL);
	assert (row < ct->nrows);
	assert (col < ct->ncols);

	COUNT_TABLE_GET (ct, row, col) += value;
}
