// Created by John Åkerblom 2014-12-15

#ifndef __H3M_OD_BODY_EXT_QUEST_H_DEF__
#define __H3M_OD_BODY_EXT_QUEST_H_DEF__

// Included by h3m_od_body_ext.h

#include "../../msvc_comp_stdint.h"
#include "../h3m_common.h"

#pragma pack(push, 1)

// BINARY COMPATIBLE
struct H3M_OD_BODY_EXT_QUEST_ARTIFACT {
    uint8_t count;
    // maped allows max 8 entries here, ofc representation used here can be changed to support more
    uint16_t artifacts[8];      // only <count entries> used
};

// BINARY COMPATIBLE
struct H3M_OD_BODY_EXT_QUEST_CREATURES {
    uint8_t count;
    // This quest does not exist in ROE so absod creature slots safely used here
    // maped allows max 8 entries here, ofc representation used here can be changed to support more
    struct H3M_COMMON_CREATURE_SLOT_ABSOD creatures[8]; // only <count entries> used
};

#define Q_NONE 0x00
#define Q_EXPERIENCE 0x01
#define Q_PRIMARY_SKILLS 0x02
#define Q_DEFEAT_HERO 0x03
#define Q_DEFEAT_MONSTER 0x04
#define Q_ARTIFACTS 0x05
#define Q_CREATURES 0x06
#define Q_RESOURCES 0x07
#define Q_BE_HERO 0x08
#define Q_BE_PLAYER 0x09

union H3M_OD_BODY_EXT_QUEST_OBJECTIVE {
    H3M_COMMON_EXPERIENCE q_any;
    H3M_COMMON_EXPERIENCE q_experience;
    struct H3M_COMMON_PRIMARY_SKILLS q_primary_skills;
    H3M_COMMON_ABSOD_ID q_defeat_monster;
    H3M_COMMON_ABSOD_ID q_defeat_hero;
    struct H3M_OD_BODY_EXT_QUEST_ARTIFACT q_artifacts;
    struct H3M_OD_BODY_EXT_QUEST_CREATURES q_creatures;
    struct H3M_COMMON_RESOURCES q_resources;
    H3M_COMMON_HERO_TYPE q_be_hero;
    H3M_COMMON_PLAYER_FLAG q_be_player;
};

// NOT BINARY COMPATIBLE
struct H3M_OD_BODY_EXT_QUEST_DEADLINE_AND_MESG {
    uint32_t deadline;

    // The following only if 0xFF != quest_type:
    uint32_t proposal_mesg_size;        // SOD feature
    uint8_t *proposal_mesg;     // SOD feature
    uint32_t progress_mesg_size;        // SOD feature
    uint8_t *progress_mesg;     // SOD feature
    uint32_t completion_mesg_size;      // SOD feature
    uint8_t *completion_mesg;   // SOD feature
};

// NOT BINARY COMPATIBLE
struct H3M_OD_BODY_EXT_QUEST {
    union H3M_OD_BODY_EXT_QUEST_OBJECTIVE *objective;
    struct H3M_OD_BODY_EXT_QUEST_DEADLINE_AND_MESG deadline_and_mesg;
};

#pragma pack(pop)

#endif
