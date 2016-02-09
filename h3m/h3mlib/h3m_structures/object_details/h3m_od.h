// Created by John Åkerblom 2014-12-15

#ifndef __H3M_OD_H_DEF__
#define __H3M_OD_H_DEF__

// Included by h3m.h

#include "../../utils/msvc_comp_stdint.h"
#include "h3m_od_body_dynamic.h"
#include "h3m_od_body_static.h"

#pragma pack(push, 1)

// BINARY COMPATIBLE
struct H3M_OD_HEADER {
    uint8_t x;
    uint8_t y;
    uint8_t z;
    uint32_t oa_index;
    uint8_t unknown1[5];        //0x00
};

// NOT BINARY COMPATIBLE - CONTAINS DYNAMIC ELEMENTS
struct H3M_OD_ENTRY {
    struct H3M_OD_HEADER header;
    uint32_t absod_id;          // AB/SOD feature which comes after header for some objects
    uint8_t *body;              // DYNAMIC MEMBER
};

// NOT BINARY COMPATIBLE - CONTAINS DYNAMIC ELEMENTS
struct H3M_OD {
    uint32_t count;
    struct H3M_OD_ENTRY *entries;       // DYNAMIC MEMBER
};

#pragma pack(pop)

#endif
