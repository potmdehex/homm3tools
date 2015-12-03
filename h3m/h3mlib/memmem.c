#include <stdlib.h>
#include <string.h>

/**
 * The memmem() function finds the start of the first occurrence of the
 * substring 'needle' of length 'nlen' in the memory area 'haystack' of
 * length 'hlen'.
 *
 * The return value is a pointer to the beginning of the sub-string, or
 * NULL if the substring is not found.
 *
 * Modified types, original author: caf, http://stackoverflow.com/questions/2188914/c-searching-for-a-string-in-a-file
 */
unsigned char *memmem(unsigned char *haystack, size_t hlen,
    const unsigned char *needle, size_t nlen)
{
    unsigned char needle_first;
    unsigned char *p = haystack;
    size_t plen = hlen;

    if (!nlen)
        return NULL;

    needle_first = *(unsigned char *)needle;

    while (plen >= nlen
        && (p = (unsigned char *)memchr(p, needle_first, plen - nlen + 1))) {
        if (!memcmp(p, needle, nlen))
            return p;

        p++;
        plen = hlen - (p - haystack);
    }

    return NULL;
}
