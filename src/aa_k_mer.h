#ifndef AA_K_MER
#define AA_K_MER

#include <stdlib.h>
#include "k_mer.h"

size_t aa_k_mer_get_total               (size_t k);
size_t aa_k_mer_get_pos_corrected_total (size_t k);
size_t aa_k_mer_get_index               (const char *k_mer, size_t k);
size_t aa_k_mer_get_pos_corrected_index (const char *k_mer, size_t k,
		KMerPos pos);

#endif /* aa_k_mer.h */
