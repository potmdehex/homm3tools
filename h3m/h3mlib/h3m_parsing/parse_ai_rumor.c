// Created by John Åkerblom 2014-11-21

#include "../h3mlib.h"
#include "parse_ai.h"
#include "../format_select.h"
#include "../safe_read.h"

#include <stdlib.h>
#include <string.h>

int parse_ai_rumor(struct H3MLIB_CTX *ctx)
{
    struct H3M *p = &ctx->h3m;
    //struct H3MLIB_META*meta = &ctx->meta;
    struct H3MLIB_PARSING *parsing = &ctx->parsing;
    //int orig_off = ctx->parsing.offset;
    const uint32_t fm = p->format;
    //int ret = 0;
    uint32_t count = 0;
    unsigned int i = 0;

    SAFE_READ_SIZEOF(&count, parsing)
    if (0 != count) {
        struct H3M_AI_RUMOR *rumors = NULL;
        SAFE_CHECK_N(count, 64)

        rumors = calloc(count, sizeof(*rumors));

        for (i = 0; i < count; ++i) {
            SAFE_READ_SIZEOF(&rumors[i].name_size, parsing)
            SAFE_ALLOC_N(rumors[i].name, rumors[i].name_size + 1, 256)
            SAFE_READ_N(rumors[i].name, rumors[i].name_size, parsing)

            SAFE_READ_SIZEOF(&rumors[i].desc_size, parsing)
            SAFE_ALLOC_N(rumors[i].desc, rumors[i].desc_size + 1, 4096)
            SAFE_READ_N(rumors[i].desc, rumors[i].desc_size, parsing)
        }

        FS_ANY_ASSIGN(p->ai, rumors_count, count, fm)
        FS_ANY_ASSIGN(p->ai, rumors, rumors, fm)
    }

    return 0;
}
