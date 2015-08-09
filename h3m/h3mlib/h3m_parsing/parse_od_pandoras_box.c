// Created by John Åkerblom 2014-11-22

#include "parse_od.h"
#include "parsing_common.h"

int parse_od_pandoras_box(struct H3MLIB_CTX *ctx,
    struct H3M_OD_ENTRY *od_entry, struct META_OD_ENTRY *meta_od_entry)
{
    struct H3M *p = &ctx->h3m;
    struct H3MLIB_PARSING *parsing = &ctx->parsing;
    //struct H3MLIB_META *meta = &ctx->meta;
    const uint32_t fm = p->format;
    struct H3M_OD_BODY_DYNAMIC_PANDORAS_BOX *body = NULL;
    size_t n = 0;
    int ret = 0;

    od_entry->body = calloc(1, sizeof(*body));
    body = (struct H3M_OD_BODY_DYNAMIC_PANDORAS_BOX *)od_entry->body;
    meta_od_entry->binary_compatible = 0;
    meta_od_entry->body_size = sizeof(*body);

    SAFE_READ_SIZEOF(&body->has_guardians, parsing)
    if (0 != body->has_guardians) {
        if (0 != (ret =
                parse_od_ext_guardians(ctx, &body->guardians, meta_od_entry))) {
            return ret;
        }
    }

    if (0 != (ret = parse_od_ext_contents(ctx, &body->contents, meta_od_entry))) {
        return ret;
    }

    return 0;
}
