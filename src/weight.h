#ifndef WEIGHT_H
#define WEIGHT_H

#include <stdlib.h>

double tf       (size_t freq, size_t total_freq);
double idf      (size_t N, size_t nt);
size_t idf_nt   (size_t *vet, size_t n);
double l2_space (size_t *vet, size_t n);

#endif /* weight.h */
