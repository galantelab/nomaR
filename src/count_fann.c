#include <stdio.h>
#include <string.h>
#include <assert.h>

#include "log.h"
#include "strv.h"
#include "k_mer.h"
#include "aa_k_mer.h"
#include "aa_file.h"
#include "wrapper.h"

static void
count_process_k_mer (FILE *fp, const char *seq, size_t k,
		size_t *table, size_t ncols)
{
	KMerIter iter = {};
	KMerPos pos = 0;
	size_t col = 0;
	size_t i = 0;
	char k_mer[k + 1];

	k_mer_iter_init (&iter, seq, k);

	while ((pos = k_mer_iter_next (&iter, k_mer)))
	{
		col = aa_k_mer_get_pos_corrected_index (k_mer, k, pos);
		table[col]++;
	}

	fprintf (fp, "%zu", table[0]);
	for (i = 1; i < ncols; i++)
		fprintf (fp, " %zu", table[i]);

	fprintf (fp, "\n");
}

static void
count_process_data (FILE *fp, const char *file, size_t *table,
		size_t ncols, char **label, size_t k)
{
	AAFile *aa_file = NULL;
	AAFileEntry *entry = NULL;

	size_t row = 0;
	size_t nrows = 0;
	size_t i = 0;

	nrows = strv_length (label);
	aa_file = aa_file_open_for_reading (file);
	entry = aa_file_entry_new ();

	while (aa_file_read (aa_file, entry))
		{
			if (strlen (entry->seq) < k)
				continue;

			strv_contains ((const char * const*) label,
					entry->class, &row);

			memset (table, 0, sizeof (size_t) * ncols);
			count_process_k_mer (fp, entry->seq, k, table, ncols);

			fprintf (fp, "%d", row == 0 ? 1 : 0);
			for (i = 1; i < nrows; i++)
				fprintf (fp, " %d", row == i ? 1 : 0);
			fprintf (fp, "\n");
		}

	aa_file_entry_free (entry);
	aa_file_close (aa_file);
}

static char **
count_label (const char *prefix, const char *file, size_t k, size_t *acm)
{
	FILE *fp = NULL;

	char data[BUFSIZ] = {};
	char **label = NULL;

	AAFile *aa_file = NULL;
	AAFileEntry *entry = NULL;
	StrvBuilder *sb = NULL;

	size_t i = 0;

	snprintf (data, BUFSIZ, "%s_label.txt", prefix);

	fp = fopen (data, "w");
	if (fp == NULL)
		log_errno_fatal ("Error opening '%s'", data);

	sb = strv_builder_new ();

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

			if (!strv_builder_contains (sb, entry->class, NULL))
				strv_builder_add (sb, entry->class);

			(*acm)++;
		}

	label = strv_builder_end (sb);

	for (i = 0; i < strv_length (label); i++)
		fprintf (fp, "%s\n", label[i]);

	fclose (fp);
	aa_file_entry_free (entry);
	aa_file_close (aa_file);

	return label;
}

void
count_fann (const char *prefix, const char *file, const size_t k)
{
	assert (file != NULL);
	assert (k > 0);

	FILE *fp = NULL;

	char data[BUFSIZ] = {};
	char **label = NULL;

	size_t *table = NULL;
	size_t ncols = 0;
	size_t acm = 0;

	label = count_label (prefix, file, k, &acm);
	ncols = aa_k_mer_get_pos_corrected_total (k);
	table = xcalloc (ncols, sizeof (size_t));

	snprintf (data, BUFSIZ, "%s.data", prefix);

	fp = fopen (data, "w");
	if (fp == NULL)
		log_errno_fatal ("Error opening '%s'", data);

	// header
	fprintf (fp, "%zu %zu %zu\n", acm, ncols, strv_length (label));

	count_process_data (fp, file, table, ncols, label, k);

	strv_free (label);
	xfree (table);
	fclose (fp);
}
