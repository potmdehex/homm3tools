// Created by John Åkerblom 2014-11-18Bla bla standard lib so unsafe

#include "h3mlib.h"
#include "h3mlib_cleanup.h"
#include "h3mlib_ctx.h"
#include "h3m_structures/h3m.h"
#include "format_select.h"

#include <uthash.h>
#include <stdlib.h>
#include <string.h>

#ifdef _MSC_VER
#pragma warning(disable:4996)   // M$ standard lib unsafe
#endif

#define SAFE_FREE(P) if(NULL != P) free(P)

static void _free_oa_hash(struct META_OA_HASH_ENTRY *oa_hash)
{
    struct META_OA_HASH_ENTRY *cur, *tmp;

    HASH_ITER(hh, oa_hash, cur, tmp) {
        HASH_DEL(oa_hash, cur);
        free(cur->def);
        free(cur);
        cur = NULL;
    }
}

static int _free_ai_sod(struct H3M *p)
{
    unsigned int i = 0;

    for (i = 0; i < p->ai.sod.custom_heroes_count; ++i) {
        SAFE_FREE(p->ai.sod.custom_heroes[i].name);
    }
    SAFE_FREE(p->ai.sod.custom_heroes);

    for (i = 0; i < 156; ++i) {
        if (0 == p->ai.sod.hero_settings[i].has_settings) {
            continue;
        }

        SAFE_FREE(p->ai.sod.hero_settings[i].biography.desc);
        SAFE_FREE(p->ai.sod.hero_settings[i].artifacts.backpack_entries);
        SAFE_FREE(p->ai.sod.hero_settings[i].secondary_skills.entries);
    }

    return 0;
}

void free_dyn_pointers(UT_array * dyn_pointers, int free_members)
{
    struct META_DYN_POINTER **dyn_pointer = NULL;
    struct META_DYN_ARRAY_ENTRY **entry = NULL;

    for (dyn_pointer = (struct META_DYN_POINTER **)utarray_front(dyn_pointers);
        dyn_pointer != NULL;
        dyn_pointer =
        (struct META_DYN_POINTER **)utarray_next(dyn_pointers, dyn_pointer)) {
        if (0 != free_members && -1 != (*dyn_pointer)->dyn_size) {
            SAFE_FREE((*dyn_pointer)->p);
        }
        if (NULL != (*dyn_pointer)->p_array) {
            for (entry =
                (struct META_DYN_ARRAY_ENTRY **)utarray_front((*dyn_pointer)->
                    p_array); entry != NULL;
                entry =
                (struct META_DYN_ARRAY_ENTRY **)utarray_next((*dyn_pointer)->
                    p_array, entry)) {
                free_dyn_pointers((*entry)->dyn_pointers, free_members);
                free(*entry);
            }
            utarray_free((*dyn_pointer)->p_array);
        }
        free(*dyn_pointer);
    }
    utarray_free(dyn_pointers);
}

void free_players(union H3M_PLAYER **players)
{
    for (int i = 0; i < H3M_MAX_PLAYERS; ++i) {
        SAFE_FREE(players[i]);
    }
    free(players);
}

int h3mlib_cleanup(struct H3MLIB_CTX **ctx)
{
    struct H3M *p = &(*ctx)->h3m;
    struct H3MLIB_META *meta = &(*ctx)->meta;
    struct H3M_CODE *hc = &(*ctx)->h3m_code;
    unsigned int i = 0;
    unsigned int j = 0;
    const uint32_t fm = p->format;

    SAFE_FREE(p->bi.any.name);
    SAFE_FREE(p->bi.any.desc);

    if (NULL != p->players) {
        free_players(p->players);
    }

    if (H3M_FORMAT_ROE != fm) {
        for (i = 0; i < H3M_MAX_PLAYERS; ++i) {
            if (0 != meta->player_has_ai[i] && NULL != p->player_ai[i].heroes) {
                for (j = 0; j < p->player_ai[i].heroes_count; ++j) {
                    SAFE_FREE(p->player_ai[i].heroes[j]);
                }
            }
            SAFE_FREE(p->player_ai[i].heroes);
            SAFE_FREE(meta->player_ai_heroes_sizes[i]);
        }
    }

    if (NULL != FS_ANY(p->ai, rumors, p->format)) {
        if (0 != (uint32_t) FS_ANY(p->ai, rumors_count, p->format)) {
            for (i = 0; i < (uint32_t) FS_ANY(p->ai, rumors_count, fm); ++i) {
                SAFE_FREE(FS_ANY(p->ai, rumors[i].name, fm));
                SAFE_FREE(FS_ANY(p->ai, rumors[i].desc, fm));
            }

            SAFE_FREE(FS_ANY(p->ai, rumors, fm));
        }
    }

    FS_SOD_CALL(_free_ai_sod(p), fm)

        SAFE_FREE(p->tiles);

    if (NULL != p->oa.entries) {
        for (i = 0; i < p->oa.count; ++i) {
            SAFE_FREE(p->oa.entries[i].header.def);
        }
    }

    if (NULL != p->od.entries) {
        for (i = 0; i < p->od.count; ++i) {
            if (NULL != meta->od_entries[i].dyn_pointers) {
                free_dyn_pointers(meta->od_entries[i].dyn_pointers, 1);
            }
            SAFE_FREE(p->od.entries[i].body);
        }
    }

    if (NULL != p->event.entries) {
        for (i = 0; i < p->event.count; ++i) {
            SAFE_FREE(p->event.entries[i].header.name);
            SAFE_FREE(p->event.entries[i].header.mesg);
        }
    }

    SAFE_FREE(p->oa.entries);
    SAFE_FREE(p->od.entries);
    SAFE_FREE(p->event.entries);

    SAFE_FREE(hc->dll);
    SAFE_FREE(hc->shellcode_oa);

    SAFE_FREE(meta->oa_entries);
    SAFE_FREE(meta->od_entries);

    _free_oa_hash((*ctx)->meta.oa_hash);

    SAFE_FREE(*ctx);

    *ctx = NULL;

    return 0;
}
