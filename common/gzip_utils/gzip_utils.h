/* Created by John Åkerblom 2014-11-22  */

#ifndef __GZIP_UTILS_H_DEF__
#define __GZIP_UTILS_H_DEF__ 

#include "crossplatform.h"

#ifdef __cplusplus
extern "C" {
#endif

int DLL_PUBLIC gu_compress(const char *filename_in, const char *filename_out);

int DLL_PUBLIC gu_decompress(const char *filename_in, const char *filename_out);
int DLL_PUBLIC gu_decompress_file_to_mem(const char *filename, void **dst, unsigned long *dst_size);
int DLL_PUBLIC gu_decompress_mem(void *src, unsigned long src_size, void **dst, unsigned long *dst_size, unsigned long *remaining_size);

void DLL_PUBLIC gu_free(void *buf);

#ifdef _WIN32
    #include <stddef.h>
    int DLL_PUBLIC gu_decompress_file_to_mem_u(const wchar_t *filename, void **dst, unsigned long *dst_size);
#endif

#ifdef __cplusplus
}
#endif

#endif
