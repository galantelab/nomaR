#ifndef STRV_H
#define STRV_H

#include <stdlib.h>

typedef struct _StrvBuilder StrvBuilder;

StrvBuilder *   strv_builder_new        (void);
char        **  strv_builder_end        (StrvBuilder *b);
size_t          strv_builder_length     (StrvBuilder *b);
void            strv_builder_add        (StrvBuilder *b, const char *value);
int             strv_builder_contains   (StrvBuilder *b, const char *value, size_t *i);

void            strv_free               (char **strv);
size_t          strv_length             (char **strv);
int             strv_contains           (const char *const *strv, const char *str, size_t *i);

#endif /* strv.h */
