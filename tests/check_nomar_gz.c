#include <check.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include "check_nomar.h"

#include "../src/wrapper.h"
#include "../src/gz.c"

static void
create_file (const char *cnt, char *path)
{
	FILE *fp = NULL;

	fp = fopen (path, "w");
	fprintf (fp, "%s", cnt);

	fclose (fp);
}

static void
create_big_file (char *path)
{
	FILE *fp = NULL;
	int i, j;

	fp = fopen (path, "w");

	for  (i = 0; i < 10; i++)
		{
			for  (j = 0; j < 10000; j++)
				fprintf (fp, "ponga");
			fprintf (fp, "\n");
		}

	fclose (fp);
}

static void
create_long_line (char *path)
{
	FILE *fp = NULL;
	int i;

	fp = fopen (path, "w");

	for  (i = 0; i < 10000; i++)
		fprintf (fp, "ponga");

	fclose (fp);
}

START_TEST (test_open_fatal)
{
	char gz_path[] = "/tmp/ponga.txt.1";
	gz_open_for_reading (gz_path);
}
END_TEST

START_TEST (test_read1)
{
	GzFile *gz = NULL;
	char gz_path[] = "/tmp/ponga.txt.5";
	char *line = NULL;
	size_t n = 0;

	create_big_file (gz_path);
	gz = gz_open_for_reading (gz_path);

	while (gz_getline (gz, &line, &n))
		;

	xfree (line);
	gz_close (gz);
	unlink (gz_path);
}
END_TEST

START_TEST (test_read2)
{
	GzFile *gz = NULL;
	char gz_path[] = "/tmp/ponga.txt.6";
	char *line = NULL;
	int i = 0;
	int l = 0;
	size_t n = 0;

	const char *gz_cnt_ex =
		"=> 1 ponga\n"
		"=> 2 ponga\n"
		"=> 3 ponga\n"
		"=> 4 ponga\n"
		"=> 5 ponga\n"
		"=> 6 ponga\n"
		"=> 7 ponga\n"
		"=> 8 ponga\n"
		"=> 9 ponga\n"
		"=> 10 ponga\n";

	create_file (gz_cnt_ex, gz_path);
	gz = gz_open_for_reading (gz_path);

	while (gz_getline (gz, &line, &n))
		{
			sscanf (line, "%*s %d", &l);
			ck_assert_int_eq (l, ++i);
		}

	ck_assert_int_eq (gz_get_num_line (gz), i);
	ck_assert_str_eq (gz_get_filename (gz), gz_path);

	xfree (line);
	gz_close (gz);
	unlink (gz_path);
}
END_TEST

START_TEST (test_read_long_line)
{
	GzFile *gz = NULL;
	char gz_path[] = "/tmp/ponga.txt.7";
	char *line = NULL;
	size_t n = 0;

	create_long_line (gz_path);
	gz = gz_open_for_reading (gz_path);

	while (gz_getline (gz, &line, &n))
		;

	xfree (line);
	gz_close (gz);
	unlink (gz_path);
}
END_TEST

Suite *
make_gz_suite (void)
{
	Suite *s;
	TCase *tc_core;
	TCase *tc_abort;

	s = suite_create ("GZ");

	/* Core test case */
	tc_core = tcase_create ("Core");

	/* Abort test case */
	tc_abort = tcase_create ("Abort");

	tcase_add_test (tc_core, test_read1);
	tcase_add_test (tc_core, test_read2);
	tcase_add_test (tc_core, test_read_long_line);

	tcase_add_exit_test (tc_abort,
			test_open_fatal,  EXIT_FAILURE);

	suite_add_tcase (s, tc_core);
	suite_add_tcase (s, tc_abort);

	return s;
}
