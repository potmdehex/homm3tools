// Created by John Åkerblom 2014-12-15

#ifndef __H3M_EVENT_H_DEF__
#define __H3M_EVENT_H_DEF__

// Included by h3m.h

#include "../msvc_comp_stdint.h"
#include "h3m_event_body.h"

#pragma pack(push, 1)

// NOT BINARY COMPATIBLE - CONTAINS DYNAMIC ELEMENTS
struct H3M_EVENT_HEADER {
    uint32_t name_size;
    uint8_t *name;              // DYNAMIC MEMBER
    uint32_t mesg_size;
    uint8_t *mesg;              // DYNAMIC MEMBER
};

// NOT BINARY COMPATIBLE - CONTAINS DYNAMIC ELEMENTS
struct H3M_EVENT_ENTRY {
    struct H3M_EVENT_HEADER header;     // DYNAMIC MEMBER
    union H3M_EVENT_BODY body;
};

// NOT BINARY COMPATIBLE - CONTAINS DYNAMIC ELEMENTS
struct H3M_EVENT {
    uint32_t count;
    struct H3M_EVENT_ENTRY *entries;    // DYNAMIC MEMBER
};

#pragma pack(pop)

#endif
