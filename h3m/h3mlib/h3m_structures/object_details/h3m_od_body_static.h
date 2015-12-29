// Created by John Åkerblom 2014-12-15

#ifndef __H3M_OD_BODY_STATIC_H_DEF__
#define __H3M_OD_BODY_STATIC_H_DEF__

// Included by h3m_od.h

#include "../utils/msvc_comp_stdint.h"

#pragma pack(push, 1)

// BINARY COMPATIBLE
struct H3M_OD_BODY_STATIC_PLACEHOLDER_HERO_ANY {
    uint8_t owner;
    uint8_t type;               // MUST be 0xFF for this struct
    uint8_t power_rating;
};

// BINARY COMPATIBLE
struct H3M_OD_BODY_STATIC_PLACEHOLDER_HERO_SPECIFIC {
    uint8_t owner;
    uint8_t type;               // CANNOT be 0xFF for this struct
};

// BINARY COMPATIBLE
struct H3M_OD_BODY_STATIC_SCHOLAR {
    uint8_t reward_type;
    uint8_t reward_value;
    uint8_t unknown[6];
};

// BINARY COMPATIBLE
struct H3M_OD_BODY_STATIC_SHRINE {
    uint32_t spell;
};

// BINARY COMPATIBLE
struct H3M_OD_BODY_STATIC_ARTIFACT {
    uint8_t has_guardians;      // MUST be 0x00 for this struct
};

// BINARY COMPATIBLE
struct H3M_OD_BODY_STATIC_SPELL_SCROLL {
    uint8_t has_guardians;      // MUST be 0x00 for this struct
    uint32_t spell;
};

// BINARY COMPATIBLE
struct H3M_OD_BODY_STATIC_RESOURCE {
    uint8_t has_guardians;      // MUST be 0x00 for this struct
    uint32_t quantity;
    uint8_t unknown1[4];        // default 0x00
};

// BINARY COMPATIBLE
struct H3M_OD_BODY_STATIC_FLAGGED {
    uint32_t owner;
};

// BINARY COMPATIBLE
struct H3M_OD_BODY_STATIC_GRAIL {
    uint32_t allowable_radius;
};

// BINARY COMPATIBLE
struct H3M_OD_BODY_STATIC_WITCH_HUT_ABSOD {
    uint8_t potential_skills[4];
};

// BINARY COMPATIBLE
struct H3M_OD_BODY_STATIC_MONSTER {
    uint16_t quantity;
    uint8_t disposition;        // likelyhood of joining hero
    uint8_t has_mesg_and_treasure;      // MUST be 0x00 for this struct
    uint8_t never_flees;
    uint8_t does_not_grow;
    uint8_t unknown1[2];
};

// BINARY COMPATIBLE
struct H3M_OD_BODY_STATIC_GARRISON_ROE {
    uint32_t owner;
    struct H3M_COMMON_ARMY_ROE creatures;
    // ROE - no removable_units here
    uint8_t unknown1[8];
};

// BINARY COMPATIBLE
struct H3M_OD_BODY_STATIC_GARRISON_ABSOD {
    uint32_t owner;
    struct H3M_COMMON_ARMY_ABSOD creatures;
    uint8_t removable_units;    // AB/SOD feature
    uint8_t unknown1[8];
};

union H3M_OD_BODY_STATIC_GARRISON {
    struct H3M_OD_BODY_STATIC_GARRISON_ROE roe;
    struct H3M_OD_BODY_STATIC_GARRISON_ROE any;
    struct H3M_OD_BODY_STATIC_GARRISON_ABSOD ab;
    struct H3M_OD_BODY_STATIC_GARRISON_ABSOD sod;
    struct H3M_OD_BODY_STATIC_GARRISON_ABSOD absod;
};

#define BODY_RESOURCE_GENERATOR BODY_FLAGGED
#define BODY_DWELLING BODY_FLAGGED

#pragma pack(pop)

#endif
