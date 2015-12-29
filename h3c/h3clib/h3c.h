// Created by John Åkerblom 2015-12-03

#ifndef __H3C_H_DEF__
#define __H3C_H_DEF__

#include <stdint.h>

#define H3C_MAP_ARMAGEDDON 0x0D // 8 maps
#define H3C_MAP_BIRTH_OF_A_BARBARIAN 0x0F // 5 maps

struct H3C_HEADER
{
    uint32_t format;
    uint8_t campaign_map;
    uint32_t name_size;
    uint8_t *name; // amount of bytes = name_size
    uint32_t desc_size;
    uint8_t *desc; // amount of bytes = desc_size
    uint8_t fixed_difficulty;
    uint8_t music;
};

struct H3C_MAP_INFO
{
    uint32_t name_size;
    uint8_t *name; // amount of bytes = name_size
    uint32_t h3m_size; // size of this map's h3m data found in this file
    uint8_t unknown[20];
    // difficulty, starting bonus etc come here. A lot of these are optional, with a preceding byte that if it is 00 means the bytes for that setting are omitted
};

struct H3C
{
    struct H3C_HEADER header;
    struct H3C_MAP_INFO map_info[8]; // difficulty, starting bonus etc, one for each map
    uint8_t *h3m[8]; // H3M data, one for each map
};

#endif
