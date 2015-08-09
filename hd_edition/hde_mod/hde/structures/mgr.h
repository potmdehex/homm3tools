// Created by John Åkerblom 2015-03-13

#ifndef __MGR_H_DEF__
#define __MGR_H_DEF__

#include <stdint.h>

#pragma pack(push, 1)

struct HDE_MGR_VTABLE
{
    void *fn_constructor;
    void *fn_destructor;
    void *fn_update;
    void *fn_other[]; // advManager has nothing here, others do
};

struct HDE_MGR
{
    struct HDE_MGR_VTABLE *vtable;
    struct HDE_MGR *parent;
    struct HDE_MGR *child;
    uint32_t unknown_size; // seems to be size of something
    uint32_t unknown_enum; // values 0, 1, 2 observed
    char name[32]; // Examples: advManager, townManager, swapManager
};

#pragma pack(pop)

#endif
