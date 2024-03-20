#include <check.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include "check_nomar.h"

#include "../src/wrapper.h"
#include "../src/utils.h"

START_TEST (test_chomp)
{
	char str[] = "PONGA";
	char str_with_new_line[] = "PONGA\n";

	chomp (str);
	chomp (str_with_new_line);

	ck_assert_str_eq (str, str_with_new_line);
	ck_assert_str_eq (str, "PONGA");
}
END_TEST

START_TEST (test_trimc)
{
	char str_left_padding[] = "###PONGA";
	char str_right_padding[] = "PONGA###";
	char str_left_right_padding[] = "####PONGA#####";

	trimc (str_left_padding, '#');
	trimc (str_right_padding, '#');
	trimc (str_left_right_padding, '#');

	ck_assert_str_eq (str_left_padding, str_right_padding);
	ck_assert_str_eq (str_left_padding, str_left_right_padding);
	ck_assert_str_eq (str_left_padding, "PONGA");
}
END_TEST

START_TEST (test_trim)
{
	char str_left_padding[] = "   PONGA";
	char str_right_padding[] = "PONGA   ";
	char str_left_right_padding[] = "   PONGA   ";

	trim (str_left_padding);
	trim (str_right_padding);
	trim (str_left_right_padding);

	ck_assert_str_eq (str_left_padding, str_right_padding);
	ck_assert_str_eq (str_left_padding, str_left_right_padding);
	ck_assert_str_eq (str_left_padding, "PONGA");
}
END_TEST

START_TEST (test_buf_expand)
{
	char *name = NULL;
	size_t size = 0;
	size_t total_size = 0;

	size = 6;
	name = xcalloc (size, sizeof (char));
	strncpy (name, "ponga", size);
	ck_assert_str_eq (name, "ponga");

	size = buf_expand ((void **) &name,
			sizeof (char), size, 5);

	// 6 + 5 = 11 -> 2^3 == 8 e 2^4 == 16
	total_size = 16;

	strncpy (name, "pongaponga", size);
	ck_assert_str_eq (name, "pongaponga");
	ck_assert_int_eq (size, total_size);

	xfree (name);
}
END_TEST

START_TEST (test_powu)
{
	int base[] = {0, 1, 2};

	int result[][10] = {
		{1, 0, 0, 0,  0,  0,  0,   0,   0,   0},
		{1, 1, 1, 1,  1,  1,  1,   1,   1,   1},
		{1, 2, 4, 8, 16, 32, 64, 128, 256, 512},
	};

	int i, j;

	for (i = 0; i < sizeof (base) / sizeof (int); i++)
		for (j = 0; j < sizeof (result) / sizeof (base); j++)
			ck_assert_int_eq (powu (base[i], j), result[i][j]);
}

START_TEST (test_exists)
{
	char file[] = "/tmp/ponga";
	FILE *fp = NULL;

	fp = fopen (file, "w");
	fprintf (fp, "PONGA\n");
	fclose (fp);

	ck_assert_int_eq (exists (file), 1);
	ck_assert_int_eq (exists ("pongatrongaflofa"), 0);

	unlink (file);
}
END_TEST

START_TEST (test_is_terminal)
{
	ck_assert_int_eq (is_terminal (fileno (stderr)), 0);
}
END_TEST

Suite *
make_utils_suite (void)
{
	Suite *s;
	TCase *tc_core;

	s = suite_create ("Utils");

	/* Core test case */
	tc_core = tcase_create ("Core");

	tcase_add_test (tc_core, test_chomp);
	tcase_add_test (tc_core, test_trimc);
	tcase_add_test (tc_core, test_trim);
	tcase_add_test (tc_core, test_buf_expand);
	tcase_add_test (tc_core, test_powu);
	tcase_add_test (tc_core, test_exists);
	tcase_add_test (tc_core, test_is_terminal);

	suite_add_tcase (s, tc_core);

	return s;
}
