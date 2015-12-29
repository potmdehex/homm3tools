// Created by John Åkerblom 2014-11-21

#ifdef _WIN32
#include <windows.h>            // DEBUG
#else
#define _GNU_SOURCE             // strnlen
#endif

#include "../h3mlib.h"
#include "parse_players.h"
#include "../utils/safe_read.h"

#include <stdlib.h>
#include <string.h>

// TODO: Merge the three _parse_player_* functions into one

static int _parse_player_roe(int idx, struct H3MLIB_CTX *ctx)
{
    struct H3M *p = &ctx->h3m;
    struct H3MLIB_META *meta = &ctx->meta;
    struct H3MLIB_PARSING *parsing = &ctx->parsing;
    int orig_off = ctx->parsing.offset;
    int ret = 0;
    size_t size = 0;
    const char *member = NULL;
    struct H3M_PLAYER_ROE *roe = NULL;

    // TODO: Roll strnlen replacement for all language that may require it

    // Simply alloc enough space for all of the unions - no one cares roeout a few
    // bytes needlessly used up for the smaller unions
    roe = calloc(1, sizeof(*roe) + H3M_MAX_HERO_NAME + 1);      // + 1 for NULL terminator
    // TODO: safety check here
    SAFE_READ_N(roe, sizeof(*roe) + H3M_MAX_HERO_NAME, parsing);

    // H3M_PLAYER_EXT_ROE
    if (0 == roe->has_main_town && 0xFF == roe->u.e0.starting_hero_type) {
        size = sizeof(*roe) - sizeof(roe->u) + sizeof(roe->u.e0);
        member = "player (e0)";
        meta->player_ext_types[idx] = 0;
    }
    // H3M_PLAYER_EXT_WITH_TOWN_ROE
    else if (0 != roe->has_main_town && 0xFF == roe->u.e1.starting_hero_type) {
        size = sizeof(*roe) - sizeof(roe->u) + sizeof(roe->u.e1);
        member = "player (e1)";
        meta->player_ext_types[idx] = 1;
    }
    // H3M_PLAYER_EXT_WITH_HERO_ROE
    else if (0 == roe->has_main_town
        && (0 == roe->u.e2.starting_hero_name_size
            || (roe->u.e2.starting_hero_name_size <= H3M_MAX_HERO_NAME
                && roe->u.e2.starting_hero_name_size ==
                strnlen((char *)roe->u.e2.starting_hero_name,
                    H3M_MAX_HERO_NAME + 1)))) {
        size =
            sizeof(*roe) - sizeof(roe->u) + sizeof(roe->u.e2) +
            roe->u.e2.starting_hero_name_size;
        member = "player (e2)";
        meta->player_has_ai[idx] = 1;
        meta->player_ext_types[idx] = 2;
    }
    // H3M_PLAYER_EXT_WITH_TOWN_AND_HERO_ROE
    else if (0 != roe->has_main_town
        && (0 == roe->u.e3.starting_hero_name_size
            || (roe->u.e3.starting_hero_name_size <= H3M_MAX_HERO_NAME
                && roe->u.e3.starting_hero_name_size ==
                strnlen((char *)roe->u.e3.starting_hero_name,
                    H3M_MAX_HERO_NAME + 1)))) {
        size =
            sizeof(*roe) - sizeof(roe->u) + sizeof(roe->u.e3) +
            roe->u.e3.starting_hero_name_size;
        member = "player (e3)";
        meta->player_has_ai[idx] = 1;
        meta->player_ext_types[idx] = 3;
    } else {
#ifdef _WIN32
        OutputDebugStringA("unknown player");
#endif
        SAFE_CHECK_NOT_PASSED()
    }

    p->players[idx] = (union H3M_PLAYER *)roe;
    meta->player_sizes[idx] = size;
    if (NULL != ctx->callbacks.cb_parse) {
        ret =
            ctx->callbacks.cb_parse(orig_off, member, &parsing->raw[orig_off],
            size, ctx->callbacks.cb_data);
    }

    ctx->parsing.offset = orig_off + size;

    return ret;
}

