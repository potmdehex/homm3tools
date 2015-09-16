/* Created by John Åkerblom 2014-12-31  */

#ifdef NO_ZLIB

#ifndef __GZIP_UTILS_H_DEF__
#define __GZIP_UTILS_H_DEF__

int gu_compress(const char *filename_in, const char *filename_out);
int gu_decompress(const char *filename_in, const char *filename_out);

int gu_decompress_file_to_mem(const char *filename, void **dst, long *dst_size);
int gu_decompress_mem(const void *src, long src_size, void **dst, long *dst_size);

void gu_free(void *buf);

#endif

#endif
