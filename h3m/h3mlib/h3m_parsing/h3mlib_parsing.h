// Created by John Åkerblom 2014-11-18

#ifndef __H3MLIB_PARSING_H_DEF__
#define __H3MLIB_PARSING_H_DEF__

#include "../msvc_comp_stdint.h"

// Context used by parsing code.
struct H3MLIB_PARSING {
    uint8_t *raw;
    size_t raw_size;
    unsigned int offset;
};

#endif
