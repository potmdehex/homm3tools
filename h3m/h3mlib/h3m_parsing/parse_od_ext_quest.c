// Created by John Åkerblom 2014-11-22

#include "parse_od.h"
#include "parsing_common.h"

size_t sizeof_quest(uint8_t *objective, uint8_t quest_type)
{
    switch (quest_type) {
    case Q_NONE:
        return 0;
    case Q_EXPERIENCE:
        return sizeof(((struct H3M_OD_BODY_EXT_QUEST *)NULL)->objective->
            q_experience);
    case Q_PRIMARY_SKILLS:
        return sizeof(((struct H3M_OD_BODY_EXT_QUEST *)NULL)->objective->
            q_primary_skills);
    case Q_DEFEAT_HERO:
        return sizeof(((struct H3M_OD_BODY_EXT_QUEST *)NULL)->objective->
            q_defeat_hero);
    case Q_DEFEAT_MONSTER:
        return sizeof(((struct H3M_OD_BODY_EXT_QUEST *)NULL)->objective->
            q_defeat_monster);
    case Q_RESOURCES:
        return sizeof(((struct H3M_OD_BODY_EXT_QUEST *)NULL)->objective->
            q_resources);
    case Q_BE_HERO:            // SOD only
        return sizeof(((struct H3M_OD_BODY_EXT_QUEST *)NULL)->objective->
            q_be_hero);
    case Q_BE_PLAYER:          // SOD only
        return sizeof(((struct H3M_OD_BODY_EXT_QUEST *)NULL)->objective->
            q_be_player);
    case Q_CREATURES:
        // sizeof count + (count * element size)
        return sizeof(uint8_t) +
            (*objective * sizeof(struct H3M_COMMON_CREATURE_SLOT_ABSOD));
    case Q_ARTIFACTS:
        // sizeof count + (count * element size), ROE artifact quest handled in Seer's Hut (same byte as quest type)
        return sizeof(uint8_t) +
            (*objective * sizeof(H3M_COMMON_ARTIFACT_TYPE_ABSOD));
    default:
        break;
    }

    return -1;
}

// Only used in AB/SOD, in RoE the quest structure does not exist
int parse_od_ext_quest(struct H3MLIB_CTX *ctx,
    uint8_t quest_type,
    struct H3M_OD_BODY_EXT_QUEST *quest, struct META_OD_ENTRY *meta_od_entry)
{
    struct H3M *p = &ctx->h3m;
    struct H3MLIB_PARSING *parsing = &ctx->parsing;
    const uint32_t fm = p->format;
    size_t n = 0;
    uint8_t *dbg = &parsing->raw[parsing->offset];
    UT_array *p_array_tmp = NULL;

    // One interesting thing to note is that the maped allows creating
    // a Seer's Hut which has several of the same artifact as its objective.
    // Maps containing such a Seer's Hut can be saved, but upon reopening
    // the object is removed from the map. The data looks malformed, containing
    // only a count of 1 and one artifact id, missing 3 bytes compared to one
    // which only has 1 artifact as its objective.

    n = sizeof_quest(&parsing->raw[parsing->offset], quest_type);
    quest->objective = calloc(1, n);
    SAFE_READ_N(quest->objective, n, parsing);

    SAFE_READ_SIZEOF(&quest->deadline_and_mesg.deadline, parsing)

    if (0 != quest_type) {
        // Map "The story of the Fool (Traemask2.h3m") 9697 bytes...
        SAFE_READ_STRING(quest->deadline_and_mesg.proposal_mesg_size, 16384,
            quest->deadline_and_mesg.proposal_mesg, parsing)
        SAFE_READ_STRING(quest->deadline_and_mesg.progress_mesg_size, 16384,
            quest->deadline_and_mesg.progress_mesg, parsing)
        SAFE_READ_STRING(quest->deadline_and_mesg.completion_mesg_size,
            16384, quest->deadline_and_mesg.completion_mesg, parsing)
    }

    return 0;
}
