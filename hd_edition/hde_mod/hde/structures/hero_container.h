// Created by John Åkerblom 2015-04-17

#ifndef __HERO_CONTAINER_H_DEF__
#define __HERO_CONTAINER_H_DEF__

#include <stdint.h>

#include "hero.h"

#pragma pack(push, 1)

// Don't really know about this struct, it might possibly be
// used to contain other pointers than pointers to heroes
// as well. Observed passed as argument to various functions, e.g
// when moving artifact

struct HDE_HERO_CONTAINER
{
    uint8_t unknown[108];
    struct HDE_HERO *hero;
    // unknown2[??]
};

#pragma pack(pop)

#endif
