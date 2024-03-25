#include <math.h>
#include "weight.h"

double
tf (double freq, double total_freq)
{
	return freq / total_freq;
}

double
idf (double N, double n)
{
	return log (N / (n + 1)) + 1;
}
