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
#include "gz.h"
#include "aa.h"
#include "utils.h"
#include "wrapper.h"
#include "train.h"

#define DEFAULT_PREFIX "out"

struct _Train
{
	const char *file;
	const char *prefix;
	int         has_k;
	int         k;
};

typedef struct _Train Train;

static int
get_class_list_and_validate_seq (const char *file, int k, char ***list)
{
#define LIST_ALLOC 8

	assert (file != NULL);
	assert (list != NULL);

	GzFile *gz = NULL;

	size_t num_line = 0;
	char *line = NULL;
	char *saveptr = NULL;

	int num_class = 0;
	int alloc = 0;
	const char *class = NULL;
	const char *seq = NULL;

	gz = gz_open_for_reading (file);

	while (gz_getline (gz, &line, &num_line))
		{
			line = chomp (line);

			class = strtok_r (line, "\t ", &saveptr);
			seq = strtok_r (NULL, "\t ", &saveptr);

			if (class == NULL)
				continue;

			if (seq == NULL)
				{
					log_warn ("CLASS (%s) has no SEQ at line %zu",
							class, num_line);
					continue;
				}

			if (!aa_check (seq))
				log_fatal (
						"CLASS (%s) SEQ (%s) does not seem "
						"to be an amino acid at line %zu",
						class, seq, num_line);

			if (strlen (seq) < k)
				{
					log_warn (
							"CLASS (%s) SEQ (%s) length is "
							"smaller than the size of the "
							"k-mer (%d) at line %zu",
							class, seq, k, num_line);
					continue;
				}

			if (alloc <= num_class)
				alloc = buf_expand ((void *) *list, sizeof (char *),
						alloc, LIST_ALLOC);

			/*if (!linear_str_search ((const char **) *list, num_class, class, NULL))*/
				/**list[num_class++] = xstrdup (class);*/
		}

	if (alloc > num_class)
		*list = xrealloc (*list, sizeof (char *) * num_class);

	xfree (line);
	gz_close (gz);

#undef LIST_ALLOC
	return num_class;
}

static void
run (Train *t)
{
	int num_class = 0;
	char **list = NULL;

	log_info ("Validating file '%s'", t->file);
	num_class = get_class_list_and_validate_seq (t->file, t->k, &list);

	if (num_class == 0)
		log_fatal ("Empty file '%s' or no valid entry found",
				t->file);
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
		"   --k-mer='%d' \\\n"
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
