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
calc_count_n (const CountTable *table, const size_t col)
{
	size_t n = 0;
	size_t row = 0;

	for (; row < count_table_get_nrows (table); row++)
		n += count_table_get (table, row, col) > 0 ? 1 : 0;

	return n;
}

static inline void
cmp_process_k_mer (const char *seq, const size_t k, CountTable *table, size_t *count_total)
{
	KMerIter iter = {};
	KMerPos pos = 0;
	int is_first_k_mer = 1;

	size_t count = 0;
	size_t row = 0;
	size_t row_total = 0;
	size_t col = 0;

	size_t n = 0;
	double idf_val = 0;

	char k_mer[k + 1];

	row_total = count_table_get_nrows (table);

	k_mer_iter_init (&iter, seq, k);

	printf ("\t");

	while ((pos = k_mer_iter_next (&iter, k_mer)))
		{
			col = aa_k_mer_get_pos_corrected_index (k_mer, k, pos);
			count = count_table_get (table, 0, col);

			n = calc_count_n (table, col);
			idf_val = idf (row_total, n);

			if (is_first_k_mer)
				is_first_k_mer = 0;
			else
				printf (";");

			printf ("%s:%f", k_mer, tf (count, count_total[0]) * idf_val);

			for (row = 1; row < row_total; row++)
				{
					count = count_table_get (table, row, col);
					printf (",%f", tf (count, count_total[row]) * idf_val);
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

void
cmp_k_mer (const CountKMer *ck, const char *file)
{
	assert (ck != NULL);
	assert (file != NULL);

	AAFile *aa_file = NULL;
	AAFileEntry *entry = NULL;

	size_t count_total[count_table_get_nrows (ck->table)];
	calc_count_total (ck->table, count_total);

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

			cmp_process_k_mer (entry->seq, ck->k, ck->table, count_total);

			printf ("\n");
		}

	aa_file_entry_free (entry);
	aa_file_close (aa_file);
}
