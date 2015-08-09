// Created by John Åkerblom 2015-03-13

#ifndef __SWAPMGR_H_DEF__
#define __SWAPMGR_H_DEF__

#include "hero.h"
#include "mgr.h"

#include <stdint.h>

#pragma pack(push, 1)

struct HDE_SWAPMGR
{
    struct HDE_MGR base;
    uint32_t unknown1; // value 1 observed
    void **ftable1;
    void **ftable2;
    struct HDE_HERO *hero_left;
    struct HDE_HERO *hero_right;
    uint32_t selected_owner; // 0 if owner of selected stack is left, 1 if right
    uint32_t inspected_owner; // 0 if owner of inspected stack is left, 1 if right
    uint32_t selected_slot; // slot of selected stack
    uint32_t inspected_slot; // slot of inspected stack
    uint32_t state; // -1 if no ongoing action, 0 if stack being selected/inspected
    // unknown2[??]
};

#pragma pack(pop)

#endif
