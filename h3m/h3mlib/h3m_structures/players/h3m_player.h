// Created by John Åkerblom 2014-11-20

#ifndef __H3M_PLAYER_H_DEF__
#define __H3M_PLAYER_H_DEF__

// Included by h3m.h

#include "../utils/msvc_comp_stdint.h"
#include "h3m_player_ai.h"

#pragma pack(push, 1)

////////////////////////////////////////////////////////////////////////////////
// ROE
////////////////////////////////////////////////////////////////////////////////

// BINARY COMPATIBLE
struct H3M_PLAYER_EXT_ROE_DEFAULT       // Seen in The Mandate of Heaven (ROE)
{
    uint8_t starting_hero_is_random;
    uint8_t starting_hero_type; // MUST be 0xFF for this struct 
};                              // Total size 8(6+2)

// BINARY COMPATIBLE
struct H3M_PLAYER_EXT_WITH_TOWN_ROE     // Seen in The Mandate of Heaven (ROE)
{
    // ROE - no starting_town_create_hero uint here
    // ROE - no starting_town_type uint here
    uint8_t starting_town_xpos;
    uint8_t starting_town_ypos;
    uint8_t starting_town_zpos;

    uint8_t starting_hero_is_random;
    uint8_t starting_hero_type; // MUST be 0xFF for this struct
};                              // Total size 11(6+5) ROE

// BINARY COMPATIBLE, LAST ELEMENT DYNAMIC - SIZE NOT CONSTANT
struct H3M_PLAYER_EXT_WITH_HERO_ROE     // Seen in The Mandate of Heaven (ROE)
{
    uint8_t starting_hero_is_random;
    uint8_t starting_hero_type; // CANNOT be 0xFF for this struct

    uint8_t starting_hero_face;
    uint32_t starting_hero_name_size;
    uint8_t starting_hero_name[];       // <starting_hero_name_size>
};                              // Total size 13+?(6+7+?), ? = <starting_hero_name_size>

// BINARY COMPATIBLE, LAST ELEMENT DYNAMIC - SIZE NOT CONSTANT
struct H3M_PLAYER_EXT_WITH_TOWN_AND_HERO_ROE    // Never seen, merely predicted
{
    // ROE - no starting_town_create_hero uint here
    // ROE - no starting_town_type uint here
    uint8_t starting_town_xpos;
    uint8_t starting_town_ypos;
    uint8_t starting_town_zpos;

    uint8_t starting_hero_is_random;
    uint8_t starting_hero_type; // CANNOT be 0xFF for this struct

    uint8_t starting_hero_face;
    uint32_t starting_hero_name_size;
    uint8_t starting_hero_name[];       // <starting_hero_name_size
};                              // Total size 16+?(6+10+?), ? = <starting_hero_name_size>

union H3M_PLAYER_EXT_ROE {
    struct H3M_PLAYER_EXT_ROE_DEFAULT e0;
    struct H3M_PLAYER_EXT_WITH_TOWN_ROE e1;
    struct H3M_PLAYER_EXT_WITH_HERO_ROE e2;
    struct H3M_PLAYER_EXT_WITH_TOWN_AND_HERO_ROE e3;
};

////////////////////////////////////////////////////////////////////////////////
// AB/SOD
////////////////////////////////////////////////////////////////////////////////

// BINARY COMPATIBLE, LAST ELEMENT DYNAMIC - SIZE NOT CONSTANT
struct H3M_PLAYER_EXT_ABSOD_DEFAULT     // Seen in War of the Mighty, Arrogance (AB, SOD)
{
    uint8_t starting_hero_is_random;
    uint8_t starting_hero_type; // MUST be 0xFF for this struct 
    uint8_t starting_hero_face;

    uint32_t starting_hero_name_size;
    uint8_t starting_hero_name[];       // <starting_hero_name_size>
};                              // Total size AB/SOD 14+?(7+7+?)/15(8+7+?), ? = <starting_hero_name_size> which can be 0 

// BINARY COMPATIBLE, LAST ELEMENT DYNAMIC - SIZE NOT CONSTANT
struct H3M_PLAYER_EXT_WITH_TOWN_ABSOD   // Seen in War of the Mighty, Arrogance (AB, SOD)
{
    uint8_t starting_town_create_hero;  // AB/SOD Feature
    uint8_t starting_town_type; // AB/SOD Feature
    uint8_t starting_town_xpos;
    uint8_t starting_town_ypos;
    uint8_t starting_town_zpos;

