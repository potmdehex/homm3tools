// Created by John Åkerblom 2014-11-20

#ifndef __H3M_PLAYER_AI_H_DEF__
#define __H3M_PLAYER_AI_H_DEF__

// Included by h3m_player.h

#include "../msvc_comp_stdint.h"

#pragma pack(push, 1)

// BINARY COMPATIBLE, LAST ELEMENT DYNAMIC - SIZE NOT CONSTANT
struct H3M_PLAYER_AI_HERO_ABSOD {
    uint8_t type;
    uint32_t name_size;
    uint8_t name[];             // <name_size>
};

// NOT BINARY COMPATIBLE - CONTAINS DYNAMIC ELEMENTS
struct H3M_PLAYER_AI_ABSOD      // Seen in Pandora's Box (SOD)
{
    uint8_t unknown1;           // Russians: "garbage ( not found what it affects ..."
    uint32_t heroes_count;
    struct H3M_PLAYER_AI_HERO_ABSOD **heroes;   // DYNAMIC MEMBER
};

#pragma pack(pop)

#endif
