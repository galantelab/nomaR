#ifndef AA_K_MER
#define AA_K_MER

#include <stdint.h>
#include "k_mer.h"

uint32_t aa_k_mer_get_total               (int k);
uint32_t aa_k_mer_get_pos_corrected_total (int k);
uint32_t aa_k_mer_get_index               (const char *k_mer, int k);
uint32_t aa_k_mer_get_pos_corrected_index (const char *k_mer, int k,
		KMerPos pos);

#endif /* aa_k_mer.h */
