// Created by John Åkerblom 2015-03-13

#ifndef __HERO_H_DEF__
#define __HERO_H_DEF__

#include <stdint.h>

#pragma pack(push, 1)

#define HDE_HERO_DIRECTION_NORTH        0
#define HDE_HERO_DIRECTION_NORTHEAST    1
#define HDE_HERO_DIRECTION_EAST         2
#define HDE_HERO_DIRECTION_SOUTHEAST    3
#define HDE_HERO_DIRECTION_SOUTH        4
#define HDE_HERO_DIRECTION_SOUTHWEST    5
#define HDE_HERO_DIRECTION_WEST         6
#define HDE_HERO_DIRECTION_NORTHWEST    7

// high dword of all uin64_ts here is always 0xFFFFFFFF
struct HDE_HERO_ARTIFACTS
{
    uint64_t worn[8];
    uint64_t misc[4];
    uint64_t war_machines[4];
    uint64_t misc5;
    uint64_t spellbook; // 0 if has spellbook, otherwise -1
    uint64_t backpack[64];
    uint8_t backpack_count;
};

struct HDE_HERO
{
    uint16_t x;
    uint16_t y;
    uint16_t z;
    uint8_t unknown1[43];
    uint32_t dest_x;
    uint32_t dest_y;
    uint32_t dest_z;
    uint8_t unknown2[6];
    uint8_t direction_facing; // HDE_HERO_DIRECTION
    uint8_t unknown3[7];
    uint16_t movement_points;
    uint8_t unknown4[64];
    uint32_t creature_types[7];
    uint32_t creature_quantities[7];
    uint8_t secondary_skill_levels[28]; // Byte 0 is Pathfinding, Byte 7 Wisdom etc... Values 1-3
    uint8_t secondary_skill_slot[28];  //  Byte 0 is Pathfinding, Byte 7 Wisdom etc... Values 1-8, number of slot
    uint32_t secondary_skill_count; // Number of skills learned
    uint8_t unknown5[40];
    struct HDE_HERO_ARTIFACTS artifacts;
    uint8_t unknown6[140];
    uint8_t attack;
    uint8_t defense;
    uint8_t spell_power;
    uint8_t knowledge;
    uint8_t unknown7[24];
};

#define HDE_HERO_SAME_COORDINATES(h1, h2) (h1->x == h2->x && h1->y == h2->y && h1->z == h2->z)

#pragma pack(pop)

#endif
