// Created by John Åkerblom 2014-11-22

#ifndef __H3M_AI_TEAMS_H_DEF__
#define __H3M_AI_TEAMS_H_DEF__

// Included by h3m_ai.h

#include "../msvc_comp_stdint.h"
#include "../h3mlib.h"

#pragma pack(push, 1)

// BINARY COMPATIBLE
struct H3M_AI_TEAMS {
    uint8_t team[H3M_MAX_PLAYERS];
};
#define red_team    team[0]
#define blue_team   team[1]
#define tan_team    team[2]
#define green_team  team[3]
#define orange_team team[4]
#define purple_team team[5]
#define teal_team   team[6]
#define pink_team   team[7]

#pragma pack(pop)

#endif
