#ifdef HAVE_CONFIG_H
#include "config.h"
#else
#define PACKAGE_STRING "ponga 0.0.0"
#endif

#include <hdf5.h>
#include <string.h>
#include <time.h>
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

static void
h5_metadata_creation_time (hid_t file_id, hid_t attribute_space, hid_t attribute_type)
{
	hid_t attribute_id;
	herr_t status;
	time_t creation_time;
	char buf[32];

	creation_time = time (NULL);

	struct tm *lt = localtime (&creation_time);
	buf[strftime (buf, sizeof (buf), "%Y-%m-%d %H:%M:%S", lt)] = '\0';

	status = H5Tset_size (attribute_type, strlen (buf) + 1);
	if (status < 0)
		log_fatal ("H5Tset_size: Error setting attribute size");

	attribute_id = H5Acreate2 (file_id, "creation_date", attribute_type,
			attribute_space, H5P_DEFAULT, H5P_DEFAULT);

	if (attribute_id < 0)
		log_fatal ("H5Acreate2: Error creating attribute 'creation_date'");

	status = H5Awrite (attribute_id, attribute_type, buf);
	if (status < 0)
		log_fatal ("H5Awrite: Error writing metadata 'creation_time'");

	status = H5Aclose (attribute_id);
	if (status < 0)
		log_fatal ("H5Aclose: Error closing attribute");
}

static void
h5_metadata_program_name (hid_t file_id, hid_t attribute_space, hid_t attribute_type)
{
	hid_t attribute_id;
	herr_t status;

	status = H5Tset_size (attribute_type, strlen (PACKAGE_STRING) + 1);
	if (status < 0)
		log_fatal ("H5Tset_size: Error setting attribute size");

	attribute_id = H5Acreate2 (file_id, "program_name", attribute_type,
			attribute_space, H5P_DEFAULT, H5P_DEFAULT);

	if (attribute_id < 0)
		log_fatal ("H5Acreate2: Error creating attribute 'program_name'");

	status = H5Awrite (attribute_id, attribute_type, PACKAGE_STRING);
	if (status < 0)
		log_fatal ("H5Awrite: Error writing metadata 'program_name'");

	status = H5Aclose (attribute_id);
	if (status < 0)
		log_fatal ("H5Aclose: Error closing attribute");
}

static void
h5_create_and_write_metadata (hid_t file_id)
{
	hid_t attribute_space, attribute_type;
	herr_t status;
	int metadata_num = 2;
	int i = 0;

	void (*metadata[])(hid_t, hid_t, hid_t) = {
		h5_metadata_program_name,
		h5_metadata_creation_time
	};

	attribute_space = H5Screate (H5S_SCALAR);
	if (attribute_space < 0)
		log_fatal ("H5Screate: Error creating attribute dataspace");

	attribute_type = H5Tcopy (H5T_C_S1);
	if (attribute_type < 0)
		log_fatal ("H5Tcopy: Error creating attribute type");

	for (; i < metadata_num; i++)
		(*metadata[i])(file_id, attribute_space, attribute_type);

	status = H5Sclose (attribute_space);
	if (status < 0)
		log_fatal ("H5Aclose: Error closing attribute space");

	status = H5Tclose (attribute_type);
	if (status < 0)
		log_fatal ("H5Tclose: Error closing attribute type");
}

static hid_t
h5_create_file (const char *file)
{
	hid_t file_id;

	file_id = H5Fcreate (file, H5F_ACC_TRUNC, H5P_DEFAULT, H5P_DEFAULT);
	if (file_id < 0)
		log_fatal ("h5_create: Cannot create file '%s'", file);

	return file_id;
}

static hid_t
h5_create_integer_datatype (void)
{
	hid_t datatype_id;

	datatype_id = H5Tcopy (H5T_NATIVE_HSIZE);
	if (datatype_id < 0)
		log_fatal ("H5Tcopy: Error creating datatype 'int'");

	return datatype_id;
}

static hid_t
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

static hid_t
h5_create_count_dataset (hid_t file_id, hid_t datatype_id,
		size_t x_len, size_t y_len)
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

static hid_t
h5_create_label_dataset (hid_t file_id, hid_t datatype_id, size_t x_len)
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
h5_close (H5 *h5)
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
h5_create (const char *file, size_t x_len, size_t y_len)
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

	h5 = xcalloc (1, sizeof (H5));

	*h5 = (H5) {
		.file_id          = file_id,
		.dataset_count_id = dataset_count_id,
		.dataset_label_id = dataset_label_id,
		.datatype_int_id  = datatype_int_id,
		.datatype_str_id  = datatype_str_id
	};

	h5_create_and_write_metadata (file_id);

	return h5;
}

void
h5_write_count_dataset (H5 *h5, size_t *data)
{
	herr_t status;

	status = H5Dwrite (h5->dataset_count_id, h5->datatype_int_id,
		H5S_ALL, H5S_ALL, H5P_DEFAULT, data);

	if (status < 0)
		log_fatal ("H5Dwrite: Error writing to dataset '/count'");
}

void
h5_write_label_dataset (H5 *h5, const char **data)
{
	herr_t status;

	status = H5Dwrite (h5->dataset_label_id, h5->datatype_str_id,
		H5S_ALL, H5S_ALL, H5P_DEFAULT, data);

	if (status < 0)
		log_fatal ("H5Dwrite: Error writing to dataset '/label'");
}
