// Created by John Åkerblom 2014-11-20

#ifndef __H3M_TILE_H_DEF__
#define __H3M_TILE_H_DEF__

#include "../../utils/msvc_comp_stdint.h"

#define TERRAIN_TYPE_IS_VALID(T) \
    ((T.terrain_type <= T_ROCK)? 1 : 0)

/* There also exists seemingly unused "hidden" terrain sprites. For instance
 * Water has a bunch of these at and close to 0xFF */
#define PICTURE_IS_VALID(T) \
    ((T.terrain_type == T_DIRT          && T.terrain_sprite <= 0x2C) ? 1 : \
    (T.terrain_type == T_SAND           && T.terrain_sprite <= 0x17) ? 1 : \
    (T.terrain_type == T_GRASS          && T.terrain_sprite <= 0x48) ? 1 : \
    (T.terrain_type == T_SNOW           && T.terrain_sprite <= 0x48) ? 1 : \
    (T.terrain_type == T_SWAMP          && T.terrain_sprite <= 0x48) ? 1 : \
    (T.terrain_type == T_ROUGH          && T.terrain_sprite <= 0x48) ? 1 : \
    (T.terrain_type == T_SUBTERRANEAN   && T.terrain_sprite <= 0x48) ? 1 : \
    (T.terrain_type == T_LAVA           && T.terrain_sprite <= 0x48) ? 1 : \
    (T.terrain_type == T_WATER          && T.terrain_sprite <= 0x20) ? 1 : \
    (T.terrain_type == T_ROCK           && T.terrain_sprite <= 0x2F) ? 1 : 0)

#define TILE_IS_VALID(T) \
    (TERRAIN_TYPE_IS_VALID(T) && PICTURE_IS_VALID(T))\

enum TERRAIN_TYPES {
    T_DIRT,
    T_SAND,
    T_GRASS,
    T_SNOW,
    T_SWAMP,
    T_ROUGH,
    T_SUBTERRANEAN,
    T_LAVA,
    T_WATER,
    T_ROCK
};

/* For mirroring H3M_TILE */
#define BIT_HORI 0x01
#define BIT_VERT 0x02

#pragma pack(push, 1)

// BINARY COMPATIBLE
struct H3M_TILE {
    uint8_t terrain_type;
    uint8_t terrain_sprite;
    uint8_t river_type;
    uint8_t river_sprite;
    uint8_t road_type;
    uint8_t road_sprite;
    uint8_t mirroring;
};

#pragma pack(pop)

#endif