static int _parse_player_ab(int idx, struct H3MLIB_CTX *ctx)
{
    struct H3M *p = &ctx->h3m;
    struct H3MLIB_META *meta = &ctx->meta;
    struct H3MLIB_PARSING *parsing = &ctx->parsing;
    int orig_off = ctx->parsing.offset;
    int ret = 0;
    size_t size = 0;
    const char *member = NULL;
    struct H3M_PLAYER_AB *ab = NULL;

    // TODO: Roll strnlen replacement for all language that may require it

    // Simply alloc enough space for all of the unions - no one cares about a few
    // bytes needlessly used up for the smaller unions
    ab = calloc(1, sizeof(*ab) + H3M_MAX_HERO_NAME + 1);        // + 1 for NULL terminator
    // TODO: safety check here
    SAFE_READ_N(ab, sizeof(*ab) + H3M_MAX_HERO_NAME, parsing);

    //// HACK FOR PLACEHOLDER HERO
    //if (0 == ab->has_main_town && 0xFF == ab->u.e0.starting_hero_type && 0 != ab->town_types)
    //{
    //    size = 10;
    //    member = "player (e4)";
    //    meta->player_has_ai[idx] = 1;
    //    meta->player_ext_types[idx] = 0;
    //}
    //// H3M_PLAYER_EXT_ABSOD
    //else 
    if (0 == ab->has_main_town && 0xFF == ab->u.e0.starting_hero_type) {
        size = sizeof(*ab) - sizeof(ab->u) + sizeof(ab->u.e0);
        member = "player (e0)";
        meta->player_ext_types[idx] = 0;
    }
    // H3M_PLAYER_EXT_WITH_TOWN_ABSOD
    else if (0 != ab->has_main_town && 0xFF == ab->u.e1.starting_hero_type) {
        size = sizeof(*ab) - sizeof(ab->u) + sizeof(ab->u.e1);
        member = "player (e1)";
        meta->player_ext_types[idx] = 1;
    }
    // H3M_PLAYER_EXT_WITH_HERO_ABSOD
    else if (0 == ab->has_main_town
        && (0 == ab->u.e2.starting_hero_name_size
            || (ab->u.e2.starting_hero_name_size <= H3M_MAX_HERO_NAME
                && ab->u.e2.starting_hero_name_size ==
                strnlen((char *)ab->u.e2.starting_hero_name,
                    H3M_MAX_HERO_NAME + 1)))) {
        size =
            sizeof(*ab) - sizeof(ab->u) + sizeof(ab->u.e2) +
            ab->u.e2.starting_hero_name_size;
        member = "player (e2)";
        meta->player_has_ai[idx] = 1;
        meta->player_ext_types[idx] = 2;
    }
    // H3M_PLAYER_EXT_WITH_TOWN_AND_HERO_ABSOD
    else if (0 != ab->has_main_town
        && (0 == ab->u.e3.starting_hero_name_size
            || (ab->u.e3.starting_hero_name_size <= H3M_MAX_HERO_NAME
                && ab->u.e3.starting_hero_name_size ==
                strnlen((char *)ab->u.e3.starting_hero_name,
                    H3M_MAX_HERO_NAME + 1)))) {
        size =
            sizeof(*ab) - sizeof(ab->u) + sizeof(ab->u.e3) +
            ab->u.e3.starting_hero_name_size;
        member = "player (e3)";
        meta->player_has_ai[idx] = 1;
        meta->player_ext_types[idx] = 3;
    } else {
#ifdef _WIN32
        OutputDebugStringA("unknown player");
#endif
        SAFE_CHECK_NOT_PASSED()
    }

    p->players[idx] = (union H3M_PLAYER *)ab;
    meta->player_sizes[idx] = size;
    if (NULL != ctx->callbacks.cb_parse) {
        ret =
            ctx->callbacks.cb_parse(orig_off, member, &parsing->raw[orig_off],
            size, ctx->callbacks.cb_data);
    }

    ctx->parsing.offset = orig_off + size;

    return ret;
}

