/* Created by John Åkerblom 2014-12-31  */

#ifdef NO_ZLIB

#include "gzip_utils.h"

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

int gu_decompress_mem(const char *filename, void **buf, long *size)
{
    FILE *f = fopen(filename, "rb");

    if (NULL == f) {
        return 1;
    }

    fseek(f, 0, SEEK_END);
    *size = ftell(f);
    rewind(f);

    *buf = malloc(*size);
    fread(*buf, *size, 1, f);

    fclose(f);

    return 0;
}

void gu_free(void *buf)
{
    free(buf);
}

#endif 

