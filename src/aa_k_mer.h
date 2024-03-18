#ifndef AA_K_MER
#define AA_K_MER

#include "k_mer.h"

unsigned int aa_k_mer_get_total               (int k);
unsigned int aa_k_mer_get_pos_corrected_total (int k);
unsigned int aa_k_mer_get_index               (const char *k_mer, int k);
unsigned int aa_k_mer_get_pos_corrected_index (const char *k_mer, int k,
		KMerPos pos);

#endif /* aa_k_mer.h */
