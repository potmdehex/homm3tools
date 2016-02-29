// Created by John �kerblom 2014-12-15

#ifndef __H3M_COMMON_H_DEF__
#define __H3M_COMMON_H_DEF__

#include "h3m_common_spells.h"

#include "../../utils/msvc_comp_stdint.h"

#pragma pack(push, 1)

typedef uint32_t H3M_COMMON_ABSOD_ID;
typedef uint32_t H3M_COMMON_EXPERIENCE;
typedef uint32_t H3M_COMMON_SPELL_POINTS;
typedef uint16_t H3M_COMMON_ARTIFACT_TYPE_ABSOD;
typedef uint16_t H3M_COMMON_CREATURE_TYPE_ABSOD;
typedef uint8_t H3M_COMMON_ARTIFACT_TYPE_ROE;
typedef uint8_t H3M_COMMON_CREATURE_TYPE_ROE;
typedef uint8_t H3M_COMMON_HERO_TYPE;
typedef uint8_t H3M_COMMON_PLAYER_FLAG;
typedef uint8_t H3M_COMMON_SPELL_TYPE;
typedef uint8_t H3M_COMMON_MORALE;
typedef uint8_t H3M_COMMON_LUCK;

union H3M_COMMON_ARTIFACT_TYPE {
    H3M_COMMON_ARTIFACT_TYPE_ROE roe;
    H3M_COMMON_ARTIFACT_TYPE_ABSOD ab;
    H3M_COMMON_ARTIFACT_TYPE_ABSOD sod;
    H3M_COMMON_ARTIFACT_TYPE_ABSOD absod;
};

// BINARY COMPATIBLE, SIZE NOT CONSTANT
struct H3M_COMMON_STRING {
    uint32_t size;
    uint8_t data[];
};

// BINARY COMPATIBLE
struct H3M_COMMON_CREATURE_SLOT_ROE {
    H3M_COMMON_CREATURE_TYPE_ROE type;  // 0xFF - no creature
    uint16_t quantity;
};

// BINARY COMPATIBLE
struct H3M_COMMON_CREATURE_SLOT_ABSOD {
    H3M_COMMON_CREATURE_TYPE_ABSOD type;        // 0xFF - no creature
    uint16_t quantity;
};

// BINARY COMPATIBLE
struct H3M_COMMON_ARMY_ROE {
    struct H3M_COMMON_CREATURE_SLOT_ROE slots[7];
};

// BINARY COMPATIBLE
struct H3M_COMMON_ARMY_ABSOD {
    struct H3M_COMMON_CREATURE_SLOT_ABSOD slots[7];
};

// Used by heroes, towns and events but NOT by Seer's Hut (they can ask for 8)
union H3M_COMMON_ARMY {
    struct H3M_COMMON_ARMY_ROE roe;
    struct H3M_COMMON_ARMY_ABSOD ab;
    struct H3M_COMMON_ARMY_ABSOD sod;
    struct H3M_COMMON_ARMY_ABSOD absod;
};

// BINARY COMPATIBLE
struct H3M_COMMON_RESOURCES {
    uint32_t resource[7];       // TODO split into respective resource
};


// BINARY COMPATIBLE
struct H3M_COMMON_RESOURCE {
    uint8_t type;
    uint32_t amount;
};

struct H3M_COMMON_PRIMARY_SKILL
{
    uint8_t type;
    uint8_t amount;
};

// BINARY COMPATIBLE
struct H3M_COMMON_PRIMARY_SKILLS {
    uint8_t attack_skill;
    uint8_t defense_skill;
    uint8_t spell_power;
    uint8_t knowledge;
};

// BINARY COMPATIBLE
struct H3M_COMMON_SECONDARY_SKILL {
    uint8_t type;
    uint8_t level;
};

// TODO remove this struct, see H3M_COMMON_ARTIFACT_TYPE
// BINARY COMPATIBLE
struct H3M_COMMON_BACKPACK_ENTRY {
    uint16_t type;
};                              // Size 2

// BINARY COMPATIBLE, SIZE NOT CONSTANT
struct H3M_COMMON_BACKPACK_ROE {
    uint16_t count;
    H3M_COMMON_ARTIFACT_TYPE_ROE artifacts[];
};

// BINARY COMPATIBLE, SIZE NOT CONSTANT
struct H3M_COMMON_BACKPACK_ABSOD {
    uint16_t count;
    H3M_COMMON_ARTIFACT_TYPE_ABSOD artifacts[];
};

union H3M_COMMON_BACKPACK {
    struct H3M_COMMON_BACKPACK_ROE roe;
    struct H3M_COMMON_BACKPACK_ROE any;
    struct H3M_COMMON_BACKPACK_ABSOD ab;
    struct H3M_COMMON_BACKPACK_ABSOD sod;
    struct H3M_COMMON_BACKPACK_ABSOD absod;
};

// BINARY COMPATIBLE
struct H3M_COMMON_WORN_ARTIFACTS_ROE {
    H3M_COMMON_ARTIFACT_TYPE_ROE headwear;
    H3M_COMMON_ARTIFACT_TYPE_ROE shoulders;
    H3M_COMMON_ARTIFACT_TYPE_ROE right_hand;
    H3M_COMMON_ARTIFACT_TYPE_ROE left_hand;
    H3M_COMMON_ARTIFACT_TYPE_ROE torso;
    H3M_COMMON_ARTIFACT_TYPE_ROE right_ring;
    H3M_COMMON_ARTIFACT_TYPE_ROE left_ring;
    H3M_COMMON_ARTIFACT_TYPE_ROE feet;
    H3M_COMMON_ARTIFACT_TYPE_ROE misc1;
    H3M_COMMON_ARTIFACT_TYPE_ROE misc2;
    H3M_COMMON_ARTIFACT_TYPE_ROE misc3;
    H3M_COMMON_ARTIFACT_TYPE_ROE misc4;
    H3M_COMMON_ARTIFACT_TYPE_ROE device1;
    H3M_COMMON_ARTIFACT_TYPE_ROE device2;
    H3M_COMMON_ARTIFACT_TYPE_ROE device3;
    H3M_COMMON_ARTIFACT_TYPE_ROE device4;
    H3M_COMMON_ARTIFACT_TYPE_ROE spellbook;
    H3M_COMMON_ARTIFACT_TYPE_ROE misc5;
    // ROE - no unknown here
};                              // Size 18

