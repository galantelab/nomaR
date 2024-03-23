#include <check.h>
#include <stdio.h>
#include <unistd.h>
#include "check_nomar.h"

#include "../src/aa.h"
#include "../src/aa_k_mer.h"
#include "../src/count_k_mer.h"

static void
create_test_file (const char *file)
{
	FILE *fp = NULL;

	fp = fopen (file, "w");

	fprintf (fp,
		"PONGA\tRCADMDPSWPLIWKGMPANFHIPC\n"
		"GAPON\tGYHPHTFTGEGPMGQM\n"
		"PONGA\tIFVWNYIHRPAGESAK\n"
		"GAPON\tDHLWLVGFKND\n"
		"PONGA\tNEAIFRGIP\n"
		"GAPON\tAAYQCTRANKQCICFVY\n"
		"PONGA\tVANHFSNFKQLYQSNIW\n"
		"GAPON\tVTYGCYTMKMW\n"
		"PONGA\tGINKSKREDKSN\n"
		"GAPON\tSLGANQCCYHSYAMDANDKKG\n"
	);

	fclose (fp);
}

START_TEST (test_count_k_mer_freq)
{
	const char file[] = "/tmp/ponga.txt";

	CountKMer *ck = NULL;
	int k = 1;
	int i = 0;
	int j = 0;

	const char *label[] = {
		"PONGA",
		"GAPON"
	};
	int num_label = sizeof (label) / sizeof (char *);

	uint32_t tp[][AA_LEN] = {
		{6, 4, 8, 3, 2, 3, 2, 4, 3, 8, 2, 6, 2, 5, 6, 6, 0, 4, 2, 2},
		{6, 1, 4, 4, 6, 1, 4, 8, 4, 1, 3, 5, 5, 3, 2, 2, 5, 2, 7, 3}
	};

	create_test_file (file);

	ck = count_k_mer (file, k);

	ck_assert_ptr_nonnull (ck);
	ck_assert_ptr_nonnull (ck->table);
	ck_assert_ptr_nonnull (ck->label);

	ck_assert_int_eq (count_table_get_nrows (ck->table), num_label);
	ck_assert_int_eq (count_table_get_ncols (ck->table),
			aa_k_mer_get_pos_corrected_total (k));

	for (i = 0; i < count_table_get_nrows (ck->table); i++)
		{
			ck_assert_str_eq (ck->label[i], label[i]);
			for (j = 0; j < count_table_get_ncols (ck->table); j++)
				ck_assert_int_eq (count_table_get (ck->table, i, j), tp[i][j]);
		}

	count_k_mer_free (ck);
	unlink (file);
}
END_TEST

Suite *
make_count_k_mer_suite (void)
{
	Suite *s;
	TCase *tc_core;

	s = suite_create ("CountKMer");

	/* Core test case */
	tc_core = tcase_create ("Core");

	tcase_add_test (tc_core, test_count_k_mer_freq);

	suite_add_tcase (s, tc_core);

	return s;
}
