// Created by John Åkerblom 2014-12-15

#ifndef __H3M_OA_BODY_H_DEF__
#define __H3M_OA_BODY_H_DEF__

// Included by h3m.h

#include "../utils/msvc_comp_stdint.h"

#pragma pack(push, 1)

// BINARY COMPATIBLE
struct H3M_OA_BODY {
    // The passable and active arrays are bitfields representing an 8x6 tile
    // region where bit 1 marks passable and bit 0 impassable. Counting goes
    // from left to right downwards towards the bottom right corner. This means
    // that first bit in passable[0] is [x-7, y-5] from bottom right corner and
    // last bit in passable[6] is the bottom right corner.
    uint8_t passable[6];
    uint8_t active[6];
    uint16_t allowed_landscapes;
    uint16_t landscape_group;
    uint32_t object_class;
    uint32_t object_number;
    // 1 - towns  2 - monsters  5 - treasure
    // 3 - heroes 4 - artifacts
    uint8_t object_group;
    uint8_t above;
    uint8_t unknown[16];
};

#pragma pack(pop)

#endif
