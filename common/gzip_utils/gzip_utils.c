/* Created by John Åkerblom 2014-11-22  */
#include "gzip_utils.h"

#ifdef _WIN32
    #define ZLIB_WINAPI
#endif
#include <zlib.h>

#include <stdlib.h>
#include <stdio.h>

#ifdef _MSC_VER
    #pragma warning(disable:4996)
#endif

static unsigned long _gzsize_f(FILE *f)
{
	unsigned long size = -1;

	fseek(f, -4, SEEK_END);

	fread(&size, 4, 1, f);

	return size;
}

static long _gzsize(const char *input_file)
{
	FILE *f = NULL;
	unsigned long size = -1;

	if (NULL == (f = fopen(input_file, "rb"))) {
		return -1;
	}

	size = _gzsize_f(f);

	fclose(f);

	return size;
}


int gu_compress(const char *filename_in, const char *filename_out)
{
    gzFile gzf = NULL;
	FILE *f = NULL;
	char *buf = NULL;
	unsigned long size = 0;

    if (NULL == (f = fopen(filename_in, "rb"))) {
		return 1;
	}

	fseek(f, 0, SEEK_END);
	size = ftell(f);
	rewind(f);

	buf = malloc(size);
	fread(buf, 1, size, f);
	
	fclose(f);

    if (NULL == (gzf = gzopen(filename_out, "wb"))) {
		return 1;
	}

	gzwrite(gzf, buf, size);

	gzclose_w(gzf);
	free(buf);

	return 0;
}

#define CHUNK 0x4000
static int _inflate(unsigned char *src, unsigned long src_size, 
    unsigned char **dst, unsigned long *dst_size, unsigned long *remaining_size) 
{
    FILE *file = NULL;
    z_stream strm = { 0 };
    unsigned char *dst_new = NULL;
    unsigned long dst_new_size = 0;
    int ret = 0;

    strm.zalloc = Z_NULL;
    strm.zfree = Z_NULL;
    strm.opaque = Z_NULL;
    strm.avail_out = CHUNK;
    strm.avail_in = CHUNK;

    /* 15 window bits, and the +32 tells zlib to to detect
    * if using gzip or zlib */
    inflateInit2(&strm, (15 + 32));

    /* Loop as long as there is more data to uncompress */
    do {
        /* Make sure we don't read past the src buffer size */
        if (strm.total_in + strm.avail_in > src_size) {
            strm.avail_in = src_size - strm.total_in;
        }

        /* Make sure we don't output past the dst buffer size by re-allocing */
        if (strm.total_out + strm.avail_out > *dst_size) {
            dst_new_size = *dst_size + CHUNK;
            dst_new = calloc(1, dst_new_size);
            memcpy(dst_new, dst, *dst_size);

            free(*dst);
            *dst = dst_new;
            *dst_size = dst_new_size;
        }

        strm.next_in = src + strm.total_in;
        strm.next_out = *dst + strm.total_out;
        
        ret = inflate(&strm, Z_NO_FLUSH);
        if (ret < 0) {
            inflateEnd(&strm);
            return ret;
        }
    } while (strm.avail_out == 0);

    *dst_size = strm.total_out;

    if (remaining_size != NULL) {
        *remaining_size = src_size - strm.total_in;
    }

    return 0;
}

int gu_decompress_mem(void *src, unsigned long src_size, void **dst, unsigned long *dst_size, unsigned long *remaining_size)
{
    /* Set initial output buffer size, will be re-alloced in _inflate if needed */
    *dst_size = 0x4000;
    *dst = malloc(*dst_size);

    /* If decompression with Z_DATA ERROR src is likely not compressed,
     * so simply return src buffer */
    if (Z_DATA_ERROR == _inflate(src, src_size, (unsigned char **)dst, dst_size, remaining_size)) {
        free(*dst);
        *dst = (void *)src;
        *dst_size = src_size;
    }

    return 0;
}

int gu_decompress_file_to_mem(const char *filename, void **dst, unsigned long *dst_size)
{
    FILE *f = NULL;
    void *src = NULL;
    unsigned long src_size = 0;

    if (NULL == (f = fopen(filename, "rb"))) {
        return 1;
    }

    fseek(f, 0, SEEK_END);
    src_size = ftell(f);
    rewind(f);

    src = malloc(src_size);
    fread(src, 1, src_size, f);
    fclose(f);

    return gu_decompress_mem(src, src_size, dst, dst_size, NULL);
}

#ifdef _WIN32
int gu_decompress_file_to_mem_u(const wchar_t *filename, void **dst, unsigned long *dst_size)
{
    FILE *f = NULL;
    void *src = NULL;
    unsigned long src_size = 0;
    
    if (NULL == (f = _wfopen(filename, L"rb"))) {
        return 1;
    }
    
    fseek(f, 0, SEEK_END);
    src_size = ftell(f);
    rewind(f);

    src = malloc(src_size);
    fread(src, 1, src_size, f);
    fclose(f);

    return gu_decompress_mem(src, src_size, dst, dst_size, NULL);
}
#endif

int gu_decompress(const char *filename_in, const char *filename_out)
{
    FILE *f = NULL;
    void *dst = NULL;
    long size = 0;
    int ret = 0;

    if (0 != (ret = gu_decompress_file_to_mem(filename_in, &dst, &size))) {
        return ret;
    }

    if (NULL == (f = fopen(filename_out, "wb"))) {
        free(dst);
		return 1;
	}
	
	fwrite(dst, size, 1, f);

	free(dst);
	fclose(f);

	return 0;
}

void gu_free(void *buf)
{
    free(buf);
}
