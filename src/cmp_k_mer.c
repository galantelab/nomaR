#include <stdio.h>
#include <string.h>
#include <assert.h>

#include "log.h"
#include "strv.h"
#include "k_mer.h"
#include "aa_k_mer.h"
#include "aa_file.h"
#include "weight.h"
#include "cmp_k_mer.h"

static inline size_t
calc_idf_nt (const CountTable *table, const size_t col)
{
	size_t nrows = count_table_get_nrows (table);
	size_t row = 0;

	size_t vet[nrows];

	for (row = 0; row < nrows; row++)
		vet[row] = count_table_get (table, row, col);

	return idf_nt (vet, nrows);
}

static inline void
cmp_process_k_mer (const char *seq, const size_t k, CountTable *table, double *l2_total)
{
	KMerIter iter = {};
	KMerPos pos = 0;
	int is_first_k_mer = 1;

	size_t row = 0;
	size_t nrows = 0;
	size_t col = 0;
	size_t count = 0;

	size_t nt = 0;
	double idf_val = 0;

	char k_mer[k + 1];

	nrows = count_table_get_nrows (table);

	k_mer_iter_init (&iter, seq, k);

	printf ("\t");

	while ((pos = k_mer_iter_next (&iter, k_mer)))
		{
			col = aa_k_mer_get_pos_corrected_index (k_mer, k, pos);
			count = count_table_get (table, 0, col);

			nt = calc_idf_nt (table, col);
			idf_val = idf (nrows, nt);

			if (is_first_k_mer)
				is_first_k_mer = 0;
			else
				printf (";");

			printf ("%s:%e", k_mer, (tf (count, count) * idf_val) / l2_total[0]);

			for (row = 1; row < nrows; row++)
				{
					count = count_table_get (table, row, col);
					printf (",%e", (tf (count, count) * idf_val) / l2_total[row]);
				}
		}
}

static inline void
print_header (char **label)
{
	char **p = NULL;

	printf ("#CLASS\tSEQ\tK-MER:%s", label[0]);

	for (p = &label[1]; *p != NULL; p++)
		printf (",%s", *p);

	printf ("\n");
}

static inline void
calc_count_total (const CountTable *table, size_t *count_total)
{
	size_t i = 0;
	size_t j = 0;

	memset (count_total, 0, sizeof (size_t) * count_table_get_nrows (table));

	for (i = 0; i < count_table_get_nrows (table); i++)
		for (j = 0; j < count_table_get_ncols (table); j++)
			count_total[i] += count_table_get (table, i, j);
}

static inline void
calc_l2_space (const CountTable *table, double *l2_total)
{
	size_t *data = NULL;
	size_t dim[2] = {};
	size_t i = 0;

	data = count_table_data (table);
	count_table_get_dim (table, dim);

	memset (l2_total, 0, sizeof (double) * dim[0]);

	for (i = 0; i < dim[0]; i++)
		l2_total[i] = l2_space (&data[i * dim[1]], dim[1]);
}

void
cmp_k_mer (const CountKMer *ck, const char *file)
{
	assert (ck != NULL);
	assert (file != NULL);

	AAFile *aa_file = NULL;
	AAFileEntry *entry = NULL;

	/*size_t count_total[count_table_get_nrows (ck->table)];*/
	/*calc_count_total (ck->table, count_total);*/

	double l2_total[count_table_get_nrows (ck->table)];
	calc_l2_space (ck->table, l2_total);

	print_header (ck->label);

	aa_file = aa_file_open_for_reading (file);
	entry = aa_file_entry_new ();

	while (aa_file_read (aa_file, entry))
		{
			if (strlen (entry->seq) < ck->k)
				{
					log_warn (
							"CLASS (%s) SEQ (%s) length is "
							"smaller than the size of the "
							"k-mer (%zu) at line %zu",
							entry->class, entry->seq,
							ck->k, entry->num_line);
					continue;
				}

			if (!strv_contains ((const char * const*) ck->label, entry->class, NULL))
				{
					log_warn ("CLASS (%s) not found counts at line %zu",
							entry->class, entry->num_line);
					continue;
				}

			printf ("%s\t%s", entry->class, entry->seq);

			cmp_process_k_mer (entry->seq, ck->k, ck->table, l2_total);

			printf ("\n");
		}

	aa_file_entry_free (entry);
	aa_file_close (aa_file);
}
