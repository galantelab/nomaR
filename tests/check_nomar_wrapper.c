#include <check.h>
#include <stdlib.h>
#include "check_nomar.h"

#include "../src/wrapper.h"

START_TEST (test_xmalloc)
{
	void *p = xmalloc (sizeof (int));
	* (int *) p = 66;
	ck_assert_int_eq (* (int *) p, 66);
	xfree (p);

	p = xmalloc (0);
	xfree (p);
}
END_TEST

START_TEST (test_xcalloc)
{
	void *p = xcalloc (1, sizeof (int));
	ck_assert_int_eq (* (int *) p, 0);
	xfree (p);

	p = xcalloc (0, sizeof (int));
	xfree (p);

	p = xcalloc (1, 0);
	xfree (p);
}
END_TEST

START_TEST (test_xrealloc)
{
	void *p = NULL;

	p = xrealloc (p, sizeof (int));
	* (int *) p = 66;
	ck_assert_int_eq (* (int *) p, 66);

	p = xrealloc (p, sizeof (int) * 2);
	* ((int *) p + 1) = 33;
	ck_assert_int_eq (* ((int *) p + 1), 33);

	xfree (p);
}
END_TEST

START_TEST (test_xstrdup)
{
	char *s = xstrdup ("PONGA");
	ck_assert_str_eq (s, "PONGA");
	xfree (s);
}
END_TEST

START_TEST (test_xfree)
{
	void *p = xmalloc (1);
	xfree (p);
	xfree (NULL);
}
END_TEST

Suite *
make_wrapper_suite (void)
{
	Suite *s;
	TCase *tc_core;

	s = suite_create ("Wrapper");

	/* Core test case */
	tc_core = tcase_create ("Core");

	tcase_add_test (tc_core, test_xmalloc);
	tcase_add_test (tc_core, test_xcalloc);
	tcase_add_test (tc_core, test_xrealloc);
	tcase_add_test (tc_core, test_xstrdup);
	tcase_add_test (tc_core, test_xfree);

	suite_add_tcase (s, tc_core);

	return s;
}
