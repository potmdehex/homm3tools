// Created by John Åkerblom 2014-11-21

#include "../h3mlib.h"
#include "../h3mlib_ctx.h"
#include "../h3m_structures/additional_info/h3m_ai_win_cond.h"
#include "../format_select.h"
#include "../safe_read.h"

#include <stdlib.h>

static size_t _sizeof_win_cond(uint8_t win_cond_type, uint32_t fm)
{
    switch (win_cond_type) {
    case C_ACQUIRE_ARTIFACT:
        return (H3M_FORMAT_ROE == fm) ?
            sizeof(((union H3M_AI_WIN_COND *)NULL)->c_acquire.roe)
            : sizeof(((union H3M_AI_WIN_COND *)NULL)->c_acquire.absod);
    case C_ACCUMULATE_CREATURES:
        return (H3M_FORMAT_ROE == fm) ?
            sizeof(((union H3M_AI_WIN_COND *)NULL)->c_accumulate_creatures.roe)
            : sizeof(((union H3M_AI_WIN_COND *)NULL)->c_accumulate_creatures.
            absod);
    case C_ACCUMULATE_RESOURCES:
        return sizeof(((union H3M_AI_WIN_COND *)NULL)->c_accumulate_resources);
    case C_FLAG_DWELLINGS:
    case C_FLAG_MINES:
        return sizeof(((union H3M_AI_WIN_COND *)NULL)->c_flag);
    case C_UPGRADE_TOWN:
        return sizeof(((union H3M_AI_WIN_COND *)NULL)->c_upgrade);
    case C_TRANSPORT_ARTIFACT:
        return sizeof(((union H3M_AI_WIN_COND *)NULL)->c_transport);
    case C_BUILD_GRAIL:
    case C_DEFEAT_HERO:
    case C_CAPTURE_TOWN:
    case C_DEFEAT_MONSTER:
        return sizeof(((union H3M_AI_WIN_COND *)NULL)->c_position);
    default:
        break;
    }

    return -1;
}

int parse_ai_win_cond(struct H3MLIB_CTX *ctx)
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
    FS_ANY_ASSIGN(p->ai, win_cond_type, c, fm);

    if (0xFF != c) {
        SAFE_CHECK_N(c, C_TRANSPORT_ARTIFACT)   // C_TRANSPORT_ARTIFACT(0x02) is the highest win condition
        n = _sizeof_win_cond(c, fm);
        //SAFE_READ_N(&p->ai.any.win_cond, parsing, p->ai.any.win_cond_size)
        FS_ANY_READ_N(p->ai, win_cond, n, parsing, fm)

        meta->ai_win_cond_size = n;
    }

    return 0;
}
