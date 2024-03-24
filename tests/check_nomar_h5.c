#if __has_include("valgrind/valgrind.h")
#include <valgrind/valgrind.h>
#else
#define RUNNING_ON_VALGRIND=0
#endif

#include <check.h>
#include <stdio.h>
#include <unistd.h>
#include "check_nomar.h"

#include "../src/strv.h"
#include "../src/aa_k_mer.h"
#include "../src/h5.h"

START_TEST (test_h5)
{
	if (RUNNING_ON_VALGRIND)
		return;

	const char file[] = "/tmp/ponga.h5";
	H5 *h5 = NULL;

	CountTable *table = NULL;

	size_t i = 0;
	size_t j = 0;
	size_t k = 2;

	const char *label[] = {
		"class1", "class2", "class3", NULL
	};
	char **ptr = (char **) label;

	size_t nrows = strv_length (ptr);
	size_t ncols = aa_k_mer_get_total (k);

	size_t k2 = 0;
	char **label2 = NULL;

	h5 = h5_create (file);
	table = count_table_new (nrows, ncols);

	for (i = 0; i < nrows; i++)
		for (j = 0; j < ncols; j++)
			count_table_set (table, i, j, ncols * i + j);

	h5_write_count_dataset (h5, table, k);
	h5_write_label_dataset (h5, label);

	h5_close (h5);
	count_table_free (table);

	h5 = h5_open (file);
	label2 = h5_read_label_dataset (h5);
	table = h5_read_count_dataset (h5, &k2);

	for (i = 0; i < strv_length (label2); i++)
		ck_assert_str_eq (label2[i], label[i]);

	for (i = 0; i < nrows; i++)
		for (j = 0; j < ncols; j++)
			ck_assert_int_eq (count_table_get (table, i, j),
					ncols * i + j);

	ck_assert_int_eq (k2, k);

	h5_close (h5);
	strv_free (label2);
	count_table_free (table);

	unlink (file);
}
END_TEST

Suite *
make_h5_suite (void)
{
	Suite *s;
	TCase *tc_core;

	s = suite_create ("H5");

	/* Core test case */
	tc_core = tcase_create ("Core");

	tcase_add_test (tc_core, test_h5);

	suite_add_tcase (s, tc_core);

	return s;
}