// BINARY COMPATIBLE
struct H3M_COMMON_WORN_ARTIFACTS_AB {
    H3M_COMMON_ARTIFACT_TYPE_ABSOD headwear;
    H3M_COMMON_ARTIFACT_TYPE_ABSOD shoulders;
    H3M_COMMON_ARTIFACT_TYPE_ABSOD right_hand;
    H3M_COMMON_ARTIFACT_TYPE_ABSOD left_hand;
    H3M_COMMON_ARTIFACT_TYPE_ABSOD torso;
    H3M_COMMON_ARTIFACT_TYPE_ABSOD right_ring;
    H3M_COMMON_ARTIFACT_TYPE_ABSOD left_ring;
    H3M_COMMON_ARTIFACT_TYPE_ABSOD feet;
    H3M_COMMON_ARTIFACT_TYPE_ABSOD misc1;
    H3M_COMMON_ARTIFACT_TYPE_ABSOD misc2;
    H3M_COMMON_ARTIFACT_TYPE_ABSOD misc3;
    H3M_COMMON_ARTIFACT_TYPE_ABSOD misc4;
    H3M_COMMON_ARTIFACT_TYPE_ABSOD device1;
    H3M_COMMON_ARTIFACT_TYPE_ABSOD device2;
    H3M_COMMON_ARTIFACT_TYPE_ABSOD device3;
    H3M_COMMON_ARTIFACT_TYPE_ABSOD device4;
    H3M_COMMON_ARTIFACT_TYPE_ABSOD spellbook;
    H3M_COMMON_ARTIFACT_TYPE_ABSOD misc5;
    
    // AB - no unknown here
};                              // Size 36

// BINARY COMPATIBLE
struct H3M_COMMON_WORN_ARTIFACTS_SOD {
    H3M_COMMON_ARTIFACT_TYPE_ABSOD headwear;
    H3M_COMMON_ARTIFACT_TYPE_ABSOD shoulders;
    H3M_COMMON_ARTIFACT_TYPE_ABSOD right_hand;
    H3M_COMMON_ARTIFACT_TYPE_ABSOD left_hand;
    H3M_COMMON_ARTIFACT_TYPE_ABSOD torso;
    H3M_COMMON_ARTIFACT_TYPE_ABSOD right_ring;
    H3M_COMMON_ARTIFACT_TYPE_ABSOD left_ring;
    H3M_COMMON_ARTIFACT_TYPE_ABSOD feet;
    H3M_COMMON_ARTIFACT_TYPE_ABSOD misc1;
    H3M_COMMON_ARTIFACT_TYPE_ABSOD misc2;
    H3M_COMMON_ARTIFACT_TYPE_ABSOD misc3;
    H3M_COMMON_ARTIFACT_TYPE_ABSOD misc4;
    H3M_COMMON_ARTIFACT_TYPE_ABSOD device1;
    H3M_COMMON_ARTIFACT_TYPE_ABSOD device2;
    H3M_COMMON_ARTIFACT_TYPE_ABSOD device3;
    H3M_COMMON_ARTIFACT_TYPE_ABSOD device4;
    uint16_t unknown;           // SOD - uint16_t unknown here
    H3M_COMMON_ARTIFACT_TYPE_ABSOD spellbook;
    H3M_COMMON_ARTIFACT_TYPE_ABSOD misc5;
};                              // Size 38

union H3M_COMMON_WORN_ARTIFACTS {
    struct H3M_COMMON_WORN_ARTIFACTS_ROE roe;
    struct H3M_COMMON_WORN_ARTIFACTS_AB ab;
    struct H3M_COMMON_WORN_ARTIFACTS_SOD sod;
};

// BINARY COMPATIBLE, SIZE NOT CONSTANT
struct H3M_COMMON_ARTIFACTS_ROE {
    struct H3M_COMMON_WORN_ARTIFACTS_ROE worn;
    struct H3M_COMMON_BACKPACK_ROE backpack;
};

// BINARY COMPATIBLE, SIZE NOT CONSTANT
struct H3M_COMMON_ARTIFACTS_AB {
    struct H3M_COMMON_WORN_ARTIFACTS_AB worn;
    struct H3M_COMMON_BACKPACK_ABSOD backpack;
};

// BINARY COMPATIBLE, SIZE NOT CONSTANT
struct H3M_COMMON_ARTIFACTS_SOD {
    struct H3M_COMMON_WORN_ARTIFACTS_SOD worn;
    struct H3M_COMMON_BACKPACK_ABSOD backpack;
};

union H3M_COMMON_ARTIFACTS {
    struct H3M_COMMON_ARTIFACTS_ROE roe;
    struct H3M_COMMON_ARTIFACTS_AB ab;
    struct H3M_COMMON_ARTIFACTS_SOD sod;
    struct H3M_COMMON_ARTIFACTS_AB absod;       // note that offsets differ after worn
};

#pragma pack(pop)

#endif
