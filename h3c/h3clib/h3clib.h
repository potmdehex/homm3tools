// Created by John Åkerblom 2015-12-03

#ifndef __H3CLIB_H_DEF__
#define __H3CLIB_H_DEF__

#ifdef __cplusplus
extern "C" {
#endif

#include <stddef.h>             // size_t

    enum H3C_FORMAT {
        H3C_FORMAT_ROE = 0x0000000E, // Observed in HD Edition
        H3C_FORMAT_AB = 0x00000005, // RoE campaigns in H3Complete use this too
        H3C_FORMAT_SOD = 0x00000006,
        H3C_FORMAT_CHR = 0x00000007 // Used in all Heroes Chronicles games
    };

    struct H3CLIB_CTX;
    typedef struct H3CLIB_CTX *h3clib_ctx_t;

    /* Basic API */

    int h3c_read(h3clib_ctx_t *ctx, const char *filename);
    int h3c_write(h3clib_ctx_t ctx, const char *filename);
    int h3c_exit(h3clib_ctx_t *ctx);

    typedef int (*h3c_error_cb_t) (const char *error, void *cb_data);

    int h3c_read_convert(h3clib_ctx_t *ctx,
        const char *filename, enum H3C_FORMAT target_format,
        enum H3C_FORMAT *source_format, h3c_error_cb_t cb_error, 
        void *cb_data);

    /* Unicode API */

#if defined _WIN32 && defined _MSC_VER
    int h3c_read_u(h3clib_ctx_t *ctx, const wchar_t *filename);
    int h3c_write_u(h3clib_ctx_t ctx, const wchar_t *filename);
    
    int h3c_read_convert_u(h3clib_ctx_t *ctx,
        const wchar_t *filename,
        enum H3C_FORMAT target_format,
        enum H3C_FORMAT *source_format, h3c_error_cb_t cb_error,
        void *cb_data);
#endif

#ifdef __cplusplus
}
#endif
#endif
