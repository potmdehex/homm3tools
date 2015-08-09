/* Created by John Åkerblom 2014-12-31  */

#ifdef NO_ZLIB

#ifndef __GZIP_UTILS_H_DEF__
#define __GZIP_UTILS_H_DEF__

int gu_compress(const char *filename_in, const char *filename_out);
int gu_decompress(const char *filename_in, const char *filename_out);

int gu_decompress_mem(const char *filename, void **buf, long *size);
void gu_free(void *buf);

#endif

#endif
