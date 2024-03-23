#include <stdio.h>
#include <string.h>
#include <assert.h>

#include "log.h"
#include "utils.h"
#include "wrapper.h"
#include "aa.h"
#include "aa_file.h"

#define AA_FILE_GETLINE \
	while ((rc = gz_getline (aa_file->gz, &aa_file->buf, &aa_file->buf_size)) \
					&& aa_file->buf[0] == '\n')

AAFileEntry *
aa_file_entry_new (void)
{
	AAFileEntry *entry = xcalloc (1, sizeof (AAFileEntry));
	return entry;
}

void
aa_file_entry_free (AAFileEntry *entry)
{
	if (entry == NULL)
		return;

	xfree (entry->class);
	xfree (entry->seq);

	xfree (entry);
}

AAFile *
aa_file_open_for_reading (const char *path)
{
	assert (path != NULL);

	AAFile *aa_file = NULL;
	int rc = 0;

	aa_file = xcalloc (1, sizeof (AAFile));
	aa_file->gz = gz_open_for_reading (path);

	AA_FILE_GETLINE;

	// Reached end of file
	if (!rc)
		aa_file->eof = 1;

	return aa_file;
}

void
aa_file_close (AAFile *aa_file)
{
	if (aa_file == NULL)
		return;

	gz_close (aa_file->gz);

	xfree (aa_file->buf);
	xfree (aa_file);
}

int
aa_file_read (AAFile *aa_file, AAFileEntry *entry)
{
	assert (aa_file != NULL && entry != NULL);

	int rc = 0;

	char *saveptr = NULL;

	const char *class = NULL;
	const char *seq = NULL;

	// end of file
	if (aa_file->eof)
		return 0;

	aa_file->buf = chomp (aa_file->buf);
	entry->num_line = gz_get_num_line (aa_file->gz);

	class = strtok_r (aa_file->buf, "\t ", &saveptr);
	seq = strtok_r (NULL, "\t ", &saveptr);

	if (class == NULL)
		log_fatal ("Missing 'CLASS' (field 1) at line %zu",
				entry->num_line);

	entry->class_size = entry_set (&entry->class,
			entry->class_size, class);

	if (seq == NULL)
		log_fatal ("Missing 'SEQ' (field 2) at line %zu",
				entry->num_line);

	if (!aa_check (seq))
		log_fatal (
				"CLASS (%s) SEQ (%s) does not seem "
				"to be an amino acid at line %zu",
				class, seq, entry->num_line);

	entry->seq_size = entry_set (&entry->seq,
			entry->seq_size, seq);

	/*
	* get next entry
	* ignore blank lines
	*/
	AA_FILE_GETLINE;

	// Reached end of file
	if (!rc)
		aa_file->eof = 1;

	return 1;
}
