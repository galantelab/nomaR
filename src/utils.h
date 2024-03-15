#ifndef UTILS_H
#define UTILS_H

#include <stdlib.h>

char * chomp      (char *str);
char * trim       (char *str);
char * trimc      (char *str, int c);
size_t buf_expand (void **buf, size_t size,
		size_t old_nmemb, size_t length);

#endif /* utils.h */
