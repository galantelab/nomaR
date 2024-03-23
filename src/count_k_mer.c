#include <stdio.h>
#include <string.h>
#include <assert.h>

#include "log.h"
#include "strv.h"
#include "k_mer.h"
#include "aa_k_mer.h"
#include "aa_file.h"
#include "wrapper.h"
#include "count_k_mer.h"

static inline void
count_process_k_mer (const char *seq, const size_t row,
		const size_t k, CountTable *table)
{
	KMerIter iter = {};
	KMerPos pos = 0;
	size_t col = 0;
	char k_mer[k + 1];

	k_mer_iter_init (&iter, seq, k);

	while ((pos = k_mer_iter_next (&iter, k_mer)))
	{
		col = aa_k_mer_get_pos_corrected_index (k_mer, k, pos);
		count_table_sum (table, row, col, 1);
	}
}

static inline void
count_process_data (const char *file, CountTable *table,
		StrvBuilder *sb, const size_t k)
{
	AAFile *aa_file = NULL;
	AAFileEntry *entry = NULL;
	size_t row = 0;

	aa_file = aa_file_open_for_reading (file);
	entry = aa_file_entry_new ();

	while (aa_file_read (aa_file, entry))
		{
			if (strlen (entry->seq) < k)
				{
					log_warn (
							"CLASS (%s) SEQ (%s) length is "
							"smaller than the size of the "
							"k-mer (%zu) at line %zu",
							entry->class, entry->seq,
							k, entry->num_line);
					continue;
				}

			if (!strv_builder_contains (sb, entry->class, &row))
				{
					count_table_add_row (table, 1);
					strv_builder_add (sb, entry->class);
					row = strv_builder_length (sb) - 1;
				}

			count_process_k_mer (entry->seq, row, k, table);
		}

	aa_file_entry_free (entry);
	aa_file_close (aa_file);
}

CountKMer *
count_k_mer (const char *file, const size_t k)
{
	assert (file != NULL);
	assert (k > 0);

	CountKMer *ck = NULL;

	StrvBuilder *sb = NULL;
	CountTable *table = NULL;
	size_t ncols = 0;

	ck = xcalloc (1, sizeof (CountKMer));

	sb = strv_builder_new ();
	ncols = aa_k_mer_get_pos_corrected_total (k);
	table = count_table_new (0, ncols);

	count_process_data (file, table, sb, k);

	*ck = (CountKMer) {
		.k      = k,
		.table  = table,
		.label  = strv_builder_end (sb)
	};

	return ck;
}

void
count_k_mer_free (CountKMer *ck)
{
	if (ck == NULL)
		return;

	strv_free (ck->label);
	count_table_free (ck->table);

	xfree (ck);
}
