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
#include "count_fann.h"
#include "fann.h"

#define DEFAULT_PREFIX "out"

struct _FANN
{
	const char *file;
	const char *prefix;
	int         has_k;
	size_t      k;
};

typedef struct _FANN FANN;

static void
run (FANN *f)
{
	log_info ("Generate FANN input file");
	count_fann (f->prefix, f->file, f->k);
}

static void
print_usage (FILE *fp)
{
	fprintf (fp,
		"%s\n"
		"\n"
		"Usage: %s fann [-h] [-p STR] -k INT <FILE>\n"
		"\n"
		"Output:\n"
		"   FANN input file\n"
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
	fprintf (fp, "Try '%s fann --help' for more information\n",
			PACKAGE);
}

static void
fann_print (const FANN *f)
{
	char buf[BUFSIZ] = {};

	snprintf (buf, BUFSIZ,
		">> FANN <<\n"
		"\n"
		"#\n# %s\n#\n\n"
		"# Run %s\n"
		"$ %s Test \\\n"
		"   --k-mer='%zu' \\\n"
		"   --prefix='%s' \\\n"
		"   '%s'\n",
		PACKAGE_STRING, PACKAGE, PACKAGE,
		f->k, f->prefix, f->file);

	buf[BUFSIZ - 1] = '\0';
	log_info ("%s", buf);
}

static int
fann_validate (FANN *f)
{
	int rc = EXIT_SUCCESS;

	/*
	* Validate arguments and mandatory options
	*/

	if (f->file == NULL)
		{
			fprintf (stderr, "%s: Missing file\n", PACKAGE);
			print_try_help (stderr);
			rc = EXIT_FAILURE; goto Exit;
		}

	if (!exists (f->file))
		{
			fprintf (stderr, "%s: file '%s': No such file\n", PACKAGE, f->file);
			rc = EXIT_FAILURE; goto Exit;
		}

	if (!f->has_k)
		{
			fprintf (stderr, "%s: Missing --k-mer option\n", PACKAGE);
			rc = EXIT_FAILURE; goto Exit;
		}

	if (f->k < 0)
		{
			fprintf (stderr, "%s: --k-mer must be greater than 0\n", PACKAGE);
			rc = EXIT_FAILURE; goto Exit;
		}

Exit:
	return rc;
}

int
parse_fann_command_opt (int argc, char **argv)
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

	FANN f = {.prefix = DEFAULT_PREFIX};
	int rc = EXIT_SUCCESS;
	int option_index = 0;
	int c;

	while ((c = getopt_long (argc, argv, "hp:k:", opt, &option_index)) >= 0)
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
						f.prefix = optarg;
						break;
					}
				case 'k':
					{
						f.k = atoi (optarg);
						f.has_k = 1;
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
	f.file = argv[optind + 1];

	// Validate options
	rc = fann_validate (&f);

	// If no error
	if (rc == EXIT_SUCCESS)
		{
			/*
			* RUN FOOLS
			*/
			fann_print (&f);
			run (&f);
		}

Exit:
	return rc;
}
