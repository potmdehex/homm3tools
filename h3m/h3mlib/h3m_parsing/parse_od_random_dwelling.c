// Created by John Åkerblom 2014-11-22

#include "parse_od.h"
#include "parsing_common.h"

int parse_od_random_dwelling(struct H3MLIB_CTX *ctx,
    struct H3M_OD_ENTRY *od_entry, struct META_OD_ENTRY *meta_od_entry)
{
    struct H3M *p = &ctx->h3m;
    struct H3MLIB_PARSING *parsing = &ctx->parsing;
    //struct H3MLIB_META *meta = &ctx->meta;
    const uint32_t fm = p->format;
    struct H3M_OD_BODY_DYNAMIC_RANDOM_DWELLING *body = NULL;

    od_entry->body = calloc(1, sizeof(*body));
    body = (struct H3M_OD_BODY_DYNAMIC_RANDOM_DWELLING *)od_entry->body;
    meta_od_entry->binary_compatible = 0;
    meta_od_entry->body_size = sizeof(*body);

    SAFE_READ_SIZEOF(&body->owner, parsing)
    if (H3M_OBJECT_RANDOM_DWELLING_PRESET_ALIGNMENT_ABSOD !=
        meta_od_entry->oa_type) {
        SAFE_READ_SIZEOF(&body->castle_absod_id, parsing)
        if (0 == body->castle_absod_id) {
            SAFE_READ_SIZEOF_BUF(body->alignment, parsing)
        }
    }

    if (H3M_OBJECT_RANDOM_DWELLING_PRESET_LEVEL_ABSOD != meta_od_entry->oa_type) {
        SAFE_READ_SIZEOF(&body->min_level, parsing)
        SAFE_READ_SIZEOF(&body->max_level, parsing)
    }

    return 0;
}
