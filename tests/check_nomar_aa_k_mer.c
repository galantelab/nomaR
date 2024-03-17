#include <check.h>
#include <signal.h>
#include "check_nomar.h"

#include "../src/aa_k_mer.h"

START_TEST (test_aa_k_mer_get_index_fail1)
{
	aa_k_mer_get_index (NULL, 1);
}

START_TEST (test_aa_k_mer_get_index_fail2)
{
	aa_k_mer_get_index ("AAA", 0);
}

START_TEST (test_aa_k_mer_get_pos_corrected_index_fail1)
{
	aa_k_mer_get_pos_corrected_index (NULL, 1, 1);
}

START_TEST (test_aa_k_mer_get_pos_corrected_index_fail2)
{
	aa_k_mer_get_pos_corrected_index ("AAA", 0, 1);
}

START_TEST (test_aa_k_mer_get_index)
{
	int i = 0;
	int k = 2;

	char *k_mer[] = {
		"AR", "RN", "ND", "DC"
	};

	int pos[] = {
		1, 22, 43, 64
	};

	for (; i < sizeof (pos) / sizeof (int); i++)
		ck_assert_int_eq (aa_k_mer_get_index (k_mer[i], k), pos[i]);
}
END_TEST

START_TEST (test_aa_k_mer_get_pos_corrected_index)
{
	int i = 0;
	int j = 0;
	int k = 5;

	char *k_mer_left[] = {
		"A", "VVVV"
	};

	int result_left[] = {
		0, 168419
	};

	char *k_mer_internal[] = {
		"AAAAA", "VVVVV"
	};

	int result_internal[] = {
		168420, 3368419
	};

	char *k_mer_right[] = {
		"AAAA", "AAA",
		"AA",   "VV",
		"A",    "V"
	};

	int result_right[] = {
		3368420, 3528420,
		3536420, 3536819,
		3536820, 3536839
	};

	int num_k[] = {
		2, 2, 6
	};

	KMerPos pos[] = {
		K_MER_LEFT_EDGE,
		K_MER_INTERNAL,
		K_MER_RIGHT_EDGE
	};

	char **k_mers[] = {
		k_mer_left,
		k_mer_internal,
		k_mer_right
	};

	int *results[] = {
		result_left,
		result_internal,
		result_right
	};

	for (i = 0; i < sizeof (k_mers) / sizeof (char **); i++)
		{
			char **k_mer = k_mers[i];
			int *result = results[i];

			for (j = 0; j < num_k[i]; j++)
				ck_assert_int_eq (
						aa_k_mer_get_pos_corrected_index (k_mer[j], k, pos[i]),
						result[j]
				);
		}
}

Suite *
make_aa_k_mer_suite (void)
{
	Suite *s;
	TCase *tc_core;
	TCase *tc_abort;

	s = suite_create ("AA_KMer");

	/* Core test case */
	tc_core = tcase_create ("Core");

	/* Abort test case */
	tc_abort = tcase_create ("Abort");

	tcase_add_test (tc_core, test_aa_k_mer_get_index);
	tcase_add_test (tc_core, test_aa_k_mer_get_pos_corrected_index);

	tcase_add_test_raise_signal (tc_abort,
			test_aa_k_mer_get_index_fail1,
			SIGABRT);

	tcase_add_test_raise_signal (tc_abort,
			test_aa_k_mer_get_index_fail2,
			SIGABRT);

	tcase_add_test_raise_signal (tc_abort,
			test_aa_k_mer_get_pos_corrected_index_fail1,
			SIGABRT);

	tcase_add_test_raise_signal (tc_abort,
			test_aa_k_mer_get_pos_corrected_index_fail2,
			SIGABRT);

	suite_add_tcase (s, tc_core);
	suite_add_tcase (s, tc_abort);

	return s;
}