    uint8_t starting_hero_is_random;
    uint8_t starting_hero_type; // MUST be 0xFF for this struct
    uint8_t starting_hero_face;

    uint32_t starting_hero_name_size;
    uint8_t starting_hero_name[];       // <starting_hero_name_size>
};                              // Total size AB/SOD 19+?(7+12+?)/20+?(8+12+?), ? = <starting_hero_name_size> which can be 0

// BINARY COMPATIBLE, LAST ELEMENT DYNAMIC - SIZE NOT CONSTANT
struct H3M_PLAYER_EXT_WITH_HERO_ABSOD   // Seen in Pandora's Box (SOD)
{
    uint8_t starting_hero_is_random;
    uint8_t starting_hero_type; // CANNOT be 0xFF for this struct
    uint8_t starting_hero_face;

    uint32_t starting_hero_name_size;
    uint8_t starting_hero_name[];       // <starting_hero_name_size>

};                              // Total size AB/SOD 14+?(7+7+?)/15+?(8+7+?), ? = starting_hero_name_size

// BINARY COMPATIBLE, LAST ELEMENT DYNAMIC - SIZE NOT CONSTANT
struct H3M_PLAYER_EXT_WITH_TOWN_AND_HERO_ABSOD {
    uint8_t starting_town_create_hero;  // AB/SOD Feature
    uint8_t starting_town_type; // AB/SOD Feature
    uint8_t starting_town_xpos;
    uint8_t starting_town_ypos;
    uint8_t starting_town_zpos;

    uint8_t starting_hero_is_random;
    uint8_t starting_hero_type; // CANNOT be 0xFF for this struct
    uint8_t starting_hero_face;

    uint32_t starting_hero_name_size;
    uint8_t starting_hero_name[];       // <starting_hero_name_size>
};

union H3M_PLAYER_EXT_ABSOD {
    struct H3M_PLAYER_EXT_ABSOD_DEFAULT e0;
    struct H3M_PLAYER_EXT_WITH_TOWN_ABSOD e1;
    struct H3M_PLAYER_EXT_WITH_HERO_ABSOD e2;
    struct H3M_PLAYER_EXT_WITH_TOWN_AND_HERO_ABSOD e3;
};

/////////////////////////////////////

// NOT BINARY COMPATIBLE - CONTAINS DYNAMIC ELEMENTS
struct H3M_PLAYER_ROE {
    uint8_t can_be_human;
    uint8_t can_be_computer;
    uint8_t behavior;
    // ROE - no allowed_alignments uint here
    uint8_t town_types;         // bitfield
    // ROE - no town_conflux uint here
    uint8_t unknown1;
    uint8_t has_main_town;

    union H3M_PLAYER_EXT_ROE u;
};                              // Size before union: 6

// NOT BINARY COMPATIBLE - CONTAINS DYNAMIC ELEMENTS
struct H3M_PLAYER_AB {
    uint8_t can_be_human;
    uint8_t can_be_computer;
    uint8_t behavior;
    // AB - no allowed_alignments uint here
    uint8_t town_types;         // bitfield
    uint8_t town_conflux;       // 8-Con, AB/SOD feature
    uint8_t unknown1;           // Russians: "Whether the player owns Random Town"
    uint8_t has_main_town;

    union H3M_PLAYER_EXT_ABSOD u;
};                              // Size before union: 7

// NOT BINARY COMPATIBLE - CONTAINS DYNAMIC ELEMENTS
struct H3M_PLAYER_SOD {
    uint8_t can_be_human;
    uint8_t can_be_computer;
    uint8_t behavior;
    uint8_t allowed_alignments; // SOD feature
    uint8_t town_types;         // bitfield
    uint8_t town_conflux;       // 8-Con, AB/SOD feature
    uint8_t unknown1;           // Russians: "Whether the player owns Random Town"
    uint8_t has_main_town;

    union H3M_PLAYER_EXT_ABSOD u;
};                              // Size before union: 8

union H3M_PLAYER {
    struct H3M_PLAYER_ROE roe;
    struct H3M_PLAYER_AB ab;
    struct H3M_PLAYER_SOD sod;
};

#pragma pack(pop)

#endif
