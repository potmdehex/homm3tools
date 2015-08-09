// Created by John Åkerblom 2014-11-22

#ifndef __H3M_AI_LOSE_COND_H_DEF__
#define __H3M_AI_LOSE_COND_H_DEF__

// Included by h3m_ai.h

#include "../msvc_comp_stdint.h"

#pragma pack(push, 1)

// BINARY COMPATIBLE
struct H3M_AI_LOSE_COND_POSITION {
    uint8_t xpos;
    uint8_t ypos;
    uint8_t zpos;
};
#define H3M_AI_LOSE_COND_TOWN H3M_AI_LOSE_COND_POSITION
#define H3M_AI_LOSE_COND_HERO H3M_AI_LOSE_COND_POSITION

// BINARY COMPATIBLE
struct H3M_AI_LOSE_COND_TIME {
    uint16_t days;
};

#define C_LOSE_TOWN 0x00
#define C_LOSE_HERO 0x01
#define C_TIME      0x02

union H3M_AI_LOSE_COND {
    struct H3M_AI_LOSE_COND_POSITION c_position;
    struct H3M_AI_LOSE_COND_TIME c_time;
};

#pragma pack(pop)

#endif
