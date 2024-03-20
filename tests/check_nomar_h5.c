#include <check.h>
#include "check_nomar.h"

#include "../src/h5.h"

START_TEST (test_h5_create)
{
	const char file[] = "ponga.h5";
	H5 *h5 = NULL;

	h5 = h5_create (file, 2, 2);
	h5_close (h5);
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

	tcase_add_test (tc_core, test_h5_create);

	suite_add_tcase (s, tc_core);

	return s;
}
