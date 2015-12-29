// Created by John Åkerblom 2015-12-03

#ifndef __H3CLIB_CTX_H_DEF__
#define __H3CLIB_CTX_H_DEF__

#include "h3c.h"

#include <stdint.h>

struct H3CLIB_CTX
{
    struct H3C h3c;
    uint8_t *raw;
    uint32_t raw_size;
};

#endif
