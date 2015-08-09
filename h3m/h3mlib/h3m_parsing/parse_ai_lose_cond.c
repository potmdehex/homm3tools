// Created by John Åkerblom 2014-11-21

#include "../h3mlib.h"
#include "../h3mlib_ctx.h"
#include "../h3m_structures/h3m_ai_lose_cond.h"
#include "../format_select.h"
#include "../safe_read.h"

#include <stdlib.h>

static size_t _sizeof_lose_cond(uint8_t lose_cond_type)
{
    switch (lose_cond_type) {
    case C_LOSE_TOWN:
    case C_LOSE_HERO:
        return sizeof(((union H3M_AI_LOSE_COND *)NULL)->c_position);
    case C_TIME:
        return sizeof(((union H3M_AI_LOSE_COND *)NULL)->c_time);
    default:
        break;
    }

    return -1;
}

int parse_ai_lose_cond(struct H3MLIB_CTX *ctx)
{
    struct H3M *p = &ctx->h3m;
    struct H3MLIB_META *meta = &ctx->meta;
    struct H3MLIB_PARSING *parsing = &ctx->parsing;
    //int orig_off = ctx->parsing.offset;
    //int ret = 0;
    const uint32_t fm = p->format;
    uint8_t c = 0;
    uint32_t n = 0;

    SAFE_READ_SIZEOF(&c, parsing);
    FS_ANY_ASSIGN(p->ai, lose_cond_type, c, fm);

    if (0xFF != c) {
        SAFE_CHECK_N(c, C_TIME) // C_TIME (0x02) is the highest loss condition
        n = _sizeof_lose_cond(c);
        //SAFE_READ_N(&p->ai.any.lose_cond, parsing, p->ai.any.lose_cond_size)
        FS_ANY_READ_N(p->ai, lose_cond, n, parsing, fm)

        meta->ai_lose_cond_size = n;
    }

    return 0;
}
