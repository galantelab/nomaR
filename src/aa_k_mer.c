#include <string.h>
#include <assert.h>

#include "utils.h"
#include "aa.h"
#include "aa_k_mer.h"

static inline size_t
aa_k_mer_edge_correction (size_t i, size_t n)
{
	size_t addend = 0;

	for (; i <= n; i++)
		addend += powu (AA_LEN, i);

	return addend;
}

size_t
aa_k_mer_get_total (size_t k)
{
	assert (k > 0);
	return powu (AA_LEN, k);
}

size_t
aa_k_mer_get_pos_corrected_total (size_t k)
{
	assert (k > 0);
	return
		powu (AA_LEN, k) +
		2 * aa_k_mer_edge_correction (1, k - 1);
}

size_t
aa_k_mer_get_index (const char *k_mer, size_t k)
{
	assert (k_mer != NULL);
	assert (k > 0);

	size_t index = 0;
	size_t i = 0;

	for (; i < k; i++)
		index += aa_get_sym1_index (k_mer[i])
			* powu (AA_LEN, k - i - 1);

	return index;
}

size_t
aa_k_mer_get_pos_corrected_index (const char *k_mer, size_t k, KMerPos pos)
{
	assert (k_mer != NULL);
	assert (k > 0);

	size_t index = 0;
	size_t cur_k = strlen (k_mer);

	index = aa_k_mer_get_index (k_mer, cur_k);

	switch (pos)
		{
		case K_MER_RIGHT_EDGE:
			{
				index += aa_k_mer_edge_correction (cur_k + 1, k);
				cur_k = k;
			}
		case K_MER_LEFT_EDGE:
			{
				k = cur_k;
			}
		case K_MER_INTERNAL:
			{
				index += aa_k_mer_edge_correction (1, k - 1);
			}
		default:
			{
				// Remve warning: enumeration value not
				// handled in switch [-Wswitch]
				break;
			}
		}

	return index;
}
