// Created by John Åkerblom 2014-11-22

#include "parse_od.h"
#include "parsing_common.h"
#include "../h3m_structures/object_details/h3m_od_body_ext_quest.h"

#if 0
struct REWARD_RESOURCE {
    uint8_t type;
    uint32_t quantity;
};
struct REWARD_PRIMARY_SKILL {
    uint8_t type;
    uint8_t bonus;
};
#endif

size_t sizeof_reward(uint32_t fm, uint8_t reward_type)
{
    switch (reward_type) {
    case R_NONE:
        return 0;
    case R_EXPERIENCE:
        return sizeof(((union H3M_OD_BODY_EXT_REWARD *) NULL)->r_experience);
    case R_SPELL_POINTS:
        return sizeof(((union H3M_OD_BODY_EXT_REWARD *) NULL)->r_spell_points);
    case R_ARTIFACT:
        return (fm > H3M_FORMAT_ROE)
            ? sizeof(((union H3M_OD_BODY_EXT_REWARD *) NULL)->r_artifact.absod)
            : sizeof(((union H3M_OD_BODY_EXT_REWARD *) NULL)->r_artifact.roe);
    case R_LUCK:
        return sizeof(((union H3M_OD_BODY_EXT_REWARD *) NULL)->r_luck);
    case R_MORALE:
        return sizeof(((union H3M_OD_BODY_EXT_REWARD *) NULL)->r_morale);
    case R_SPELL:
        return sizeof(((union H3M_OD_BODY_EXT_REWARD *) NULL)->r_spell);
    case R_RESOURCE:
        return sizeof(((union H3M_OD_BODY_EXT_REWARD *) NULL)->r_resource);
    case R_PRIMARY_SKILL:
        return sizeof(((union H3M_OD_BODY_EXT_REWARD *) NULL)->r_primary_skill);
    case R_SECONDARY_SKILL:
        return sizeof(((union H3M_OD_BODY_EXT_REWARD *) NULL)->r_secondary_skill);
    case R_CREATURES:
        return (fm > H3M_FORMAT_ROE)
            ? sizeof(((union H3M_OD_BODY_EXT_REWARD *) NULL)->r_creatures.absod)
            : sizeof(((union H3M_OD_BODY_EXT_REWARD *) NULL)->r_creatures.roe);
    default:
        break;
    }
    return -1;
}

int parse_od_seers_hut(struct H3MLIB_CTX *ctx,
    struct H3M_OD_ENTRY *od_entry, struct META_OD_ENTRY *meta_od_entry)
{
    struct H3M *p = &ctx->h3m;
    struct H3MLIB_PARSING *parsing = &ctx->parsing;
    const uint32_t fm = p->format;
    struct H3M_OD_BODY_DYNAMIC_SEERS_HUT *body = NULL;
    size_t n = 0;

    od_entry->body = calloc(1, sizeof(*body));
    body = (struct H3M_OD_BODY_DYNAMIC_SEERS_HUT *)od_entry->body;
    meta_od_entry->binary_compatible = 0;
    meta_od_entry->body_size = sizeof(*body);

    // In RoE the only type of quest is "Return with artifacts", which in ROE is
    // always only one artifact. The quest hence consists of one byte signifying
    // the artifact type, 0xFF means there is no quest.
    // tl;dr: For RoE quest type == artifact type of artifact quest

    SAFE_READ_SIZEOF(&body->quest_type, parsing)
    if (fm >= H3M_FORMAT_AB && 0 != body->quest_type) {
        if (0 != parse_od_ext_quest(ctx, body->quest_type, &body->quest,
                meta_od_entry)) {
            return 1;
        }
    }

    SAFE_READ_SIZEOF(&body->reward_type, parsing)
    if (0 != body->reward_type) {
        n = sizeof_reward(fm, body->reward_type);
        if (-1 == n || n > 5) {
            return 1;
        }
        body->reward = calloc(1, n);
        SAFE_READ_N(body->reward, n, parsing)
    }

    SAFE_READ_SIZEOF_BUF(body->unknown1, parsing)

    return 0;
}
