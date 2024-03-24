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
#include "strv.h"
#include "h5.h"

/* Private */
struct _H5
{
	hid_t file_id;
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
		log_fatal ("H5Fcreate: Cannot create file '%s'", file);

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

static H5 *
h5_new (void)
{
	H5 *h5 = NULL;
	hid_t datatype_int_id, datatype_str_id;

	datatype_int_id  = h5_create_integer_datatype ();
	datatype_str_id  = h5_create_string_datatype ();

	h5 = xcalloc (1, sizeof (H5));

	*h5 = (H5) {
		.datatype_int_id  = datatype_int_id,
		.datatype_str_id  = datatype_str_id
	};

	return h5;
}

H5 *
h5_create (const char *file)
{
	assert (file != NULL);

	H5 *h5 = NULL;
	hid_t file_id;

	file_id = h5_create_file (file);
	h5_create_and_write_metadata (file_id);

	h5 = h5_new ();
	h5->file_id = file_id;

	return h5;
}

static void
h5_write_k_mer (hid_t dataset_count_id, hid_t datatype_int_id, const size_t k)
{
	hid_t attribute_id, attribute_space;
	herr_t status;

	attribute_space = H5Screate (H5S_SCALAR);

	attribute_id = H5Acreate2 (dataset_count_id, "k-mer", datatype_int_id,
			attribute_space, H5P_DEFAULT, H5P_DEFAULT);

	if (attribute_id < 0)
			log_fatal ("H5Acreate2: Error creating attribute 'k-mer'");

	status = H5Awrite (attribute_id, datatype_int_id, &k);
	if (status < 0)
		log_fatal ("H5Awrite: Error writing metadata 'k-mer'");

	status = H5Sclose (attribute_space);
	if (status < 0)
		log_fatal ("H5Aclose: Error closing attribute space");

	status = H5Aclose (attribute_id);
	if (status < 0)
		log_fatal ("H5Aclose: Error closing attribute");
}

void
h5_write_count_dataset (H5 *h5, const CountTable *table, const size_t k)
{
	assert (h5 != NULL);
	assert (table != NULL);
	assert (k > 0);

	hid_t dataset_count_id;
	herr_t status;

	dataset_count_id = h5_create_count_dataset (h5->file_id,
			h5->datatype_int_id, count_table_get_nrows (table),
			count_table_get_ncols (table));

	status = H5Dwrite (dataset_count_id, h5->datatype_int_id,
		H5S_ALL, H5S_ALL, H5P_DEFAULT, count_table_data (table));

	if (status < 0)
		log_fatal ("H5Dwrite: Error writing to dataset '/count'");

	h5_write_k_mer (dataset_count_id, h5->datatype_int_id, k);

	status = H5Dclose (dataset_count_id);
	if (status < 0)
		log_fatal ("H5Dclose: Error closing dataset '/count'");
}

void
h5_write_label_dataset (H5 *h5, const char **label)
{
	assert (h5 != NULL);
	assert (label != NULL && *label != NULL);

	hid_t dataset_label_id;
	herr_t status;

	char **p = (char **) label;

	dataset_label_id = h5_create_label_dataset (h5->file_id,
			h5->datatype_str_id, strv_length (p));

	status = H5Dwrite (dataset_label_id, h5->datatype_str_id,
			H5S_ALL, H5S_ALL, H5P_DEFAULT, label);

	if (status < 0)
		log_fatal ("H5Dwrite: Error writing to dataset '/label'");

	status = H5Dclose (dataset_label_id);
	if (status < 0)
		log_fatal ("H5Dclose: Error closing dataset '/label'");
}

static hid_t
h5_open_file_ro (const char *file)
{
	hid_t file_id;

	file_id = H5Fopen (file, H5F_ACC_RDONLY, H5P_DEFAULT);
	if (file_id < 0)
		log_fatal ("H5Fopen: Cannot open file '%s'", file);

	return file_id;
}

H5 *
h5_open (const char *file)
{
	assert (file != NULL);

	H5 *h5 = NULL;
	hid_t file_id;

	file_id = h5_open_file_ro (file);

	h5 = h5_new ();
	h5->file_id = file_id;

	return h5;
}

static void
h5_read_k_mer (hid_t dataset_count_id, hid_t datatype_int_id, size_t *k)
{
	hid_t attribute_id;
	herr_t status;

	attribute_id = H5Aopen (dataset_count_id, "k-mer", H5P_DEFAULT);
	if (attribute_id < 0)
		log_fatal ("H5Aopen: Failed to open 'k-mer' attribute");

	status = H5Aread (attribute_id, datatype_int_id, k);
	if (status < 0)
		log_fatal ("H5Aread: Failed to read 'k-mer' value");

	status = H5Aclose (attribute_id);
	if (status < 0)
		log_fatal ("H5Aclose: Failed to close 'k-mer' attribute");
}

CountTable *
h5_read_count_dataset (H5 *h5, size_t *k)
{
	assert (h5 != NULL);
	assert (k != NULL);

	CountTable *table = NULL;

	hid_t dataset_count_id, dataspace_id;
	hsize_t dims[2];
	herr_t status;

	dataset_count_id = H5Dopen (h5->file_id, "/count", H5P_DEFAULT);
	if (dataset_count_id < 0)
		log_fatal ("H5Dopen: Failed to open /count dataset");

	dataspace_id = H5Dget_space (dataset_count_id);
	if (dataspace_id < 0)
		log_fatal ("H5Dget_space: Failed to get dataspace");

	status = H5Sget_simple_extent_dims (dataspace_id, dims, NULL);
	if (status < 0)
		log_fatal ("H5Sget_simple_extent_dims: Failed to get dims");

	table = count_table_new (dims[0], dims[1]);

	status = H5Dread (dataset_count_id, h5->datatype_int_id,
			H5S_ALL, H5S_ALL, H5P_DEFAULT, count_table_data (table));

	if (status < 0)
		log_fatal ("H5Dread: Failed to read /count dataset");

	h5_read_k_mer (dataset_count_id, h5->datatype_int_id, k);

	status = H5Sclose (dataspace_id);
	if (status < 0)
		log_fatal ("H5Sclose: Failed to close dataspace");

	status = H5Dclose (dataset_count_id);
	if (status < 0)
		log_fatal ("H5Aclose: Failed to close /count dataset");

	return table;
}

char **
h5_read_label_dataset (H5 *h5)
{
	assert (h5 != NULL);

	char **label = NULL;

	hid_t dataset_label_id, dataspace_id;
	hsize_t num_label;
	herr_t status;

	dataset_label_id = H5Dopen (h5->file_id, "/label", H5P_DEFAULT);
	if (dataset_label_id < 0)
		log_fatal ("H5Dopen: Failed to open /label dataset");

	dataspace_id = H5Dget_space (dataset_label_id);
	if (dataspace_id < 0)
		log_fatal ("H5Dget_space: Failed to get dataspace");

	num_label = H5Sget_simple_extent_npoints (dataspace_id);
	if (num_label < 0)
		log_fatal ("H5Sget_simple_extent_npoints: Failed to get num_label");

	label = xcalloc (num_label + 1, sizeof (char *));

	status = H5Dread (dataset_label_id, h5->datatype_str_id,
			H5S_ALL, H5S_ALL, H5P_DEFAULT, label);

	if (status < 0)
		log_fatal ("H5Dread: Failed to read /label dataset");

	status = H5Sclose (dataspace_id);
	if (status < 0)
		log_fatal ("H5Sclose: Failed to close dataspace");

	status = H5Dclose (dataset_label_id);
	if (status < 0)
		log_fatal ("H5Aclose: Failed to close /label dataset");

	return label;
}
