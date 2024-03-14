#include <stdlib.h>
#include <check.h>
#include "check_nomar.h"

int
main (void)
{
	int number_failed = 0;
	SRunner *sr = NULL;

	// Create runner
	sr = srunner_create (make_example_suite ());
	srunner_add_suite (sr, make_aa_suite ());
	srunner_add_suite (sr, make_k_mer_suite ());
	srunner_set_tap (sr, "-");

	srunner_run_all (sr, CK_NORMAL);
	number_failed = srunner_ntests_failed (sr);
	srunner_free (sr);

	return number_failed;
}
