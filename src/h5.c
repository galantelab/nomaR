#include <hdf5.h>
#include <assert.h>

#include "log.h"
#include "wrapper.h"
#include "h5.h"

/* Private */
struct _H5
{
	hid_t file_id;
	hid_t dataset_count_id;
	hid_t dataset_label_id;
	hid_t datatype_int_id;
	hid_t datatype_str_id;
};

static inline hid_t
h5_create_integer_datatype (void)
{
	hid_t datatype_id;

	datatype_id = H5Tcopy (H5T_STD_U32LE);
	if (datatype_id < 0)
		log_fatal ("H5Tcopy: Error creating datatype 'int'");

	return datatype_id;
}

static inline hid_t
h5_create_string_datatype (void)
{
	hid_t datatype_id;

	datatype_id = H5Tcopy (H5T_C_S1);
	if (datatype_id < 0)
		log_fatal ("H5Tcopy: Error creating datatype 'str'");

	// String with variable size
	H5Tset_size (datatype_id, H5T_VARIABLE);

	return datatype_id;
}

static inline hid_t
h5_create_file (const char *file)
{
	hid_t file_id;

	file_id = H5Fcreate (file, H5F_ACC_TRUNC, H5P_DEFAULT, H5P_DEFAULT);
	if (file_id < 0)
		log_fatal ("h5_create: Cannot create file '%s'", file);

	return file_id;
}

static inline hid_t
h5_create_count_dataset (hid_t file_id, hid_t datatype_id,
		uint32_t x_len, uint32_t y_len)
{
	hid_t dataset_id, dataspace_id;
	hsize_t dims[2];
	herr_t status;

	dims[0] = x_len;
	dims[1] = y_len;

	dataspace_id = H5Screate_simple (2, dims, NULL);
	if (dataspace_id < 0)
		log_fatal ("H5Screate_simple: Error creating dataspace");

	dataset_id = H5Dcreate2 (file_id, "/count", datatype_id, dataspace_id,
			H5P_DEFAULT, H5P_DEFAULT, H5P_DEFAULT);

	if (dataset_id < 0)
		log_fatal ("H5Dcreate2: Error creating dataset '/count'");

	status = H5Sclose (dataspace_id);
	if (status < 0)
		log_fatal ("H5Sclose: Error closing dataspace");

	return dataset_id;
}

static inline hid_t
h5_create_label_dataset (hid_t file_id, hid_t datatype_id, uint32_t x_len)
{
	hid_t dataset_id, dataspace_id;
	hsize_t dims[1];
	herr_t status;

	dims[0] = x_len;

	dataspace_id = H5Screate_simple (1, dims, NULL);
	if (dataspace_id < 0)
		log_fatal ("H5Screate_simple: Error creating dataspace");

	dataset_id = H5Dcreate2 (file_id, "/label", datatype_id, dataspace_id,
			H5P_DEFAULT, H5P_DEFAULT, H5P_DEFAULT);

	if (dataset_id < 0)
		log_fatal ("H5Dcreate2: Error creating dataset '/label'");

	status = H5Sclose (dataspace_id);
	if (status < 0)
		log_fatal ("H5Sclose: Error closing dataspace");

	return dataset_id;
}

void
H5_close (H5 *h5)
{
	if (h5 == NULL)
		return;

	herr_t status;

	status = H5Dclose (h5->dataset_count_id);
	if (status < 0)
		log_fatal ("H5Dclose: Error closing dataset '/count'");

	status = H5Dclose (h5->dataset_label_id);
	if (status < 0)
		log_fatal ("H5Dclose: Error closing dataset '/label'");

	status = H5Tclose (h5->datatype_int_id);
	if (status < 0)
		log_fatal ("H5Tclose: Error closing datatype 'int'");

	status = H5Tclose (h5->datatype_str_id);
	if (status < 0)
		log_fatal ("H5Tclose: Error closing datatype 'str'");

	status = H5Fclose (h5->file_id);
	if (status < 0)
		log_fatal ("H5Fclose: Error closing file");

	xfree (h5);
}

H5 *
h5_create (const char *file, uint32_t x_len, uint32_t y_len)
{
	assert (file != NULL);
	assert (x_len > 0 && y_len > 0);

	H5 *h5 = NULL;
	hid_t file_id;
	hid_t dataset_count_id, dataset_label_id;
	hid_t datatype_int_id, datatype_str_id;

	file_id          = h5_create_file (file);
	datatype_int_id  = h5_create_integer_datatype ();
	datatype_str_id  = h5_create_string_datatype ();
	dataset_count_id = h5_create_count_dataset (file_id, datatype_int_id, x_len, y_len);
	dataset_label_id = h5_create_label_dataset (file_id, datatype_str_id, x_len);

	h5 = xcalloc (1, sizeof (h5));

	*h5 = (H5) {
		.file_id          = file_id,
		.dataset_count_id = dataset_count_id,
		.dataset_label_id = dataset_label_id,
		.datatype_int_id  = datatype_int_id,
		.datatype_str_id  = datatype_str_id
	};

	return h5;
}
