// Created by John Åkerblom 2014-11-22

#include "parse_od.h"
#include "parsing_common.h"

int parse_od_quest_guard(struct H3MLIB_CTX *ctx,
    struct H3M_OD_ENTRY *od_entry, struct META_OD_ENTRY *meta_od_entry)
{
    struct H3M *p = &ctx->h3m;
    struct H3MLIB_PARSING *parsing = &ctx->parsing;
    const uint32_t fm = p->format;
    struct H3M_OD_BODY_DYNAMIC_QUEST_GUARD *body = NULL;
    size_t n = 0;

    od_entry->body = calloc(1, sizeof(*body));
    body = (struct H3M_OD_BODY_DYNAMIC_QUEST_GUARD *)od_entry->body;
    meta_od_entry->binary_compatible = 0;
    meta_od_entry->body_size = sizeof(*body);

    SAFE_READ_SIZEOF(&body->quest_type, parsing)
    if (0x00 != body->quest_type) {
        if (0 != parse_od_ext_quest(ctx, body->quest_type, &body->quest,
                meta_od_entry)) {
            return 1;
        }
    }

    return 0;
}
