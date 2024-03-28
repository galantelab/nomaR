#include <math.h>
#include "weight.h"

double
tf (size_t freq, size_t total_freq)
{
	/*return log (total_freq / freq);*/
	/*return (freq * 1000000) / total_freq;*/
	return freq;
}

size_t
idf_nt (size_t *vet, size_t n)
{
	double nt = 0;
	size_t i = 0;

	for (i = 0; i < n; i++)
		nt += vet[i] > 0;

	return nt;
}

double
idf (size_t N, size_t nt)
{
	/*return log (N / (n + 1)) + 1;*/
	return log ((N + 1) / (nt + 1)) + 1;
}

double
l2_space (size_t *vet, size_t n)
{
	double l2= 0;
	size_t i = 0;

	for (i = 0; i < n; i++)
		l2 += vet[i] * vet[i];

	return sqrt (l2);
}
