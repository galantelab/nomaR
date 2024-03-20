#ifndef UTILS_H
#define UTILS_H

#include <stdlib.h>
#include <stdint.h>

char * chomp (char *str);
char * trim  (char *str);
char * trimc (char *str, int c);

size_t buf_expand (void **buf, size_t size,
		size_t old_nmemb, size_t length);

uint32_t powu (uint32_t base, uint32_t exp);

int exists      (const char *file);
int is_terminal (int fd);

#endif /* utils.h */
