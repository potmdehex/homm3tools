// Created by John Åkerblom 2014-11-22

#include "parse_od.h"
#include "parsing_common.h"

#define R_NONE 0x00
#define R_EXPERIENCE 0x01
#define R_SPELL_POINTS 0x02
#define R_MORALE 0x03
#define R_LUCK 0x04
#define R_RESOURCE 0x05
#define R_PRIMARY_SKILL 0x06
#define R_SECONDARY_SKILL 0x07
#define R_ARTIFACT 0x08
#define R_SPELL 0x09
#define R_CREATURE 0x0A

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
    // TODO: change to be like _sizeof_win_cond in parse_ai_win_cond.c
    // once there exists proper reward structs. format will probably be
    // needed to decide sizes of some (creatures and artifacts)?
    switch (reward_type) {
    case R_NONE:
        return 0;
    case R_EXPERIENCE:
    case R_SPELL_POINTS:
        return 4;
    case R_ARTIFACT:
        return (fm == H3M_FORMAT_ROE) ? 1 : 2;
    case R_LUCK:
    case R_MORALE:
    case R_SPELL:
        return 1;
    case R_RESOURCE:
        return 5;
    case R_PRIMARY_SKILL:
    case R_SECONDARY_SKILL:
        return 2;
    case R_CREATURE:
        return (H3M_FORMAT_ROE == fm) ? 3 : 4;
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
    // AB/SOD actually have several quest types, parse if quest_type is not 0
    if (fm >= H3M_FORMAT_AB && 0 != body->quest_type) {
        if (0 != parse_od_ext_quest(ctx, body->quest_type, &body->quest,
                meta_od_entry)) {
            return 1;
        }
    }

    SAFE_READ_SIZEOF(&body->reward_type, parsing)
    if (0 != body->reward_type) {
        // TODO have reward struct/union and actually parse rewards
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
