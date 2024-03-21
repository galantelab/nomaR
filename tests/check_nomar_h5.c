#if __has_include("valgrind/valgrind.h")
#include <valgrind/valgrind.h>
#else
#define RUNNING_ON_VALGRIND=0
#endif

#include <check.h>
#include <unistd.h>
#include "check_nomar.h"

#include "../src/aa_k_mer.h"
#include "../src/h5.h"

START_TEST (test_h5_create)
{
	if (RUNNING_ON_VALGRIND)
		return;

	const char file[] = "/tmp/ponga.h5";
	H5 *h5 = NULL;

	int i = 0;
	int j = 0;
	int k = 2;

	const char *label[] = {
		"class1", "class2", "class3"
	};

	int x_len = sizeof (label) / sizeof (char *);
	int y_len = aa_k_mer_get_total (k);

	uint32_t counts[x_len * y_len];

	h5 = h5_create (file, x_len, y_len);

	for (i = 0; i < x_len; i++)
		for (j = 0; j < y_len; j++)
			counts[y_len * i + j] = y_len * i + j;

	h5_write_count_dataset (h5, counts);
	h5_write_label_dataset (h5, label);

	h5_close (h5);
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

	tcase_add_test (tc_core, test_h5_create);

	suite_add_tcase (s, tc_core);

	return s;
}
