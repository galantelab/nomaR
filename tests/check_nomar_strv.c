#include <check.h>
#include <signal.h>
#include "check_nomar.h"

#include "../src/strv.h"

START_TEST (test_strv_builder_new)
{
	StrvBuilder *b = strv_builder_new ();
	ck_assert_ptr_ne (b, NULL);

	char **strv = strv_builder_end (b);
	strv_builder_end (NULL);

	ck_assert_ptr_ne (strv, NULL);
	ck_assert_ptr_eq (strv[0], NULL);

	strv_free (strv);
	strv_free (NULL);
}
END_TEST

START_TEST (test_strv_builder_add)
{
	StrvBuilder *b = NULL;

	char *to_add[] = {
		"um",   "dois", "três", "quatro", "cinco",
		"seis", "sete", "oito", "nove",   "dez"
	};

	char **strv = NULL;
	int i = 0;

	b = strv_builder_new ();

	for (i = 0; i < sizeof (to_add) / sizeof (char *); i++)
		strv_builder_add (b, to_add[i]);

	strv = strv_builder_end (b);
	ck_assert_int_eq (sizeof (to_add) / sizeof (char *),
			strv_length (strv));

	for (i = 0; strv[i] != NULL; i++)
		ck_assert_str_eq (strv[i], to_add[i]);

	strv_free (strv);
}
END_TEST

START_TEST (test_strv_contains)
{
	const char *strv[] = {
		"um",   "dois", "três", "quatro", "cinco",
		"seis", "sete", "oito", "nove",   "dez",
		NULL
	};

	const char *none[] = {NULL};
	size_t i = 0;

	ck_assert_int_eq (strv_contains (strv, "nove", NULL), 1);
	ck_assert_int_eq (strv_contains (strv, "ponga", NULL), 0);

	strv_contains (strv, "dez", &i);
	ck_assert_str_eq (strv[i], "dez");

	ck_assert_int_eq (strv_contains (none, "dois", NULL), 0);
}
END_TEST

START_TEST (test_strv_builder_add_unique)
{
	StrvBuilder *b;
	char **strv = NULL;

	b = strv_builder_new ();

	strv_builder_add_unique (b, "ponga");
	strv_builder_add_unique (b, "ponga");
	strv_builder_add_unique (b, "ponga");

	strv = strv_builder_end (b);
	ck_assert_int_eq (strv_length (strv), 1);
	strv_free (strv);
}
END_TEST

START_TEST (test_strv_contains_fail)
{
	strv_contains (NULL, "ponga", NULL);
}
END_TEST

Suite *
make_strv_suite (void)
{
	Suite *s;
	TCase *tc_core;
	TCase *tc_abort;

	s = suite_create ("Strv");

	/* Core test case */
	tc_core = tcase_create ("Core");

	/* Abort test case */
	tc_abort = tcase_create ("Abort");

	tcase_add_test (tc_core, test_strv_builder_new);
	tcase_add_test (tc_core, test_strv_builder_add);
	tcase_add_test (tc_core, test_strv_builder_add_unique);
	tcase_add_test (tc_core, test_strv_contains);

	tcase_add_test_raise_signal (tc_abort,
			test_strv_contains_fail,
			SIGABRT);

	suite_add_tcase (s, tc_core);
	suite_add_tcase (s, tc_abort);

	return s;
}
