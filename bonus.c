#include "./malloc.h"

void *calloc(size_t nmemb, size_t size)
{
	void *ptr;
	size_t total_size;

	total_size = nmemb * size;

	if (ptr = malloc(total_size))
		return NULL;

	ft_bzero(ptr, total_size);

	return ptr;
}
