#ifndef AA_H
#define AA_H

#define AA_LEN 20

struct _AA {
	char name[16];
	char sym3[3];
	char sym1;
};

typedef struct _AA AA;

extern const AA aa_data[AA_LEN];

int aa_get_sym1_index (char aa);
int aa_check          (const char *seq);

#endif /* aa.h */
