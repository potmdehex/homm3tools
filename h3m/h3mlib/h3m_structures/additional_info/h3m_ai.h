// Created by John Åkerblom 2014-11-22

#ifndef __H3M_AI_H_DEF__
#define __H3M_AI_H_DEF__

// Included by h3m.h

#include "../utils/msvc_comp_stdint.h"
#include "h3m_ai_custom_hero.h"
#include "h3m_ai_hero_settings.h"
#include "h3m_ai_lose_cond.h"
#include "h3m_ai_rumor.h"
#include "h3m_ai_teams.h"
#include "h3m_ai_win_cond.h"

#pragma pack(push, 1)

// NOT BINARY COMPATIBLE - CONTAINS DYNAMIC ELEMENTS
struct H3M_AI_ROE {
    uint8_t win_cond_type;
    union H3M_AI_WIN_COND win_cond;     // DYNAMIC MEMBER
    uint8_t lose_cond_type;
    union H3M_AI_LOSE_COND lose_cond;   // DYNAMIC MEMBER
    uint8_t teams_count;
    struct H3M_AI_TEAMS teams;  // DYNAMIC MEMBER
    uint8_t available_heroes[16];       // ROE - size 16 available_heroes
    // ROE - no empty (4 empty bytes) here
    // ROE - no custom_hero_count here
    // ROE - no custom_heroes array here
    uint8_t reserved[31];
    // ROE - no available_artifacts here
    // ROE - no available_spells here
    // ROE - no available_skills here
    uint32_t rumors_count;
    struct H3M_AI_RUMOR *rumors;        // DYNAMIC MEMBER
};
#define H3M_AI_ANY H3M_AI_ROE

// NOT BINARY COMPATIBLE - CONTAINS DYNAMIC ELEMENTS
struct H3M_AI_AB {
    uint8_t win_cond_type;
    union H3M_AI_WIN_COND win_cond;     // DYNAMIC MEMBER
    uint8_t lose_cond_type;
    union H3M_AI_LOSE_COND lose_cond;   // DYNAMIC MEMBER
    uint8_t teams_count;
    struct H3M_AI_TEAMS teams;  // DYNAMIC MEMBER
    uint8_t available_heroes[20];       // AB/SOD - size 20 available_heroes
    uint32_t empty;             // AB/SOD feature
    // AB - no custom_hero_count here
    // AB - no custom_heroes array here
    uint8_t reserved[31];
    uint8_t available_artifacts[17];    // AB - size 17 available_artifacts
    // AB - no available_spells here
    // AB - no available_skills here
    uint32_t rumors_count;
    struct H3M_AI_RUMOR *rumors;        // DYNAMIC MEMBER
};

// NOT BINARY COMPATIBLE - CONTAINS DYNAMIC ELEMENTS
struct H3M_AI_SOD {
    uint8_t win_cond_type;
    union H3M_AI_WIN_COND win_cond;     // DYNAMIC MEMBER
    uint8_t lose_cond_type;
    union H3M_AI_LOSE_COND lose_cond;   // DYNAMIC MEMBER
    uint8_t teams_count;
    struct H3M_AI_TEAMS teams;  // DYNAMIC MEMBER
    uint8_t available_heroes[20];       // AB/SOD - size 20 available_heroes
    uint32_t empty;             // AB/SOD feature
    uint8_t custom_heroes_count;        // SOD feature
    struct H3M_AI_CUSTOM_HERO_SOD *custom_heroes;       // DYNAMIC MEMBER
    uint8_t reserved[31];
    uint8_t available_artifacts[18];    // SOD - size 18 available_artifacts
    uint8_t available_spells[9];        // SOD feature
    uint8_t available_skills[4];        // SOD feature
    uint32_t rumors_count;
    struct H3M_AI_RUMOR *rumors;        // DYNAMIC MEMBER

    struct H3M_AI_HERO_SETTINGS hero_settings[156];     // DYNAMIC MEMBER, SOD feature
};

union H3M_AI {
    struct H3M_AI_ROE roe;
    struct H3M_AI_ROE any;      // Only useful for getting a few sizes
    struct H3M_AI_AB ab;
    struct H3M_AI_SOD sod;
};

#pragma pack(pop)

#endif
