/* Created by John Åkerblom 2014-12-31  */

#ifdef NO_ZLIB

#include "gzip_empty.h"

#include <stdio.h>
#include <stdlib.h>

int gu_compress(const char *filename_in, const char *filename_out)
{
    return 1;
}

int gu_decompress(const char *filename_in, const char *filename_out)
{
    return 1;
}

int gu_decompress_mem(const void *src, long src_size, void **dst, long *dst_size)
{
    return 1;
}

int gu_decompress_file_to_mem(const char *filename, void **dst, long *dst_size)
{
    FILE *f = fopen(filename, "rb");

    if (NULL == f) {
        return 1;
    }

    fseek(f, 0, SEEK_END);
    *dst_size = ftell(f);
    rewind(f);

    *dst = malloc(*dst_size);
    fread(*dst, *dst_size, 1, f);

    fclose(f);

    return 0;
}

void gu_free(void *buf)
{
    free(buf);
}

#endif 

