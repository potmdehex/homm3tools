// Created by John Åkerblom 2014-12-15

#ifndef __H3M_OD_BODY_EXT_TOWN_EVENT_H_DEF__
#define __H3M_OD_BODY_EXT_TOWN_EVENT_H_DEF__

// Included by h3m_od_dynamic.h, h3m_od_static.h

#include "../../utils/msvc_comp_stdint.h"

#pragma pack(push, 1)

// NOT BINARY COMPATIBLE - CONTAINS DYNAMIC ELEMENTS
struct H3M_OD_BODY_EXT_TOWN_EVENT {
    uint32_t name_size;
    uint8_t *name;              // DYNAMIC MEMBER
    uint32_t mesg_size;
    uint8_t *mesg;              // DYNAMIC MEMBER
    struct H3M_COMMON_RESOURCES resources;
    uint8_t applies_to_players;
    uint8_t applies_to_human;   // SOD feature
    uint8_t applies_to_computer;
    uint16_t first_occurence;
    uint8_t subsequent_occurences;
    uint8_t unknown1[17];
    uint8_t buildings[6];
    uint16_t creature_quantities[7];    // 1 for each generator
    uint8_t unknown2[4];
};

#pragma pack(pop)

#endif
