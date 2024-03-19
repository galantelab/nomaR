#include <stdlib.h>
#include <string.h>
#include <assert.h>

#include "aa_k_mer.h"
#include "k_mer.h"
#include "gz.h"
#include "utils.h"
#include "wrapper.h"
#include "count_k_mer.h"

struct _CountKMer
{
	Count       *c;
	GzFile      *gz;
	const char  *file;
	const char **label;
	int          num_label;
	int          k;
};

typedef struct _CountKMer CountKMer;

static Count *
count_new (int k, int num_label)
{
	Count *c = NULL;
	uint32_t *data = NULL;
	uint32_t y_len = 0;

	y_len = aa_k_mer_get_pos_corrected_total (k);

	c = xcalloc (1, sizeof (Count));
	data = xcalloc (y_len * num_label, sizeof (uint32_t));

	*c = (Count) {
		.x_len = num_label,
		.y_len = y_len,
		.data  = data
	};

	return c;
}

void
count_free (Count *c)
{
	if (c == NULL)
		return;

	xfree (c->data);
	xfree (c);
}

static inline void
count_process_k_mer (CountKMer *ck, int x, const char *seq)
{
	KMerIter iter = {};
	KMerPos pos = 0;
	uint32_t y = 0;
	char k_mer[ck->k + 1];

	k_mer_iter_init (&iter, seq, ck->k);

	while ((pos = k_mer_iter_next (&iter, k_mer)))
	{
		y = aa_k_mer_get_pos_corrected_index (k_mer, ck->k, pos);
		COUNT_GET (ck->c, x, y) ++;
	}
}

static inline int
count_get_label_index (CountKMer *ck, const char *class, int *x)
{
	int i = 0;

	for (; i < ck->num_label; i++)
		{
			if (strcmp (ck->label[i], class) == 0)
				{
					*x = i;
					return 1;
				}
		}

	return 0;
}

static inline void
count_process_data (CountKMer *ck)
{
	int x = 0;
	size_t num_line = 0;
	char *line = NULL;
	const char *class = NULL;
	const char *seq = NULL;
	char *saveptr = NULL;

	while (gz_getline (ck->gz, &line, &num_line))
		{
			line = chomp (line);

			class = strtok_r (line, "\t ", &saveptr);
			seq = strtok_r (NULL, "\t ", &saveptr);

			if (class != NULL && seq != NULL
					&& strlen (seq) >= ck->k
					&& count_get_label_index (ck, class, &x))
				count_process_k_mer (ck, x, seq);
		}

	xfree (line);
}

Count *
count_k_mer (const char *file, const char **label, int num_label, int k)
{
	assert (file != NULL);
	assert (label != NULL && *label != NULL);
	assert (num_label > 0);
	assert (k > 0);

	CountKMer ck = {};
	Count *c = NULL;
	GzFile *gz = NULL;

	c = count_new (k, num_label);
	gz = gz_open_for_reading (file);

	ck = (CountKMer) {
		.c         = c,
		.gz        = gz,
		.file      = file,
		.label     = label,
		.num_label = num_label,
		.k         = k
	};

	count_process_data (&ck);

	gz_close (gz);
	return c;
}
