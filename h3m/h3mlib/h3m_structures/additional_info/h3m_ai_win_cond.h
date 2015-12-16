// Created by John Åkerblom 2014-11-22

#ifndef __H3M_AI_WIN_COND_H_DEF__
#define __H3M_AI_WIN_COND_H_DEF__

// Included by h3m_ai.h

#include "../utils/msvc_comp_stdint.h"

#pragma pack(push, 1)

// BINARY COMPATIBLE
struct H3M_AI_WIN_COND_ACQUIRE_ARTIFACT_ROE {
    uint8_t allow_normal_win;   //Russians: "Doesn't work" 
    uint8_t applies_to_computer;
    uint8_t type;               // ROE - uint8_t, NOT uint16_t
};

// BINARY COMPATIBLE
struct H3M_AI_WIN_COND_ACQUIRE_ARTIFACT_ABSOD {
    uint8_t allow_normal_win;   //Russians: "Doesn't work" 
    uint8_t applies_to_computer;
    uint16_t type;              // AB/SOD - uint16_t, NOT uint8_t
};
#define H3M_AI_WIN_COND_ACQUIRE_ARTIFACT_AB H3M_AI_WIN_COND_ACQUIRE_ARTIFACT_ABSOD
#define H3M_AI_WIN_COND_ACQUIRE_ARTIFACT_SOD H3M_AI_WIN_COND_ACQUIRE_ARTIFACT_ABSOD

// BINARY COMPATIBLE
struct H3M_AI_WIN_COND_ACCUMULATE_CREATURES_ROE {
    uint8_t allow_normal_win;
    uint8_t applies_to_computer;
    uint8_t type;               // ROE - uint8_t, NOT uint16_t
    uint32_t amount;
};

// BINARY COMPATIBLE
struct H3M_AI_WIN_COND_ACCUMULATE_CREATURES_ABSOD {
    uint8_t allow_normal_win;
    uint8_t applies_to_computer;
    uint16_t type;              // AB/SOD - uint16_t, NOT uint8_t
    uint32_t amount;
};
#define H3M_AI_WIN_COND_ACCUMULATE_CREATURES_AB H3M_AI_WIN_COND_ACCUMULATE_CREATURES_ABSOD
#define H3M_AI_WIN_COND_ACCUMULATE_CREATURES_SOD H3M_AI_WIN_COND_ACCUMULATE_CREATURES_ABSOD

// BINARY COMPATIBLE
struct H3M_AI_WIN_COND_ACCUMULATE_RESOURCES {
    uint8_t allow_normal_win;
    uint8_t applies_to_computer;
    uint8_t type;
    uint32_t amount;
};
/*****************************
 * type:                     *
 * 0 - Wood     4 - Crystal  *
 * 1 - Mercury  5 - Gems     *
 * 2 - Ore      6 - Gold     *
 * 3 - Sulfur                *
 *****************************/

// BINARY COMPATIBLE
struct H3M_AI_WIN_COND_POSITION {
    uint8_t allow_normal_win;
    uint8_t applies_to_computer;
    uint8_t xpos;
    uint8_t ypos;
    uint8_t zpos;
};
#define H3M_AI_WIN_COND_BUILD_GRAIL H3M_AI_WIN_COND_POSITION
#define H3M_AI_WIN_COND_DEFEAT_HERO H3M_AI_WIN_COND_POSITION
#define H3M_AI_WIN_COND_CAPTURE_TOWN H3M_AI_WIN_COND_POSITION
#define H3M_AI_WIN_COND_DEFEAT_MONSTER H3M_AI_WIN_COND_POSITION

// BINARY COMPATIBLE
struct H3M_AI_WIN_COND_TRANSPORT_ARTIFACT {
    uint8_t allow_normal_win;
    uint8_t applies_to_computer;
    uint8_t type;
    uint8_t xpos;
    uint8_t ypos;
    uint8_t zpos;
};

// BINARY COMPATIBLE
struct H3M_AI_WIN_COND_UPGRADE_TOWN {
    uint8_t allow_normal_win;
    uint8_t applies_to_computer;
    uint8_t xpos;
    uint8_t ypos;
    uint8_t zpos;
    uint8_t hall_level;         // 0-Town, 1-City,    2-Capitol
    uint8_t castle_level;       // 0-Fort, 1-Citadel, 2-Castle
};

// BINARY COMPATIBLE
struct H3M_AI_WIN_COND_FLAG {
    uint8_t allow_normal_win;
    uint8_t applies_to_computer;
};
#define H3M_AI_WIN_COND_FLAG_DWELLINGS H3M_AI_WIN_COND_FLAG
#define H3M_AI_WIN_COND_FLAG_MINES H3M_AI_WIN_COND_FLAG

#define C_ACQUIRE_ARTIFACT      0x00
#define C_ACCUMULATE_CREATURES  0x01
#define C_ACCUMULATE_RESOURCES  0x02
#define C_UPGRADE_TOWN          0x03
#define C_BUILD_GRAIL           0x04
#define C_DEFEAT_HERO           0x05
#define C_CAPTURE_TOWN          0x06
#define C_DEFEAT_MONSTER        0x07
#define C_FLAG_DWELLINGS        0x08
#define C_FLAG_MINES            0x09
#define C_TRANSPORT_ARTIFACT    0x0A

union H3M_AI_WIN_COND {
    union H3M_AI_WIN_COND_ACQUIRE_ARTIFACT {
        struct H3M_AI_WIN_COND_ACQUIRE_ARTIFACT_ROE roe;
        struct H3M_AI_WIN_COND_ACQUIRE_ARTIFACT_ABSOD ab;
        struct H3M_AI_WIN_COND_ACQUIRE_ARTIFACT_ABSOD sod;
        struct H3M_AI_WIN_COND_ACQUIRE_ARTIFACT_ABSOD absod;
    } c_acquire;
    union H3M_AI_WIN_COND_ACCUMULATE_CREATURES {
        struct H3M_AI_WIN_COND_ACCUMULATE_CREATURES_ROE roe;
        struct H3M_AI_WIN_COND_ACCUMULATE_CREATURES_ABSOD ab;
        struct H3M_AI_WIN_COND_ACCUMULATE_CREATURES_ABSOD sod;
        struct H3M_AI_WIN_COND_ACCUMULATE_CREATURES_ABSOD absod;
    } c_accumulate_creatures;
    struct H3M_AI_WIN_COND_ACCUMULATE_RESOURCES c_accumulate_resources;
    struct H3M_AI_WIN_COND_POSITION c_position;
    struct H3M_AI_WIN_COND_UPGRADE_TOWN c_upgrade;
    struct H3M_AI_WIN_COND_FLAG c_flag;
    struct H3M_AI_WIN_COND_TRANSPORT_ARTIFACT c_transport;
};

#pragma pack(pop)

#endif
