// Created by John Åkerblom 2014-12-15

#ifndef __H3M_OD_BODY_EXT_QUEST_H_DEF__
#define __H3M_OD_BODY_EXT_QUEST_H_DEF__

// Included by h3m_od_body_ext.h

#include "../../utils/msvc_comp_stdint.h"
#include "../common/h3m_common.h"

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

union H3M_OD_BODY_EXT_REWARD {
	H3M_COMMON_EXPERIENCE r_experience;
	H3M_COMMON_SPELL_POINTS r_spell_points;
	H3M_COMMON_MORALE r_morale;
	H3M_COMMON_LUCK r_luck;
	struct H3M_COMMON_RESOURCE r_resource;
	struct H3M_COMMON_PRIMARY_SKILL r_primary_skill;
	struct H3M_COMMON_SECONDARY_SKILL r_secondary_skill;
	union
	{
		H3M_COMMON_ARTIFACT_TYPE_ROE roe;
		H3M_COMMON_ARTIFACT_TYPE_ABSOD absod;
	} r_artifact;
	H3M_COMMON_SPELL_TYPE r_spell;
	union 
	{
		struct H3M_COMMON_CREATURE_SLOT_ROE roe;
		struct H3M_COMMON_CREATURE_SLOT_ABSOD absod;
	} r_creatures;
};

#pragma pack(pop)

#endif
