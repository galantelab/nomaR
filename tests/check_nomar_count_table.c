#include <check.h>
#include <signal.h>
#include "check_nomar.h"

#include "../src/count_table.h"

static CountTable *ct = NULL;
static size_t ct_ncols = 10;

static void
setup (void)
{
	ct = count_table_new (ct_ncols);
}

static void
teardown (void)
{
	count_table_free (ct);
}

START_TEST (test_count_table_new_fail)
{
	count_table_new (0);
}
END_TEST

START_TEST (test_count_table_get_ncols_fail)
{
	count_table_get_ncols (NULL);
}
END_TEST

START_TEST (test_count_table_get_nrows_fail)
{
	count_table_get_nrows (NULL);
}
END_TEST

START_TEST (test_count_table_get_dim_fail1)
{
	count_table_get_dim (ct, NULL);
}
END_TEST

START_TEST (test_count_table_get_dim_fail2)
{
	count_table_get_dim (NULL, NULL);
}
END_TEST

START_TEST (test_count_table_add_row_fail1)
{
	count_table_add_row (ct, 0);
}
END_TEST

START_TEST (test_count_table_add_row_fail2)
{
	count_table_add_row (NULL, 0);
}
END_TEST

START_TEST (test_count_table_set_fail1)
{
	count_table_set (NULL, 100, 100, 666);
}
END_TEST

START_TEST (test_count_table_set_fail2)
{
	count_table_set (ct, 100, 100, 666);
}
END_TEST

START_TEST (test_count_table_set_fail3)
{
	count_table_add_row (ct, 1);
	count_table_set (ct, 0, 100, 666);
}
END_TEST

START_TEST (test_count_table_get_fail1)
{
	count_table_get (NULL, 100, 100);
}
END_TEST

START_TEST (test_count_table_get_fail2)
{
	count_table_get (ct, 100, 100);
}
END_TEST

START_TEST (test_count_table_get_fail3)
{
	count_table_add_row (ct, 1);
	count_table_get (ct, 0, 100);
}
END_TEST

START_TEST (test_count_table_new)
{
	ck_assert_ptr_nonnull (ct);
	ck_assert_int_eq (count_table_get_ncols (ct), 10);
	ck_assert_int_eq (count_table_get_nrows (ct), 0);
	count_table_free (NULL);
}
END_TEST

START_TEST (test_count_table_add_row)
{
	size_t i = 0;

	ck_assert_int_eq (count_table_get_nrows (ct), 0);

	for (i = 0; i < ct_ncols; i++)
		{
			count_table_add_row (ct, 1);
			ck_assert_int_eq (count_table_get_nrows (ct), i + 1);
		}
}
END_TEST

START_TEST (test_count_table_set)
{
	size_t i = 0;
	size_t j = 0;

	count_table_add_row (ct, ct_ncols);

	for (i = 0; i < ct_ncols; i++)
		for (j = 0; j < ct_ncols; j++)
			count_table_set (ct, i, j, i * ct_ncols + j);

	for (i = 0; i < ct_ncols; i++)
		for (j = 0; j < ct_ncols; j++)
			ck_assert_int_eq (count_table_get (ct, i, j), i * ct_ncols + j);
}
END_TEST

Suite *
make_count_table_suite (void)
{
	Suite *s;
	TCase *tc_core;
	TCase *tc_abort;

	s = suite_create ("CountTable");

	/* Core test case */
	tc_core = tcase_create ("Core");

	/* Abort test case */
	tc_abort = tcase_create ("Abort");

	tcase_add_checked_fixture (tc_core, setup, teardown);
	tcase_add_test (tc_core, test_count_table_new);
	tcase_add_test (tc_core, test_count_table_add_row);
	tcase_add_test (tc_core, test_count_table_set);

	tcase_add_test_raise_signal (tc_abort,
			test_count_table_new_fail,
			SIGABRT);

	tcase_add_test_raise_signal (tc_abort,
			test_count_table_get_ncols_fail,
			SIGABRT);

	tcase_add_test_raise_signal (tc_abort,
			test_count_table_get_nrows_fail,
			SIGABRT);

	tcase_add_test_raise_signal (tc_abort,
			test_count_table_get_dim_fail1,
			SIGABRT);

	tcase_add_test_raise_signal (tc_abort,
			test_count_table_get_dim_fail2,
			SIGABRT);

	tcase_add_test_raise_signal (tc_abort,
			test_count_table_add_row_fail1,
			SIGABRT);

	tcase_add_test_raise_signal (tc_abort,
			test_count_table_add_row_fail2,
			SIGABRT);

	tcase_add_test_raise_signal (tc_abort,
			test_count_table_set_fail1,
			SIGABRT);

	tcase_add_test_raise_signal (tc_abort,
			test_count_table_set_fail2,
			SIGABRT);

	tcase_add_test_raise_signal (tc_abort,
			test_count_table_set_fail3,
			SIGABRT);

	tcase_add_test_raise_signal (tc_abort,
			test_count_table_get_fail1,
			SIGABRT);

	tcase_add_test_raise_signal (tc_abort,
			test_count_table_get_fail2,
			SIGABRT);

	tcase_add_test_raise_signal (tc_abort,
			test_count_table_get_fail3,
			SIGABRT);

	suite_add_tcase (s, tc_core);
	suite_add_tcase (s, tc_abort);

	return s;
}
