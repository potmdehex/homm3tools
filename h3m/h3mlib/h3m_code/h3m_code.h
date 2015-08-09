// Created by John Åkerblom 2015-08-03

#ifndef __H3M_CODE_H_DEF__
#define __H3M_CODE_H_DEF__

#include "../h3mlib_ctx.h"
#include <stdio.h>

// Context used for injected code
struct H3M_CODE {
    enum H3M_CODE_TARGET target;
    uint8_t *dll;
    size_t dll_size;
    uint8_t *shellcode_oa;
    long shellcode_oa_offset;
    int extra_oa;
};

int h3m_code_write_oa_eof_jmp(struct H3M_CODE *hc, uint32_t oa_count, 
    uint32_t od_count, FILE * f);
int h3m_code_write_eof_dll(const struct H3M_CODE *hc, uint32_t fm, FILE * f);

// Currently exported in h3mlib.h
// int h3m_code_set_dll(h3mlib_ctx_t ctx, const char *dll);
// int h3m_code_set_target(h3mlib_ctx_t ctx, enum H3M_CODE_TARGET target);
// int h3m_code_unset(h3mlib_ctx_t ctx);

#endif
