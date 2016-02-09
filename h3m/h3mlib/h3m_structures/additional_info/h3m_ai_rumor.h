// Created by John Åkerblom 2014-11-22

#ifndef __H3M_AI_RUMOR_H_DEF__
#define __H3M_AI_RUMOR_H_DEF__

// Included by h3m_ai.h

#include "../../utils/msvc_comp_stdint.h"
#include "../../h3mlib.h"

#pragma pack(push, 1)

// NOT BINARY COMPATIBLE - CONTAINS DYNAMIC ELEMENTS
struct H3M_AI_RUMOR {
    uint32_t name_size;
    uint8_t *name;              // DYNAMIC MEMBER
    uint32_t desc_size;
    uint8_t *desc;              // DYNAMIC MEMBER
};

#pragma pack(pop)

#endif
