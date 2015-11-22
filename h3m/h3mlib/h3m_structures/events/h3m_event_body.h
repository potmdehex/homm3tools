// Created by John Åkerblom 2014-12-15

#ifndef __H3M_EVENT_BODY_H_DEF__
#define __H3M_EVENT_BODY_H_DEF__

// Included by h3m.h

#include "../msvc_comp_stdint.h"

#pragma pack(push, 1)

// BINARY COMPATIBLE
struct H3M_EVENT_BODY_ROEAB {
    struct H3M_COMMON_RESOURCES resources;
    uint8_t applies_to_players;
    uint8_t applies_to_computer;
    uint16_t first_occurence;
    uint8_t subsequent_occurences;
    uint8_t unknown2[17];
};

// BINARY COMPATIBLE
struct H3M_EVENT_BODY_SOD {
    struct H3M_COMMON_RESOURCES resources;
    uint8_t applies_to_players;
    uint8_t applies_to_human;   // SOD feature
    uint8_t applies_to_computer;
    uint16_t first_occurence;
    uint8_t subsequent_occurences;
    uint8_t unknown1[17];
};

union H3M_EVENT_BODY {
    struct H3M_EVENT_BODY_ROEAB roe;
    struct H3M_EVENT_BODY_ROEAB ab;
    struct H3M_EVENT_BODY_ROEAB roeab;
    struct H3M_EVENT_BODY_SOD sod;
};

#pragma pack(pop)

#endif
