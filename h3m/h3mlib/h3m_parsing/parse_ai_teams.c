// Created by John Åkerblom 2014-11-21

#include "../h3mlib.h"
#include "parse_ai.h"
#include "../utils/format_select.h"
#include "../utils/safe_read.h"

#include <stdlib.h>
#include <string.h>

int parse_ai_teams(struct H3MLIB_CTX *ctx)
{
    struct H3M *p = &ctx->h3m;
    struct H3MLIB_META *meta = &ctx->meta;
    struct H3MLIB_PARSING *parsing = &ctx->parsing;
    //int orig_off = ctx->parsing.offset;
    //int ret = 0;
    const uint32_t fm = p->format;
    uint8_t count = 0;

    SAFE_READ_SIZEOF(&count, parsing)
    if (0 != count) {
        FS_ANY_ASSIGN(p->ai, teams_count, count, fm)
        FS_ANY_READ_SIZEOF(p->ai, teams, parsing, fm)

        meta->ai_teams_size = sizeof(p->ai.any.teams);
    }

    return 0;
}
