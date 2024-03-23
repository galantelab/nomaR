#include <check.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include "check_nomar.h"

#include "../src/aa_file.h"

static char *file_err[] = {
	" ",
	"ponga1 ",
	"ponga1 this_is_not_aa\n"
};

static char file_ok[] =
	"AAA\tCASSKPQGYYGYTF\n"
	"AAA\tCASRKRHLSGANVLTF\n"
	"AAA\tCASVSGHPTQSRANVLTF\n"
	"AAA\tCASSDLQYTDTQYF\n"
	"AAA\tCASSDRADNQPQHF\n"
	"AAA\tCASSPPLSYEQYF\n"
	"BBB\tCASSLELDKVHNEKLFF\n"
	"BBB\tCASSVRHFVLDEQYF\n"
	"BBB\tCASSFSPKAGPHEQYF\n"
	"BBB\tCASSQDGGRSSYEQYF\n";

static void
create_file (const char *path, const char *txt)
{
	FILE *fp = fopen (path, "w");
	fprintf (fp, txt);
	fclose (fp);
}

START_TEST (test_aa_file_open)
{
	char file[] = "/tmp/ponga.tsv";
	AAFile *aa_file = NULL;

	create_file (file, "PONGA\n");

	aa_file = aa_file_open_for_reading (file);
	aa_file_close (aa_file);
	aa_file_close (NULL);

	unlink (file);
}
END_TEST

START_TEST (test_aa_file_read_err)
{
	char file[] = "/tmp/ponga.tsv";

	AAFile *aa_file = NULL;
	AAFileEntry *entry = NULL;

	create_file (file, file_err[_i]);

	aa_file = aa_file_open_for_reading (file);
	entry = aa_file_entry_new ();

	while (aa_file_read (aa_file, entry))
		;

	aa_file_entry_free (entry);
	aa_file_close (aa_file);

	unlink (file);
}
END_TEST

START_TEST (test_aa_file_read_ok)
{
	char file[] = "/tmp/ponga.tsv";

	AAFile *aa_file = NULL;
	AAFileEntry *entry = NULL;

	size_t acm = 0;

	create_file (file, file_ok);

	aa_file = aa_file_open_for_reading (file);
	entry = aa_file_entry_new ();

	while (aa_file_read (aa_file, entry))
		{
			ck_assert_ptr_nonnull (entry->class);
			ck_assert_ptr_nonnull (entry->seq);
			ck_assert_int_eq (entry->num_line, ++ acm);
		}

	aa_file_entry_free (entry);
	aa_file_close (aa_file);

	unlink (file);
}
END_TEST

Suite *
make_aa_file_suite (void)
{
	Suite *s;
	TCase *tc_core;
	TCase *tc_abort;

	s = suite_create ("AAFile");

	/* Core test case */
	tc_core = tcase_create ("Core");

	/* Abort test case */
	tc_abort = tcase_create ("Abort");

	tcase_add_test (tc_core, test_aa_file_open);
	tcase_add_test (tc_core, test_aa_file_read_ok);

	tcase_add_loop_test_raise_signal (tc_abort,
			test_aa_file_read_err,
			SIGABRT,
			0,
			sizeof (file_err) / sizeof (char *)
	);

	suite_add_tcase (s, tc_core);
	suite_add_tcase (s, tc_abort);

	return s;
}
