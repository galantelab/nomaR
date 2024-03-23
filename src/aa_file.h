#ifndef AA_FILE_H
#define AA_FILE_H

#include <stdlib.h>
#include "gz.h"

struct _AAFile
{
	GzFile       *gz;
	char         *buf;
	size_t        buf_size;
	unsigned int  eof:1;
};

typedef struct _AAFile AAFile;

struct _AAFileEntry
{
	size_t   class_size;
	size_t   seq_size;
	char    *class;
	char    *seq;
	size_t   num_line;
};

typedef struct _AAFileEntry AAFileEntry;

AAFileEntry * aa_file_entry_new (void);
void          aa_file_entry_free (AAFileEntry *entry);

AAFile      * aa_file_open_for_reading (const char *path);
void          aa_file_close (AAFile *aa_file);

int           aa_file_read (AAFile *aa_file, AAFileEntry *entry);

#endif /* aa_file.h */
