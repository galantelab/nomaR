#include <stdio.h>
#include <string.h>
#include <assert.h>

#include "log.h"
#include "strv.h"
#include "k_mer.h"
#include "aa_k_mer.h"
#include "aa_file.h"
#include "cmp_k_mer.h"

static inline void
cmp_process_k_mer (const char *seq, const size_t k, CountTable *table)
{
	KMerIter iter = {};
	KMerPos pos = 0;
	int is_first_k_mer = 1;

	size_t count = 0;
	size_t row = 0;
	size_t col = 0;

	char k_mer[k + 1];

	k_mer_iter_init (&iter, seq, k);

	printf ("\t");

	while ((pos = k_mer_iter_next (&iter, k_mer)))
		{
			col = aa_k_mer_get_pos_corrected_index (k_mer, k, pos);
			count = count_table_get (table, 0, col);

			if (is_first_k_mer)
				is_first_k_mer = 0;
			else
				printf (";");

			printf ("%s:%zu", k_mer, count);

			for (row = 1; row < count_table_get_nrows (table); row++)
				{
					count = count_table_get (table, row, col);
					printf (",%zu", count);
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

void
cmp_k_mer (const CountKMer *ck, const char *file)
{
	assert (ck != NULL);
	assert (file != NULL);

	AAFile *aa_file = NULL;
	AAFileEntry *entry = NULL;

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

			cmp_process_k_mer (entry->seq, ck->k, ck->table);

			printf ("\n");
		}

	aa_file_entry_free (entry);
	aa_file_close (aa_file);
}
