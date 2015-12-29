// Created by John Åkerblom 2014-11-21

#include "../h3mlib.h"
#include "../internal/h3mlib_ctx.h"

#include "parse_ai.h"
#include "parse_bi.h"
#include "parse_oa.h"
#include "parse_od.h"
#include "parse_event.h"
#include "parse_players.h"
#include "parse_tile.h"

#include <stdlib.h>
#include <string.h>

int parse_h3m(struct H3MLIB_CTX *ctx)
{
    struct H3M *p = &ctx->h3m;
    int ret = 0;

    // First 4 bytes of raw data is the format specifier, so simply dereference
    // as uint32_t pointer.
    p->format = *(uint32_t *)ctx->parsing.raw;
    ctx->parsing.offset += sizeof(uint32_t);
    if (NULL != ctx->callbacks.cb_parse 
            && 0 != (ret = ctx->callbacks.cb_parse(0, "format", p,
            sizeof(uint32_t), ctx->callbacks.cb_data))) {
        return ret;
    }

    if (0 != (ret = parse_bi(ctx))) {
        return ret;
    }

    switch (p->format) {
    case H3M_FORMAT_ROE:
        parse_players_roe(ctx);
        break;
    case H3M_FORMAT_AB:
        parse_players_ab(ctx);
        break;
    case H3M_FORMAT_SOD:
        // fall through
    default:
        parse_players_sod(ctx);
        break;
    }

    if (0 != (ret = parse_ai(ctx))) {
        return ret;
    }

    if (0 != (ret = parse_tile(ctx))) {
        return ret;
    }

    if (0 != (ret = parse_oa(ctx))) {
        return ret;
    }

    if (0 != (ret = parse_od(ctx))) {
        return ret;
    }

    if (0 != (ret = parse_event(ctx))) {
        return ret;
    }

    return 0;
}
