// Created by John Åkerblom 2014-12-15

#ifndef __H3M_OA_H_DEF__
#define __H3M_OA_H_DEF__

// Included by h3m.h

#include "../../utils/msvc_comp_stdint.h"
#include "h3m_oa_body.h"

#pragma pack(push, 1)

// NOT BINARY COMPATIBLE - CONTAINS DYNAMIC ELEMENTS
struct H3M_OA_HEADER {
    uint32_t def_size;
    uint8_t *def;               // DYNAMIC MEMBER
};

// NOT BINARY COMPATIBLE - CONTAINS DYNAMIC ELEMENTS
struct H3M_OA_ENTRY {
    struct H3M_OA_HEADER header;        // DYNAMIC MEMBER
    struct H3M_OA_BODY body;
};

// NOT BINARY COMPATIBLE - CONTAINS DYNAMIC ELEMENTS
struct H3M_OA {
    uint32_t count;
    struct H3M_OA_ENTRY *entries;       // DYNAMIC MEMBER
};

#pragma pack(pop)

#endif
