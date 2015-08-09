// Created by John Åkerblom 2014-11-21

#include "parse_player_ai.h"

#include "../h3mlib.h"
#include "../h3m_structures/h3m.h"
#include "../safe_read.h"

#include <stdlib.h>

int parse_player_ai_absod(int idx, struct H3MLIB_CTX *ctx)
{
    struct H3M *p = &ctx->h3m;
    struct H3MLIB_META *meta = &ctx->meta;
    struct H3MLIB_PARSING *parsing = &ctx->parsing;
    int orig_off = ctx->parsing.offset;
    int ret = 0;
    size_t n = 0;
    unsigned int i = 0;
    struct H3M_PLAYER_AI_ABSOD *absod = &p->player_ai[idx];

    SAFE_READ_SIZEOF(&absod->unknown1, parsing);
    SAFE_READ_SIZEOF(&absod->heroes_count, parsing);

    SAFE_CHECK_N(absod->heroes_count, 255)

    meta->player_ai_heroes_sizes[idx] =
        calloc(absod->heroes_count, sizeof(uint32_t));
    absod->heroes = calloc(absod->heroes_count, sizeof(void *));
    for (i = 0; i < absod->heroes_count; ++i) {
        absod->heroes[i] = calloc(1,
            sizeof(struct H3M_PLAYER_AI_HERO_ABSOD) + H3M_MAX_HERO_NAME + 1);
        SAFE_READ_N(absod->heroes[i], sizeof(struct H3M_PLAYER_AI_HERO_ABSOD),
            parsing);
        if (absod->heroes[i]->name_size > H3M_MAX_HERO_NAME) {
            return 1;
        }
        SAFE_READ_N(absod->heroes[i]->name, absod->heroes[i]->name_size,
            parsing);
        meta->player_ai_heroes_sizes[idx][i] =
            sizeof(struct H3M_PLAYER_AI_HERO_ABSOD) +
            absod->heroes[i]->name_size;
    }

    if (NULL != ctx->callbacks.cb_parse) {
        n = parsing->offset - orig_off;
        ret = ctx->callbacks.cb_parse(orig_off, "player_ai",
            &parsing->raw[orig_off], n, ctx->callbacks.cb_data);
    }

    return ret;
}
