// Created by John Åkerblom 2014-11-18

#ifndef __H3M_H_DEF__
#define __H3M_H_DEF__

#include "../msvc_comp_stdint.h"

#define H3M_MAX_PLAYERS 8

#include "h3m_ai.h"
#include "h3m_bi.h"
#include "h3m_oa.h"
#include "h3m_od.h"
#include "h3m_event.h"
#include "h3m_player.h"
#include "h3m_tile.h"

#pragma pack(push, 1)

// NOT BINARY COMPATIBLE - CONTAINS DYNAMIC ELEMENTS
struct H3M {
    uint32_t format;
    union H3M_BI bi;
    union H3M_PLAYER **players;
    struct H3M_PLAYER_AI_ABSOD player_ai[H3M_MAX_PLAYERS];
    union H3M_AI ai;
    struct H3M_TILE *tiles;
    struct H3M_OA oa;
    struct H3M_OD od;
    struct H3M_EVENT event;

    // All maps end with 124 bytes 0x00 padding. Simply commented out because
    // it's not really necessary to have in this dynamic struct
    //uint8_t end_pad[124];
};

#pragma pack(pop)

#endif
