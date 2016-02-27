// Created by John Åkerblom 2014-11-21

#include "../h3mlib.h"
#include "parse_ai.h"
#include "../utils/format_select.h"
#include "../utils/safe_read.h"

#include <stdlib.h>
#include <string.h>

int parse_ai(struct H3MLIB_CTX *ctx)
{
    struct H3M *p = &ctx->h3m;
    //struct H3MLIB_META*meta = &ctx->meta;
    struct H3MLIB_PARSING *parsing = &ctx->parsing;
    int orig_off = ctx->parsing.offset;
    int ret = 0;
    static const uint8_t nbuf[sizeof(((struct H3M_AI_ANY *)NULL)->reserved)] =
        { 0 };
    const uint32_t fm = p->format;
    size_t n = 0;

    FS_ANY_CALL(parse_ai_win_cond(ctx), fm)
    FS_ANY_CALL(parse_ai_lose_cond(ctx), fm)
    FS_ANY_CALL(parse_ai_teams(ctx), fm)

    if (fm != H3M_FORMAT_HOTA) {
        FS_ANY_READ_SIZEOF(p->ai, available_heroes, parsing, fm)
    } else { // HotA
        SAFE_READ_SIZEOF(&p->ai.hota.available_heroes_count, parsing)
        n = (p->ai.hota.available_heroes_count + 7) / 8;
        SAFE_ALLOC_N(p->ai.hota.available_heroes, n, 256)
        SAFE_READ_N(p->ai.hota.available_heroes, n, parsing)
    }

    FS_ABSOD_READ_SIZEOF(p->ai, empty, parsing, fm)
    // This variable previously thought to be empty is actually count for 
    // amount of placeholder heroes on map (only ones not power rating based).
    // TODO rename empty variable and actually parse/preserve the data
    if (fm > H3M_FORMAT_ROE) {
        SAFE_CHECK_N(FS_ABSOD(p->ai, empty, fm), 255)
        parsing->offset += FS_ABSOD(p->ai, empty, fm);
    }

    FS_SOD_CALL(parse_ai_custom_heroes(ctx), fm)
    // Should always have 31 0x00 bytes here
    if (0 != memcmp(&parsing->raw[parsing->offset], nbuf, sizeof(nbuf))) {
        return 1;
    }

    FS_ANY_READ_SIZEOF(p->ai, reserved, parsing, fm)
    if (H3M_FORMAT_HOTA != fm) {
        FS_ABSOD_READ_SIZEOF(p->ai, available_artifacts, parsing, fm)
    } else { // HotA
        SAFE_READ_SIZEOF(&p->ai.hota.allow_special_weeks, parsing) // HotA feature

        SAFE_READ_SIZEOF(&p->ai.hota.available_artifacts_count, parsing);
        n = (p->ai.hota.available_artifacts_count + 7) / 8;
        SAFE_ALLOC_N(p->ai.hota.available_artifacts, n, 1024)
        SAFE_READ_N(p->ai.hota.available_artifacts, n, parsing)
    }
    FS_SOD_READ_SIZEOF(p->ai, available_spells, parsing, fm)
    FS_SOD_READ_SIZEOF(p->ai, available_skills, parsing, fm)

    FS_ANY_CALL(parse_ai_rumor(ctx), fm)

    FS_SOD_CALL(parse_ai_hero_settings(ctx), fm)

    if (NULL != ctx->callbacks.cb_parse) {
        n = parsing->offset - orig_off;
        ret = ctx->callbacks.cb_parse(orig_off, "ai", &parsing->raw[orig_off], 
            n, ctx->callbacks.cb_data);
    }

    return ret;
}
