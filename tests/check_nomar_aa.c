#include <check.h>
#include "check_nomar.h"

#include "../src/aa.h"

START_TEST (test_aa_data)
{
	int i = 0;

	for (; i < AA_LEN; i++)
		{
			ck_assert_ptr_nonnull (aa_data[i].name);
			ck_assert_ptr_nonnull (aa_data[i].sym3);
			ck_assert_int_gt (aa_data[i].sym1, 0);
		}
}
END_TEST

START_TEST (test_aa_get_sym1_index)
{
	char aa[] = {
		'A', 'R', 'N', 'D', 'C',
		'E', 'Q', 'G', 'H', 'I',
		'L', 'K', 'M', 'F', 'P',
		'S', 'T', 'W', 'Y', 'V'
	};

	int i = 0;
	char sym1 = 0;

	for (; i < AA_LEN; i++)
		{
			sym1 = aa_data[aa_get_sym1_index (aa[i])].sym1;
			ck_assert_int_eq (aa[i], sym1);
		}
}
END_TEST

Suite *
make_aa_suite (void)
{
	Suite *s;
	TCase *tc_core;

	s = suite_create ("AA");

	/* Core test case */
	tc_core = tcase_create ("Core");

	tcase_add_test (tc_core, test_aa_data);
	tcase_add_test (tc_core, test_aa_get_sym1_index);

	suite_add_tcase (s, tc_core);

	return s;
}

