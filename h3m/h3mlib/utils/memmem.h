#ifndef __MEMMEM_H_DEF__
#define __MEMMEM_H_DEF__

#include <stdlib.h>

unsigned char *memmem(unsigned char *haystack, size_t hlen,
    const unsigned char *needle, size_t nlen);

#endif
