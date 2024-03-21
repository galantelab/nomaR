#include <stdio.h>
#include "aa.h"

const AA aa_data[AA_LEN] = {
	{"Alanine",       "Ala", 'A'},
	{"Arginine",      "Arg", 'R'},
	{"Asparagine",    "Asn", 'N'},
	{"Aspartate",     "Asp", 'D'},
	{"Cysteine",      "Cys", 'C'},
	{"Glutamate",     "Glu", 'E'},
	{"Glutamine",     "Gln", 'Q'},
	{"Glycine",       "Gly", 'G'},
	{"Histidine",     "His", 'H'},
	{"Isoleucine",    "Ile", 'I'},
	{"Leucine",       "Leu", 'L'},
	{"Lysine",        "Lys", 'K'},
	{"Methionine",    "Met", 'M'},
	{"Phenylalanine", "Phe", 'F'},
	{"Proline",       "Pro", 'P'},
	{"Serine",        "Ser", 'S'},
	{"Threonine",     "Thr", 'T'},
	{"Tryptophan",    "Trp", 'W'},
	{"Tyrosine",      "Tyr", 'Y'},
	{"Valine",        "Val", 'V'}
};

int
aa_get_sym1_index (char aa)
{
	int index = -1;

	switch (aa)
		{
		case 'A': {index = 0;  break;}
		case 'R': {index = 1;  break;}
		case 'N': {index = 2;  break;}
		case 'D': {index = 3;  break;}
		case 'C': {index = 4;  break;}
		case 'E': {index = 5;  break;}
		case 'Q': {index = 6;  break;}
		case 'G': {index = 7;  break;}
		case 'H': {index = 8;  break;}
		case 'I': {index = 9;  break;}
		case 'L': {index = 10; break;}
		case 'K': {index = 11; break;}
		case 'M': {index = 12; break;}
		case 'F': {index = 13; break;}
		case 'P': {index = 14; break;}
		case 'S': {index = 15; break;}
		case 'T': {index = 16; break;}
		case 'W': {index = 17; break;}
		case 'Y': {index = 18; break;}
		case 'V': {index = 19; break;}
		}

	return index;
}

int
aa_check (const char *seq)
{
	if (seq == NULL)
		return 0;

	const char *p = NULL;

	for (p = seq; *p != '\0'; p++)
		if (aa_get_sym1_index (*p) == -1)
			return 0;

	return 1;
}
