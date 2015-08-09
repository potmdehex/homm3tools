// Created by John Åkerblom 2014-11-22

#include "parse_od.h"
#include "parsing_common.h"

// This function is nearly identical to parse_od_artifact, could merge them
int parse_od_spell_scroll(struct H3MLIB_CTX *ctx,
    struct H3M_OD_ENTRY *od_entry, struct META_OD_ENTRY *meta_od_entry)
{
    struct H3M *p = &ctx->h3m;
    struct H3MLIB_PARSING *parsing = &ctx->parsing;
    const uint32_t fm = p->format;
    struct H3M_OD_BODY_DYNAMIC_SPELL_SCROLL *body = NULL;
    size_t n = 0;
    int ret = 0;

    if (0 ==
        ((struct H3M_OD_BODY_STATIC_SPELL_SCROLL *)&parsing->raw[parsing->
                offset])->has_guardians) {
        meta_od_entry->binary_compatible = 1;
        meta_od_entry->body_size =
            sizeof(struct H3M_OD_BODY_STATIC_SPELL_SCROLL);
        return 0;
    }

    od_entry->body = calloc(1, sizeof(*body));
    body = (struct H3M_OD_BODY_DYNAMIC_SPELL_SCROLL *)od_entry->body;
    meta_od_entry->binary_compatible = 0;
    meta_od_entry->body_size = sizeof(*body);

    SAFE_READ_SIZEOF(&body->has_guardians, parsing)
        if (0 != body->has_guardians) {
        if (0 != (ret =
                parse_od_ext_guardians(ctx, &body->guardians, meta_od_entry))) {
            return ret;
        }
    }
    // Only part that differs from parse_od_artifact aside from type of body
    SAFE_READ_SIZEOF(&body->spell, parsing)

    return 0;
}
