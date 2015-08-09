#include <stdlib.h>
#include "misc.h"

#undef NDEBUG
#include <assert.h>

void *
xmalloc(size_t sz)
{
	void *p = malloc(sz);
	assert(p || !sz);
	return p;
}

void *
xrealloc(void *ptr, size_t sz)
{
	void *p = realloc(ptr, sz);
	assert(p || !sz);
	return p;
}

void *
xcalloc(size_t n, size_t sz)
{
	void *p = calloc(n, sz);
	assert(p || !n);
	return p;
}

void
xfree(void *ptr)
{
	if (ptr)
		free(ptr);
}
