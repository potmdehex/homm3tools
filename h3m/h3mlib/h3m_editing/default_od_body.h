// Created by John Åkerblom 2014-12-23

#ifndef __DEFAULT_OD_BODY_H_DEF__
#define __DEFAULT_OD_BODY_H_DEF__

#include "../h3mlib.h"
#include "../meta/meta_object.h"
#include "../utils/msvc_comp_stdint.h"

int get_default_od_body(uint32_t fm, enum META_OBJECT type, uint8_t **body,
    int *binary_compatible, size_t *body_size);

#endif
