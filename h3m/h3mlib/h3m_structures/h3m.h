// Created by John Åkerblom 2014-11-18

#ifndef __H3M_H_DEF__
#define __H3M_H_DEF__

#include "../msvc_comp_stdint.h"

#define H3M_MAX_PLAYERS 8

#include "additional_info/h3m_ai.h"
#include "basic_info/h3m_bi.h"
#include "events/h3m_event.h"
#include "object_attributes/h3m_oa.h"
#include "object_details/h3m_od.h"
#include "players/h3m_player.h"
#include "tiles/h3m_tile.h"

// Refer to h3m_description.english.txt for basic documentation. 
// https://github.com/potmdehex/homm3tools/tree/master/h3m/h3mlib/h3m_structures/h3m_description.english.txt
// NOTE: 
// This document has several errors, only documents SoD format and leaves
// out the two last sections of the h3m format, Object Details and Events.
// Naming for structs and struct members taken from this document.
#pragma pack(push, 1)

// NOT BINARY COMPATIBLE - CONTAINS DYNAMIC ELEMENTS
struct H3M {
    uint32_t format; // Map format
    union H3M_BI bi; // bi = basic information, map size, name, desc etc
    union H3M_PLAYER **players; // Player starting
    struct H3M_PLAYER_AI_ABSOD player_ai[H3M_MAX_PLAYERS]; // AB/SoD
    union H3M_AI ai; // ai = Additional Information, victory/loss condition etc
    struct H3M_TILE *tiles; // Terrain, roads and rivers
    struct H3M_OA oa; // Attributes of objects, but not details like coords
    struct H3M_OD od; // Details of object instances appearing, e.g coordinates
    struct H3M_EVENT event; // Global events

    // Maps commonly end with 124 bytes of null padding. Extra content at end
    // is ok.
};

#pragma pack(pop)

#endif
