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

static long _gzsize_f(FILE *f)
{
	long size = -1;

	fseek(f, -4, SEEK_END);

	fread(&size, 4, 1, f);

	return size;
}

static long _gzsize(const char *input_file)
{
	FILE *f = NULL;
	long size = -1;

	if (NULL == (f = fopen(input_file, "rb"))) {
		return -1;
	}

	size = _gzsize_f(f);

	fclose(f);

	return size;
}


int gu_compress(const char *filename_in, const char *filename_out)
{
    gzFile	gzf		= NULL;
	FILE	*f		= NULL;
	char	*buf	= NULL;
	long	size	= 0;

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

int _inflate(const void *src, int srcLen, void *dst, int dstLen) {
    z_stream strm = { 0 };
    strm.total_in = strm.avail_in = srcLen;
    strm.total_out = strm.avail_out = dstLen;
    strm.next_in = (Bytef *)src;
    strm.next_out = (Bytef *)dst;

    strm.zalloc = Z_NULL;
    strm.zfree = Z_NULL;
    strm.opaque = Z_NULL;

    int err = -1;
    int ret = -1;

    err = inflateInit2(&strm, (15 + 32)); //15 window bits, and the +32 tells zlib to to detect if using gzip or zlib
    if (err == Z_OK) {
        err = inflate(&strm, Z_FINISH);
        if (err == Z_STREAM_END) {
            ret = strm.total_out;
        }
        else {
            inflateEnd(&strm);
            return err;
        }
    }
    else {
        inflateEnd(&strm);
        return err;
    }

    inflateEnd(&strm);
    return ret;
}

int gu_decompress_mem(const void *src, long src_size, void **dst, long *dst_size)
{
    /* Retrieve uncompressed size from the last 4 bytes of the file where it is
     * stored in the gzip format */
    *dst_size = *(long *)&((unsigned char *)src)[src_size - 4];

    *dst = malloc(*dst_size);
    /* If decrompession with Z_DATA ERROR src is likely not compressed,
     * so simply return src buffer */
    if (Z_DATA_ERROR == _inflate(src, src_size, *dst, *dst_size)) {
        free(*dst);
        *dst = (void *)src;
        *dst_size = src_size;
    }

    return 0;
}

int gu_decompress_file_to_mem(const char *filename, void **dst, long *dst_size)
{
    FILE *f = NULL;
    void *src = NULL;
    long src_size = 0;

    if (NULL == (f = fopen(filename, "rb"))) {
        return 1;
    }

    fseek(f, 0, SEEK_END);
    src_size = ftell(f);
    rewind(f);

    src = malloc(src_size);
    fread(src, 1, src_size, f);
    fclose(f);

    return gu_decompress_mem(src, src_size, dst, dst_size);
}

#ifdef _WIN32
int gu_decompress_file_to_mem_u(const wchar_t *filename, void **dst, long *dst_size)
{
    FILE *f = NULL;
    void *src = NULL;
    long src_size = 0;
    
    if (NULL == (f = _wfopen(filename, L"rb"))) {
        return 1;
    }
    
    fseek(f, 0, SEEK_END);
    src_size = ftell(f);
    rewind(f);

    src = malloc(src_size);
    fread(src, 1, src_size, f);
    fclose(f);

    return gu_decompress_mem(src, src_size, dst, dst_size);
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
