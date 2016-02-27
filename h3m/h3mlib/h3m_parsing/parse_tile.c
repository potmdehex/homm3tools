// Created by John Åkerblom 2014-11-22

#include "../h3mlib.h"
#include "parse_tile.h"
#include "../utils/safe_read.h"

#include <stdlib.h>
#include <string.h>

int parse_tile(struct H3MLIB_CTX *ctx)
{
    struct H3M *p = &ctx->h3m;
    uint32_t fm = p->format;
    struct H3MLIB_META *meta = &ctx->meta;
    struct H3MLIB_PARSING *parsing = &ctx->parsing;
    int orig_off = ctx->parsing.offset;
    int ret = 0;
    size_t n = 0;
    size_t map_size = (H3M_FORMAT_HOTA != fm)? p->bi.any.map_size 
        : p->bi.hota.map_size;
    int has_two_levels = (H3M_FORMAT_HOTA != fm) ? p->bi.any.has_two_levels 
        : p->bi.hota.has_two_levels;

    n = sizeof(struct H3M_TILE) * (map_size * map_size) *
        (has_two_levels? 2 : 1);
    //SAFE_CHECK_N(n, sizeof(struct H3M_TILE) * 144 * 144 * 2)
    p->tiles = calloc(1, n);
    SAFE_READ_N(p->tiles, n, parsing)

    if (NULL != ctx->callbacks.cb_parse) {
        n = parsing->offset - orig_off;
        ret = ctx->callbacks.cb_parse(orig_off, "tiles", 
            &parsing->raw[orig_off], n, ctx->callbacks.cb_data);
    }

    return ret;
}
