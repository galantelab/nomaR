#include <check.h>
#include "check_nomar.h"

START_TEST (test_example)
{
	ck_assert_int_eq (1, 1);
}
END_TEST

Suite *
make_example_suite (void)
{
	Suite *s;
	TCase *tc_core;

	s = suite_create ("Example");

	/* Core test case */
	tc_core = tcase_create ("Core");

	tcase_add_test (tc_core, test_example);

	suite_add_tcase (s, tc_core);

	return s;
}
