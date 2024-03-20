#ifndef H5_H
#define H5_H

#include <hdf5.h>
#include <stdint.h>

typedef struct _H5 H5;

H5   * h5_create              (const char *file, uint32_t x_len, uint32_t y_len);
void   h5_close               (H5 *h5);
void   h5_write_count_dataset (H5 *h5, uint32_t *data);
void   h5_write_label_dataset (H5 *h5, const char **data);

#endif /* h5.h */
