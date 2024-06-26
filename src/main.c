#ifdef HAVE_CONFIG_H
#include "config.h"
#else
#define PACKAGE "ponga"
#define PACKAGE_STRING "ponga 0.0.0"
#endif

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <getopt.h>
#include <assert.h>

#include "log.h"
#include "utils.h"
#include "train.h"
#include "test.h"
#include "fann.h"

static void
print_version (FILE *fp)
{
	fprintf (fp, "%s\n", PACKAGE_STRING);
}

static void
print_usage (FILE *fp)
{
	fprintf (fp,
		"%s\n"
		"\n"
		"Usage: %s [-hv]\n"
		"       %s <command> [options]\n"
		"\n"
		"Options:\n"
		"   -h, --help      Show help options\n"
		"   -V, --version   Show current version\n"
		"\n"
		"Commands:\n"
		"   train           Train model\n"
		"   test            Test a model\n"
		"   fann            Generate FANN input\n"
		"\n",
		PACKAGE_STRING, PACKAGE, PACKAGE);
}

static void
print_try_help (FILE *fp)
{
	fprintf (fp, "Try '%s  --help' for more information\n",
			PACKAGE);
}

static int
parse_no_command_opt (int argc, char **argv)
{
	assert (argc > 1 && argv != NULL && *argv != NULL);

	struct option opt[] =
	{
		{"version", no_argument, 0, 'V'},
		{"help",    no_argument, 0, 'h'},
		{0,         0,           0,  0 }
	};

	int rc = EXIT_SUCCESS;
	int option_index = 0;
	int c;

	while ((c = getopt_long (argc, argv, "Vh", opt, &option_index)) >= 0)
		{
			switch (c)
				{
				case 'V':
					{
						print_version (stdout);
						goto Exit;
						break;
					}
				case 'h':
					{
						print_usage (stdout);
						goto Exit;
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

Exit:
	return rc;
}

static void
set_log (void)
{
	if (!is_terminal (fileno (stderr)))
		log_set_color (0);
}

int
main (int argc, char *argv[])
{
	int rc = EXIT_SUCCESS;

	set_log ();

	if (argc == 1)
		print_usage (stdout);
	else if (argv[1][0] == '-')
		rc = parse_no_command_opt (argc, argv);
	else if (!strcmp (argv[1], "train"))
		rc = parse_train_command_opt (argc, argv);
	else if (!strcmp (argv[1], "test"))
		rc = parse_test_command_opt (argc, argv);
	else if (!strcmp (argv[1], "fann"))
		rc = parse_fann_command_opt (argc, argv);
	else
		{
			fprintf (stderr, "%s: '%s' is not a valid command\n", PACKAGE, argv[1]);
			print_try_help (stderr);
			rc = EXIT_FAILURE;
		}

	return rc;
}
