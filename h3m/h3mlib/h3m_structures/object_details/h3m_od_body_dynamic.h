// Created by John Åkerblom 2014-12-15

#ifndef __H3M_OD_BODY_DYNAMIC_H_DEF__
#define __H3M_OD_BODY_DYNAMIC_H_DEF__

// Included by h3m_od.h

#include "../../msvc_comp_stdint.h"
#include "h3m_od_body_ext_contents.h"
#include "h3m_od_body_ext_guardians.h"
#include "h3m_od_body_ext_mesg_and_treasure.h"
#include "h3m_od_body_ext_town_event.h"
#include "h3m_od_body_ext_quest.h"

#pragma pack(push, 1)

// NOT BINARY COMPATIBLE - CONTAINS DYNAMIC ELEMENTS
struct H3M_OD_BODY_DYNAMIC_HERO {
    uint8_t owner;
    uint8_t type;
    uint8_t has_name;
    struct H3M_COMMON_STRING *name;     // if 0 != has_name
    uint8_t has_experience;     // SOD feature
    uint32_t experience;        // if 0 != has_experience in SOD, always present in ROE/AB
    uint8_t has_face;
    uint8_t face;               // if 0 != has_face
    uint8_t has_secondary_skills;
    struct {
        uint32_t count;
        struct H3M_COMMON_SECONDARY_SKILL skills[];
    } *secondary_skills;        // if 0 != has_secondary_skills
    uint8_t has_creatures;
    union H3M_COMMON_ARMY *creatures;   // if 0 != has_creatures
    uint8_t formation;          // 0 - Spread, 1 - Grouped
    uint8_t has_artifacts;
    union H3M_COMMON_ARTIFACTS *artifacts;
    // TODO artifacts
    uint8_t patrol_radius;      // default 0xFF
    uint8_t has_biography;      // AB/SOD feature
    struct H3M_COMMON_STRING *biography;        // AB/SOD feature if 0 != has_biography
    uint8_t gender;             // AB/SOD feature
    uint8_t has_spells;         // SOD feature
    union {
        uint8_t ab;             // In AB only a single spell can be specified here, 0xFE is default, 0xFF none
        uint8_t sod[9];
    } *spells;                  // AB/SOD feature

    uint8_t has_primary_skills; // SOD feature
    struct H3M_COMMON_PRIMARY_SKILLS *primary_skills;   // SOD feature, if 0 != has_primary_skills
    uint8_t unknown2[16];       // default 0x00
};

// NOT BINARY COMPATIBLE - CONTAINS DYNAMIC ELEMENTS
struct H3M_OD_BODY_DYNAMIC_TOWN {
    uint8_t owner;
    uint8_t has_name;
    struct H3M_COMMON_STRING *name;     // if 0 != has_name
    uint8_t has_creatures;
    union H3M_COMMON_ARMY *creatures;   // DYNAMIC MEMBER, if 0 != has_creatures
    uint8_t formation;
    uint8_t has_buildings;
    struct {
        uint8_t built[6];
        uint8_t disabled[6];
    } *buildings;               // if 0 != has_buildings
    uint8_t has_fort;           // if 0 == has_buildings
    uint8_t must_have_spells[9];        // AB/SOD feature
    uint8_t may_have_spells[9];
    uint32_t event_count;
    struct H3M_OD_BODY_EXT_TOWN_EVENT *events;
    uint8_t alignment;          // SOD feature
    uint8_t unknown1[3];
};

// This structure could be split into three structures
// (normal, preset level, preset alignment). For random dwellings
// with preset alignment the size is always the same - they are not actually
// dynamic.
// NOT BINARY COMPATIBLE - CONTAINS DYNAMIC MEMBERS
struct H3M_OD_BODY_DYNAMIC_RANDOM_DWELLING {
    uint32_t owner;
    uint32_t castle_absod_id;   // not present in "Random Dwelling Castle" etc
    uint8_t alignment[2];       // if 0 != castle_absod_id
    uint8_t min_level;
    uint8_t max_level;
};

