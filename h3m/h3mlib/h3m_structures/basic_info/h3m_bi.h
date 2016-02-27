// Created by John Åkerblom 2014-11-22

#ifndef __H3M_BI_H_DEF__
#define __H3M_BI_H_DEF__

// Included by h3m.h

#include "../utils/msvc_comp_stdint.h"

#pragma pack(push, 1)

// NOT BINARY COMPATIBLE - CONTAINS DYNAMIC ELEMENTS
struct H3M_BI_ROE {
    uint8_t has_hero;
    uint32_t map_size;
    uint8_t has_two_levels;
    uint32_t name_size;
    // name here, <name_size> bytes
    uint8_t *name;
    uint32_t desc_size;
    // desc here, <desc_size> bytes
    uint8_t *desc;
    uint8_t difficulty;
    // ROE - no mastery_cap uint here
};
#define H3M_BI_ANY H3M_BI_ROE

// NOT BINARY COMPATIBLE - CONTAINS DYNAMIC ELEMENTS
struct H3M_BI_ABSOD {
    uint8_t has_hero;
    uint32_t map_size;
    uint8_t has_two_levels;
    uint32_t name_size;
    // name here, <name_size> bytes
    uint8_t *name;
    uint32_t desc_size;
    // desc here, <desc_size> bytes
    uint8_t *desc;
    uint8_t difficulty;
    uint8_t mastery_cap;        // AB/SOD feature
};
#define H3M_BI_AB H3M_BI_ABSOD
#define H3M_BI_SOD H3M_BI_ABSOD

// NOT BINARY COMPATIBLE - CONTAINS DYNAMIC ELEMENTS
struct H3M_BI_HOTA {
    uint32_t internal_format; // HOTA feature
    uint8_t has_hero;
    uint32_t map_size;
    uint8_t has_two_levels;
    uint32_t name_size;
    // name here, <name_size> bytes
    uint8_t *name;
    uint32_t desc_size;
    // desc here, <desc_size> bytes
    uint8_t *desc;
    uint8_t difficulty;
    uint8_t mastery_cap;        // AB/SOD feature
};

union H3M_BI {
    struct H3M_BI_ROE roe;
    struct H3M_BI_ROE any;
    struct H3M_BI_ABSOD ab;
    struct H3M_BI_ABSOD sod;
    struct H3M_BI_ABSOD absod;
    struct H3M_BI_HOTA hota;
};

#pragma pack(pop)

#endif
