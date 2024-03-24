#ifdef HAVE_CONFIG_H
#include "config.h"
#else
#define PACKAGE "ponga"
#define PACKAGE_STRING "ponga 0.0.0"
#endif

#include <stdio.h>
#include <stdlib.h>
#include <getopt.h>
#include <assert.h>

#include "log.h"
#include "utils.h"
#include "strv.h"
#include "count_k_mer.h"
#include "h5.h"
#include "train.h"

#define DEFAULT_PREFIX "out"

struct _Train
{
	const char *file;
	const char *prefix;
	int         has_k;
	size_t      k;
};

typedef struct _Train Train;

static void
run (Train *t)
{
	CountKMer *ck = NULL;

	H5 *h5 = NULL;
	char h5_file[BUFSIZ] = {};

	log_info ("Count amino acid k-mer's of length '%zu' in '%s'", t->k, t->file);
	ck = count_k_mer (t->file,  t->k);

	if (strv_length (ck->label) == 0)
		log_fatal ("Empty file '%s' or no valid entry found",
				t->file);

	snprintf (h5_file, BUFSIZ, "%s.h5", t->prefix);

	log_info ("Create h5 file '%s'", h5_file);
	h5 = h5_create (h5_file);

	log_info ("Dump count dataset to '%s'", h5_file);
	h5_write_count_dataset (h5, ck->table, t->k);

	log_info ("Dump label dataset to '%s'", h5_file);
	h5_write_label_dataset (h5, (const char **) ck->label);

	log_info ("File '%s' is ready!", h5_file);

	count_k_mer_free (ck);
	h5_close (h5);
}

static void
print_usage (FILE *fp)
{
	fprintf (fp,
		"%s\n"
		"\n"
		"Usage: %s train [-h] [-p STR] -k INT <FILE>\n"
		"\n"
		"Output:\n"
		"   A HDF5 database with k-mer counts\n"
		"\n"
		"Arguments:\n"
		"   A file with two columns: CLASS and SEQ\n"
		"\n"
		"Mandatory Options:\n"
		"   -k, --k-mer             The k-mer length\n"
		"\n"
		"Options:\n"
		"   -h, --help              Show this help and exit\n"
		"   -p, --prefix            Prefix for the output file\n"
		"                           [default: %s]\n"
		"\n",
		PACKAGE_STRING, PACKAGE, DEFAULT_PREFIX);
}

static void
print_try_help (FILE *fp)
{
	fprintf (fp, "Try '%s train --help' for more information\n",
			PACKAGE);
}

static void
train_print (const Train *t)
{
	char buf[BUFSIZ] = {};

	snprintf (buf, BUFSIZ,
		">> Train <<\n"
		"\n"
		"#\n# %s\n#\n\n"
		"# Run %s\n"
		"$ %s train \\\n"
		"   --k-mer='%zu' \\\n"
		"   --prefix='%s' \\\n"
		"   '%s'\n",
		PACKAGE_STRING, PACKAGE, PACKAGE,
		t->k, t->prefix, t->file);

	buf[BUFSIZ - 1] = '\0';
	log_info ("%s", buf);
}

static int
train_validate (Train *t)
{
	int rc = EXIT_SUCCESS;

	/*
	* Validate arguments and mandatory options
	*/

	if (t->file == NULL)
		{
			fprintf (stderr, "%s: Missing file\n", PACKAGE);
			print_try_help (stderr);
			rc = EXIT_FAILURE; goto Exit;
		}

	if (!exists (t->file))
		{
			fprintf (stderr, "%s: file '%s': No such file\n", PACKAGE, t->file);
			rc = EXIT_FAILURE; goto Exit;
		}

	if (!t->has_k)
		{
			fprintf (stderr, "%s: Missing --k-mer option\n", PACKAGE);
			rc = EXIT_FAILURE; goto Exit;
		}

	if (t->k < 0)
		{
			fprintf (stderr, "%s: --k-mer must be greater than 0\n", PACKAGE);
			rc = EXIT_FAILURE; goto Exit;
		}

Exit:
	return rc;
}

int
parse_train_command_opt (int argc, char **argv)
{
	assert (argc > 1 && argv != NULL && *argv != NULL);

	// No options or arguments
	// Print usage
	if (argc == 2)
		{
			print_usage (stdout);
			return EXIT_SUCCESS;
		}

	struct option opt[] =
	{
		{"help",    no_argument,       0, 'h'},
		{"k-mer",   required_argument, 0, 'k'},
		{"prefix",  required_argument, 0, 'p'},
		{0,         0,                 0,  0 }
	};

	Train t = {.prefix = DEFAULT_PREFIX};
	int rc = EXIT_SUCCESS;
	int option_index = 0;
	int c;

	while ((c = getopt_long (argc, argv, "hk:p:", opt, &option_index)) >= 0)
		{
			switch (c)
				{
				case 'h':
					{
						print_usage (stdout);
						goto Exit;
						break;
					}
				case 'p':
					{
						t.prefix = optarg;
						break;
					}
				case 'k':
					{
						t.k = atoi (optarg);
						t.has_k = 1;
						break;
					}
				case '?':
				case ':':
					{
						print_try_help (stderr);
						rc = EXIT_FAILURE; goto Exit;
						break;
					}
				}
		}

	// Get file as arg
	t.file = argv[optind + 1];

	// Validate options
	rc = train_validate (&t);

	// If no error
	if (rc == EXIT_SUCCESS)
		{
			/*
			* RUN FOOLS
			*/
			train_print (&t);
			run (&t);
		}

Exit:
	return rc;
}
