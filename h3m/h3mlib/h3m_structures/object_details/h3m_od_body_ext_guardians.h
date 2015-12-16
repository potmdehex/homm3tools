// Created by John Åkerblom 2014-12-15

#ifndef __H3M_OD_BODY_EXT_GUARDIANS_H_DEF__
#define __H3M_OD_BODY_EXT_GUARDIANS_H_DEF__

// Included by h3m_od_dynamic.h, h3m_od_static.h

#include "../../utils/msvc_comp_stdint.h"

#pragma pack(push, 1)

// NOT BINARY COMPATIBLE - CONTAINS DYNAMIC ELEMENTS
struct H3M_OD_BODY_EXT_GUARDIANS {
    uint32_t mesg_size;
    uint8_t *mesg;
    uint8_t has_creatures;
    union H3M_COMMON_ARMY *creatures;
    uint8_t unknown1[4];
};

#pragma pack(pop)

#endif
