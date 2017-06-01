// Created by John Åkerblom 2015-08-03

#ifndef __H3M_MODEMBED_H_DEF__
#define __H3M_MODEMBED_H_DEF__

#include "../internal/h3mlib_ctx.h"
#include <stdio.h>

// Context used for injected code
struct H3M_MODEMBED {
    enum H3M_MODEMBED_TARGET target;
    uint8_t *dll;
    size_t dll_size;
    uint8_t *shellcode_oa;
    long shellcode_oa_offset;
    int extra_oa;
};

int h3m_modembed_write_oa_eof_jmp(struct H3M_MODEMBED *hm, uint32_t oa_count, 
    uint32_t od_count, FILE * f);
int h3m_modembed_write_eof_dll(const struct H3M_MODEMBED *hm, uint32_t fm, FILE * f);

// Currently exported in h3mlib.h
// int h3m_modembed_set_dll(h3mlib_ctx_t ctx, const char *dll);
// int h3m_modembed_set_target(h3mlib_ctx_t ctx, enum H3M_MODEMBED_TARGET target);
// int h3m_modembed_unset(h3mlib_ctx_t ctx);

#pragma pack(push, 1)

struct shellcode_oa_jmp_to_dll_load_t {
    uint8_t c0_0[8];
    uint16_t maps_stack_offset;
    uint8_t c0_1[16];
    uint32_t SetCurrentDirectoryA;
    uint8_t c1_0[23];
    uint32_t CreateFileA;
    uint8_t c1_1[10];
    uint32_t total_file_size;
    uint8_t c1_2[5];
    uint32_t VirtualAlloc;
    uint8_t c1_3[2];
    uint32_t shellcode_eof_offset;
    uint8_t c1_4[23];
    uint32_t ReadFile;
    uint8_t c2[8]; // includes c3, j2 up to ss
    uint32_t call_esp_gadget;
    uint32_t anticrash_gadget1;
    uint32_t anticrash_gadget2;
    uint8_t j1[2];
};

struct shellcode_eof_load_dll_t {
    uint8_t c1_1[3];
    uint32_t CloseHandle1;
    uint8_t c1_3[24];
    uint32_t CreateFileA;
    uint8_t c1_4[6];
    uint32_t dll_size;
    uint8_t c1_5[2];
    uint32_t dll_offset;
    uint8_t c1_6[4];
    uint32_t WriteFile;
    uint8_t c1_7[3];
    uint32_t CloseHandle2;
    uint8_t c1_8[3];
    uint32_t LoadLibraryA;
    uint8_t c2_1[6];
    uint32_t GetModuleHandleA1;
    uint8_t c2_2[7];
    uint32_t GetProcAddress1;
    uint8_t c2_3[8];
    uint32_t GetModuleHandleA2;
    uint8_t c2_4[7];
    uint32_t GetProcAddress2;
    uint8_t c2_5[22];
    uint32_t SetCurrentDirectoryA;
    uint8_t c2_6[24];
    uint32_t orig_retn;
    uint8_t c2_7[4];
    uint32_t map_format;
    uint8_t c2_8[67]; // contains s: (strings) as well
    uint8_t dll[];
};

#pragma pack(pop)

#endif
