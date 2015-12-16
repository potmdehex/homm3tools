// Created by John Åkerblom 2015-01-24

#ifndef __META_OBJECT_PUSH_OD_H_DEF__
#define __META_OBJECT_PUSH_OD_H_DEF__

#include "../utils/msvc_comp_stdint.h"
#include "h3mlib_meta.h"

int meta_push_od(uint32_t fm, uint8_t *body,
    struct META_OD_ENTRY *meta_od_entry);

#endif