// NOT BINARY COMPATIBLE - CONTAINS DYNAMIC MEMBERS
struct H3M_OD_BODY_DYNAMIC_SEERS_HUT {
    // In RoE the only type of quest is "Return with artifacts", which in ROE is
    // always only one artifact. The quest hence consists of one byte signifying
    // the artifact type, 0xFF means quest type is None. In AB/SOD 0x00 means None.
    union {
        uint8_t quest_type;     // AB/SOD feature
        uint8_t artifact_type;  // ROE feature
    };
    struct H3M_OD_BODY_EXT_QUEST quest; // AB/SOD feature, only if 0xFF != quest_type
    uint8_t reward_type;
    // TODO reward union
    uint8_t *reward;            // max size 5
    uint8_t unknown1[2];
};

// NOT BINARY COMPATIBLE - CONTAINS DYNAMIC MEMBERS
struct H3M_OD_BODY_DYNAMIC_MONSTER {
    uint16_t quantity;
    uint8_t disposition;        // likelyhood of joining hero
    uint8_t has_mesg_and_treasure;
    struct H3M_OD_BODY_EXT_MESG_AND_TREASURE mesg_and_treasure; // if 0 != has_mesg_and_treasure
    uint8_t never_flees;
    uint8_t does_not_grow;
    uint8_t unknown1[2];
};

// NOT BINARY COMPATIBLE - CONTAINS DYNAMIC MEMBERS
struct H3M_OD_BODY_DYNAMIC_RESOURCE {
    uint8_t has_guardians;
    struct H3M_OD_BODY_EXT_GUARDIANS guardians;
    uint32_t quantity;
    uint8_t unknown1[4];
};

// NOT BINARY COMPATIBLE - CONTAINS DYNAMIC MEMBERS
struct H3M_OD_BODY_DYNAMIC_ARTIFACT {
    uint8_t has_guardians;
    struct H3M_OD_BODY_EXT_GUARDIANS guardians;
};

// NOT BINARY COMPATIBLE - CONTAINS DYNAMIC MEMBERS
struct H3M_OD_BODY_DYNAMIC_SPELL_SCROLL {
    uint8_t has_guardians;
    struct H3M_OD_BODY_EXT_GUARDIANS guardians;
    uint32_t spell;
};

// NOT BINARY COMPATIBLE - CONTAINS DYNAMIC MEMBERS
struct H3M_OD_BODY_DYNAMIC_EVENT {
    uint8_t has_guardians;
    struct H3M_OD_BODY_EXT_GUARDIANS guardians; // only if 0 != has_guardians
    struct H3M_OD_BODY_EXT_CONTENTS contents;
    uint8_t applies_to_players;
    uint8_t applies_to_computer;
    uint8_t cancel_after_visit;
    uint8_t unknown1[4];
};

// NOT BINARY COMPATIBLE - CONTAINS DYNAMIC MEMBERS
struct H3M_OD_BODY_DYNAMIC_PANDORAS_BOX {
    uint8_t has_guardians;
    struct H3M_OD_BODY_EXT_GUARDIANS guardians;
    struct H3M_OD_BODY_EXT_CONTENTS contents;   // DYNAMIC MEMBER
};

// Sign and Ocean Bottle
// NOT BINARY COMPATIBLE - CONTAINS DYNAMIC MEMBERS
struct H3M_OD_BODY_DYNAMIC_MESSAGE_BEARER {
    uint32_t mesg_size;
    uint8_t *mesg;
    uint8_t unknown1[4];
};

// NOT BINARY COMPATIBLE - CONTAINS DYNAMIC MEMBERS
struct H3M_OD_BODY_DYNAMIC_QUEST_GUARD {
    uint8_t quest_type;
    struct H3M_OD_BODY_EXT_QUEST quest; // only if 0xFF != quest_type
};

#pragma pack(pop)

#endif
