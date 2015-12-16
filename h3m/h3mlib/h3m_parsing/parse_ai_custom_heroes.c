// Created by John Åkerblom 2014-11-21

#include "../h3mlib.h"
#include "parse_ai.h"
#include "../utils/safe_read.h"

#include <stdlib.h>
#include <string.h>

// TODO: in order to support any mods, something needs to be done about the .sod
// references, such as using the FS_ macros instead

int parse_ai_custom_heroes(struct H3MLIB_CTX *ctx)
{
    struct H3M *p = &ctx->h3m;
    //struct H3MLIB_META*meta = &ctx->meta;
    struct H3MLIB_PARSING *parsing = &ctx->parsing;
    //int orig_off = ctx->parsing.offset;
    //int off = orig_off;
    //int ret = 0;
    unsigned int i = 0;
    struct H3M_AI_CUSTOM_HERO_SOD *hero = NULL;

    SAFE_READ_SIZEOF(&p->ai.sod.custom_heroes_count, parsing)
    if (0 == p->ai.sod.custom_heroes_count) {
        return 0;
    }

    p->ai.sod.custom_heroes =
        calloc(p->ai.sod.custom_heroes_count, sizeof(*hero));
    for (i = 0; i < p->ai.sod.custom_heroes_count; ++i) {
        hero = &p->ai.sod.custom_heroes[i];
        SAFE_READ_SIZEOF(&hero->type, parsing)
        SAFE_READ_SIZEOF(&hero->face, parsing)
        SAFE_READ_SIZEOF(&hero->name_size, parsing)
        if (hero->name_size > 0) {
        SAFE_ALLOC_N(hero->name, hero->name_size + 1, H3M_MAX_HERO_NAME + 1)
            SAFE_READ_N(hero->name, hero->name_size, parsing)
        }
        SAFE_READ_SIZEOF(&hero->allowed_players, parsing)
    }

    return 0;
}