static int _parse_player_sod(int idx, struct H3MLIB_CTX *ctx)
{
    struct H3M *p = &ctx->h3m;
    struct H3MLIB_META *meta = &ctx->meta;
    struct H3MLIB_PARSING *parsing = &ctx->parsing;
    int orig_off = ctx->parsing.offset;
    int ret = 0;
    size_t size = 0;
    const char *member = NULL;
    struct H3M_PLAYER_SOD *sod = NULL;

    // TODO: Roll strnlen replacement for all language that may require it

    // Simply alloc enough space for all of the unions - no one cares sodout a few
    // bytes needlessly used up for the smaller unions
    sod = calloc(1, sizeof(*sod) + H3M_MAX_HERO_NAME + 1);      // + 1 for NULL terminator
    // TODO: safety check here
    SAFE_READ_N(sod, sizeof(*sod) + H3M_MAX_HERO_NAME, parsing);

    // HACK FOR PLACEHOLDER HERO
    if (0 == sod->has_main_town && 0xFF == sod->u.e0.starting_hero_type
        && 0 != sod->town_types) {
        size = 10;
        member = "player (e4)";
        meta->player_has_ai[idx] = 1;
        meta->player_ext_types[idx] = 0;
    }
    // H3M_PLAYER_EXT_ABSOD
    else if (0 == sod->has_main_town && 0xFF == sod->u.e0.starting_hero_type) {
        size = sizeof(*sod) - sizeof(sod->u) + sizeof(sod->u.e0);
        member = "player (e0)";
        meta->player_ext_types[idx] = 0;
    }
    // H3M_PLAYER_EXT_WITH_TOWN_ABSOD
    else if (0 != sod->has_main_town && 0xFF == sod->u.e1.starting_hero_type) {
        size = sizeof(*sod) - sizeof(sod->u) + sizeof(sod->u.e1);
        member = "player (e1)";
        meta->player_ext_types[idx] = 1;
    }
    // H3M_PLAYER_EXT_WITH_HERO_ABSOD
    else if (0 == sod->has_main_town
        && (0 == sod->u.e2.starting_hero_name_size
            || (sod->u.e2.starting_hero_name_size <= H3M_MAX_HERO_NAME
                && sod->u.e2.starting_hero_name_size ==
                strnlen((char *)sod->u.e2.starting_hero_name,
                    H3M_MAX_HERO_NAME + 1)))) {
        size =
            sizeof(*sod) - sizeof(sod->u) + sizeof(sod->u.e2) +
            sod->u.e2.starting_hero_name_size;
        member = "player (e2)";
        meta->player_has_ai[idx] = 1;
        meta->player_ext_types[idx] = 2;
    }
    // H3M_PLAYER_EXT_WITH_TOWN_AND_HERO_ABSOD
    else if (0 != sod->has_main_town
        && (0 == sod->u.e3.starting_hero_name_size
            || (sod->u.e3.starting_hero_name_size <= H3M_MAX_HERO_NAME
                && sod->u.e3.starting_hero_name_size ==
                strnlen((char *)sod->u.e3.starting_hero_name,
                    H3M_MAX_HERO_NAME + 1)))) {
        size =
            sizeof(*sod) - sizeof(sod->u) + sizeof(sod->u.e3) +
            sod->u.e3.starting_hero_name_size;
        member = "player (e3)";
        meta->player_has_ai[idx] = 1;
        meta->player_ext_types[idx] = 3;
    } else {
#ifdef _WIN32
        OutputDebugStringA("unknown player");
#endif
        SAFE_CHECK_NOT_PASSED()
    }

    p->players[idx] = (union H3M_PLAYER *)sod;
    meta->player_sizes[idx] = size;
    if (NULL != ctx->callbacks.cb_parse) {
        ret =
            ctx->callbacks.cb_parse(orig_off, member, &parsing->raw[orig_off],
            size, ctx->callbacks.cb_data);
    }

    ctx->parsing.offset = orig_off + size;

    return ret;
}

int parse_players_roe(struct H3MLIB_CTX *ctx)
{
    struct H3M *p = &ctx->h3m;
    struct H3MLIB_META *meta = &ctx->meta;
    int i = 0;
    int ret = 0;

    p->players = calloc(H3M_MAX_PLAYERS, sizeof(void *));
    for (i = 0; i < H3M_MAX_PLAYERS; ++i) {
        if (0 != (ret = _parse_player_roe(i, ctx))) {
            return ret;
        }
    }

    memset(meta->player_has_ai, 0, sizeof(meta->player_has_ai));

    return 0;
}

int parse_players_ab(struct H3MLIB_CTX *ctx)
{
    struct H3M *p = &ctx->h3m;
    struct H3MLIB_META *meta = &ctx->meta;
    int i = 0;
    int ret = 0;

    p->players = calloc(H3M_MAX_PLAYERS, sizeof(void *));
    for (i = 0; i < H3M_MAX_PLAYERS; ++i) {
        if (0 != (ret = _parse_player_ab(i, ctx))) {
            return ret;
        }

        if (0 == meta->player_has_ai[i]) {
            continue;
        }

        if (0 != (ret = parse_player_ai_absod(i, ctx))) {
            return ret;
        }
    }

    return 0;
}

int parse_players_sod(struct H3MLIB_CTX *ctx)
{
    struct H3M *p = &ctx->h3m;
    struct H3MLIB_META *meta = &ctx->meta;
    int i = 0;
    int ret = 0;

    p->players = calloc(H3M_MAX_PLAYERS, sizeof(void *));
    for (i = 0; i < H3M_MAX_PLAYERS; ++i) {
        if (0 != (ret = _parse_player_sod(i, ctx))) {
            return ret;
        }

        if (0 == meta->player_has_ai[i]) {
            continue;
        }

        if (0 != (ret = parse_player_ai_absod(i, ctx))) {
            return ret;
        }
    }

    return 0;
}
