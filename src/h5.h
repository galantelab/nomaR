#ifndef H5_H
#define H5_H

#include <hdf5.h>
#include <stdint.h>

typedef struct _H5 H5;

H5   * h5_create (const char *file, uint32_t x_len, uint32_t y_len);
void   H5_close  (H5 *h5);

#endif /* h5.h */
