// Created by John Åkerblom 2014-12-15

#ifndef __H3M_OD_BODY_EXT_MONSTER_TREASURE_H_DEF__
#define __H3M_OD_BODY_EXT_MONSTER_TREASURE_H_DEF__

// Included by h3m_od_dynamic.h, h3m_od_static.h

#include "../../utils/msvc_comp_stdint.h"

#pragma pack(push, 1)

// BINARY COMPATIBLE
struct H3M_OD_BODY_EXT_TREASURE_ROE {
    struct H3M_COMMON_RESOURCES resources;
    H3M_COMMON_ARTIFACT_TYPE_ROE artifact;
};

// BINARY COMPATIBLE
struct H3M_OD_BODY_EXT_TREASURE_ABSOD {
    struct H3M_COMMON_RESOURCES resources;
    H3M_COMMON_ARTIFACT_TYPE_ABSOD artifact;
};

union H3M_OD_BODY_EXT_TREASURE {
    struct H3M_OD_BODY_EXT_TREASURE_ROE roe;
    struct H3M_OD_BODY_EXT_TREASURE_ABSOD ab;
    struct H3M_OD_BODY_EXT_TREASURE_ABSOD sod;
    struct H3M_OD_BODY_EXT_TREASURE_ABSOD absod;
};

struct H3M_OD_BODY_EXT_MESG_AND_TREASURE {
    uint32_t mesg_size;
    uint8_t *mesg;
    union H3M_OD_BODY_EXT_TREASURE *treasure;
};

#pragma pack(pop)

#endif
