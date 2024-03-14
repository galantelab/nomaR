#include <check.h>
#include <signal.h>
#include "check_nomar.h"

#include "../src/k_mer.h"

START_TEST (test_k_mer_iter_init_fail1)
{
	k_mer_iter_init (NULL, NULL, 0);
}
END_TEST

START_TEST (test_k_mer_iter_init_fail2)
{
	KMerIter iter;
	k_mer_iter_init (&iter, NULL, 0);
}
END_TEST

START_TEST (test_k_mer_iter_init_fail3)
{
	KMerIter iter;
	k_mer_iter_init (&iter, "", 0);
}
END_TEST

START_TEST (test_k_mer_iter_init_fail4)
{
	KMerIter iter;
	k_mer_iter_init (&iter, "ponga", 0);
}
END_TEST

START_TEST (test_k_mer_iter_next)
{
	char seq[] = "ABCDEFGHIJ";
	char *k_mers[] = {
		"A", "AB", "ABC", "ABCD",
		"ABCDE", "BCDEF", "CDEFG",
		"DEFGH", "EFGHI", "FGHIJ",
		"GHIJ", "HIJ", "IJ", "J"
	};

	int i = 0;
	int k = 5;
	char k_mer[k + 1] = {};

	KMerIter iter;
	k_mer_iter_init (&iter, seq, k);

	while (k_mer_iter_next (&iter, k_mer))
		ck_assert_str_eq (k_mer, k_mers[i++]);
}
END_TEST

START_TEST (test_k_mer_iter_count)
{
	char seq[] = "ABCDEFGHIJ";
	int seq_len = sizeof (seq) / sizeof (char) - 1;
	int count = (seq_len - _i + 1) + (2 * (_i - 1));
	int acm = 0;

	KMerIter iter;
	k_mer_iter_init (&iter, seq, _i);

	while (k_mer_iter_next (&iter, NULL))
		acm++;

	ck_assert_int_eq (count, acm);
}
END_TEST

Suite *
make_k_mer_suite (void)
{
	Suite *s;
	TCase *tc_core;
	TCase *tc_abort;

	s = suite_create ("KMer");

	/* Core test case */
	tc_core = tcase_create ("Core");

	/* Abort test case */
	tc_abort = tcase_create ("Abort");

	tcase_add_test (tc_core, test_k_mer_iter_next);
	tcase_add_loop_test (tc_core, test_k_mer_iter_count, 1, 10);

	tcase_add_test_raise_signal (tc_abort,
			test_k_mer_iter_init_fail1,
			SIGABRT);

	tcase_add_test_raise_signal (tc_abort,
			test_k_mer_iter_init_fail2,
			SIGABRT);

	tcase_add_test_raise_signal (tc_abort,
			test_k_mer_iter_init_fail3,
			SIGABRT);

	tcase_add_test_raise_signal (tc_abort,
			test_k_mer_iter_init_fail4,
			SIGABRT);

	suite_add_tcase (s, tc_core);
	suite_add_tcase (s, tc_abort);

	return s;
}
