// Created by John Åkerblom 2014-11-22

#ifndef __H3M_AI_TEAMS_H_DEF__
#define __H3M_AI_TEAMS_H_DEF__

// Included by h3m_ai.h

#include "../utils/msvc_comp_stdint.h"
#include "../h3mlib.h"

#pragma pack(push, 1)

// BINARY COMPATIBLE
struct H3M_AI_TEAMS {
    uint8_t team[H3M_MAX_PLAYERS];
};
#define TEAM_RED    team[0]
#define TEAM_BLUE   team[1]
#define TEAM_TAN    team[2]
#define TEAM_GREEN  team[3]
#define TEAM_ORANGE team[4]
#define TEAM_PURPLE team[5]
#define TEAM_TEAL   team[6]
#define TEAM_PINK   team[7]

#pragma pack(pop)

#endif
