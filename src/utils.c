#include <string.h>
#include <unistd.h>
#include <sys/stat.h>

#include "wrapper.h"
#include "utils.h"

char *
chomp (char *str)
{
	if (str == NULL)
		return NULL;

	size_t len = strlen (str);

	if (len && str[len - 1] == '\n')
		str[len - 1] = '\0';

	return str;
}

char *
trimc (char *str, int c)
{
	if (str == NULL)
		return NULL;

	size_t start, end;
	size_t len = strlen (str);

	// Empty string
	if (len == 0)
		return str;

	// Leading chars 'c'
	for (start = 0; start < len && str[start] == c; start++)
		;

	// All 'c' characters
	if (start == len)
		{
			str[0] = '\0';
			return str;
		}

	// Trailing chars 'c'
	for (end = len - 1; end >= start && str[end] == c; end--)
		;

	memmove (str, str + start, sizeof (char) * (end - start + 1));
	str[end - start + 1] = '\0';

	return str;
}

char *
trim (char *str)
{
	return trimc (str, ' ');
}

static inline size_t
nearest_pow (size_t num)
{
	size_t n = 1;

	while (n < num && n > 0)
		n <<= 1;

	return n ? n : num;
}

size_t
buf_expand (void **buf, size_t size,
		size_t old_nmemb, size_t length)
{
	size_t final_nmemb = nearest_pow (old_nmemb + length);
	*buf = xrealloc (*buf, size * final_nmemb);
	memset (*buf + old_nmemb * size, 0,
			size * (final_nmemb - old_nmemb));
	return final_nmemb;
}

uint32_t
powu (uint32_t base, uint32_t exp)
{
	uint32_t result = 1;

	while (exp)
		{
			if (exp % 2)
				result *= base;
			exp /= 2;
			base *= base;
		}

	return result;
}

int
exists (const char *file)
{
	struct stat sb;
	return stat (file, &sb) == 0 && sb.st_mode & S_IFREG;
}

int
is_terminal (int fd)
{
	struct stat sb;
	return fstat (fd, &sb) == 0 && S_ISCHR (sb.st_mode);
}
