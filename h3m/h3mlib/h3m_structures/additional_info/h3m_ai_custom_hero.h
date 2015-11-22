// Created by John Åkerblom 2014-11-22

#ifndef __H3M_AI_CUSTOM_HERO_H_DEF__
#define __H3M_AI_CUSTOM_HERO_H_DEF__

// Included by h3m_ai.h

#include "../msvc_comp_stdint.h"

#pragma pack(push, 1)

// NOT BINARY COMPATIBLE - CONTAINS DYNAMIC ELEMENTS
struct H3M_AI_CUSTOM_HERO_SOD {
    uint8_t type;
    uint8_t face;
    uint32_t name_size;
    uint8_t *name;              // DYNAMIC MEMBER
    uint8_t allowed_players;
};

#pragma pack(pop)

#endif
