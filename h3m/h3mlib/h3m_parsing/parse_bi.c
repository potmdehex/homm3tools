// Created by John Åkerblom 2014-11-21

#include "../h3mlib.h"
#include "parse_bi.h"
#include "../utils/format_select.h"
#include "../utils/safe_read.h"

#include <stdlib.h>
#include <string.h>

int parse_bi(struct H3MLIB_CTX *ctx)
{
    struct H3M *p = &ctx->h3m;
    //struct H3MLIB_META*meta = &ctx->meta;
    struct H3MLIB_PARSING *parsing = &ctx->parsing;
    int orig_off = ctx->parsing.offset;
    int ret = 0;
    size_t n = 0;
    struct H3M_BI_ANY *any = &p->bi.any;
    struct H3M_BI_HOTA *hota = &p->bi.hota;
    uint32_t fm = ctx->h3m.format;

    n = sizeof(any->has_hero) + sizeof(any->map_size)
        + sizeof(any->has_two_levels);
    if (fm != H3M_FORMAT_HOTA) {
        // BI is the same across RoE->SoD versions except for the last uint8_t 
        // so the .any union can simply be used instead of the FS_-macros
        SAFE_READ_N(any, n, parsing);
        SAFE_READ_STRING(any->name_size, 32, any->name, parsing)
        SAFE_READ_STRING(any->desc_size, 4096, any->desc, parsing)
        SAFE_READ_SIZEOF(&any->difficulty, parsing);

        FS_ABSOD_READ_SIZEOF(p->bi, mastery_cap, parsing, p->format)
    } else { // HotA
        n += sizeof(p->bi.hota.internal_format);
        SAFE_READ_N(hota, n, parsing);
        SAFE_READ_STRING(hota->name_size, 32, hota->name, parsing)
        SAFE_READ_STRING(hota->desc_size, 4096, hota->desc, parsing)
        SAFE_READ_SIZEOF(&hota->difficulty, parsing);

        SAFE_READ_SIZEOF(&hota->mastery_cap, parsing);
    }

    if (NULL != ctx->callbacks.cb_parse) {
        n = parsing->offset - orig_off;
        ret = ctx->callbacks.cb_parse(orig_off, "bi", &parsing->raw[orig_off], 
            n, ctx->callbacks.cb_data);
    }

    return ret;
}
