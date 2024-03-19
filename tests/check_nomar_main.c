#include <check.h>
#include "check_nomar.h"

#include "../src/log.h"

int
main (void)
{
	log_set_quiet (1);

	int number_failed = 0;
	SRunner *sr = NULL;

	// Create runner
	sr = srunner_create (make_example_suite ());
	srunner_add_suite (sr, make_aa_suite ());
	srunner_add_suite (sr, make_k_mer_suite ());
	srunner_add_suite (sr, make_utils_suite ());
	srunner_add_suite (sr, make_wrapper_suite ());
	srunner_add_suite (sr, make_gz_suite ());
	srunner_add_suite (sr, make_aa_k_mer_suite ());
	srunner_add_suite (sr, make_count_k_mer_suite ());
	srunner_set_tap (sr, "-");

	srunner_run_all (sr, CK_ENV);
	number_failed = srunner_ntests_failed (sr);
	srunner_free (sr);

	return number_failed;
}
