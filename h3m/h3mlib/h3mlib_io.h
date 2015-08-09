// Created by John Åkerblom 2014-11-18

#ifndef __H3MLIB_IO_H_DEF__
#define __H3MLIB_IO_H_DEF__

#include "h3mlib.h"
#include "h3mlib_ctx.h"

int h3mlib_io_read_mem(struct H3MLIB_CTX **ctx,
    uint8_t *raw,
    uint32_t raw_size,
    h3m_parse_cb_t cb_parse,
    h3m_error_cb_t cb_error, h3m_custom_def_cb_t cb_def, void *cb_data);

int h3mlib_io_read(struct H3MLIB_CTX **ctx,
    const char *filename,
    h3m_parse_cb_t cb_parse,
    h3m_error_cb_t cb_error, h3m_custom_def_cb_t cb_def, void *cb_data);

int h3mlib_io_write(struct H3MLIB_CTX *ctx, const char *filename);

#if defined _WIN32 && defined _MSC_VER

int h3mlib_io_read_u(struct H3MLIB_CTX **ctx,
    const wchar_t *filename,
    h3m_parse_cb_t cb_parse,
    h3m_error_cb_t cb_error, h3m_custom_def_cb_t cb_def, void *cb_data);

int h3mlib_io_write_u(struct H3MLIB_CTX *ctx, const wchar_t *filename);

#endif

#endif
