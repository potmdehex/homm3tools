#ifndef __H3STRUCT_H_DEF__
#define __H3STRUCT_H_DEF__

#include <stdint.h>

#pragma pack(push, 1)

// TODO map the unknown members, here is just an offset fixed excerpt from HDE Mod
struct H3STRUCT_HERO
{
    uint16_t x;
    uint16_t y;
    uint16_t z;
    uint8_t unknown1[43];
    uint32_t dest_x;
    uint32_t dest_y;
    uint32_t dest_z;
    uint8_t unknown2[6];
    uint8_t direction_facing;
    uint8_t unknown3[7 + 2]; // 2 extra bytes either here or above not in RoE
    uint32_t movement_pts;
    // continues here
};

#pragma pack(pop)

#endif
