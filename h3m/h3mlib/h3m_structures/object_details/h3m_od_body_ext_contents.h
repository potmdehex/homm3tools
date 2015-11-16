// Created by John Åkerblom 2014-12-15

#ifndef __H3M_OD_BODY_EXT_CONTENTS_H_DEF__
#define __H3M_OD_BODY_EXT_CONTENTS_H_DEF__

// Included by h3m_od_dynamic.h, h3m_od_static.h

#include "../../msvc_comp_stdint.h"
#include "../h3m_common.h"

#pragma pack(push, 1)

// Used by event object and Pandora's Box
// NOT BINARY COMPATIBLE - CONTAINS DYNAMIC ELEMENTS
struct H3M_OD_BODY_EXT_CONTENTS {
    uint32_t experience;
    uint32_t spell_points;
    uint8_t morale;
    uint8_t luck;
    struct H3M_COMMON_RESOURCES resources;
    struct H3M_COMMON_PRIMARY_SKILLS primary_skills;
    uint8_t secondary_skills_count;
    struct H3M_COMMON_SECONDARY_SKILL *secondary_skills;
    uint8_t artifacts_count;
    union H3M_COMMON_ARTIFACT_TYPE *artifacts;
    uint8_t spells_count;
    H3M_COMMON_SPELL_TYPE *spells;
    uint8_t creatures_count;
    union H3M_COMMON_ARMY *creatures;
    uint8_t unknown1[8];
};

#pragma pack(pop)

#endif
