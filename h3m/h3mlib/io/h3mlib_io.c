// Created by John Åkerblom 2014-11-18

#include "../utils/format_select.h"
#include "../h3mlib.h"
#include "../io/h3mlib_io.h"
#include "../h3m_modembed/h3m_modembed.h"
#include "../h3m_parsing/parse_h3m.h"
#include "../h3m_structures/h3m.h"

#ifndef NO_ZLIB
#include <gzip_utils.h>
#else
#include "gzip_empty/gzip_utils.h"
#endif

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#ifdef _MSC_VER
#pragma warning(disable:4996)   // M$ standard lib unsafe
#endif

int h3mlib_io_read_mem(struct H3MLIB_CTX **ctx,
    uint8_t *raw,
    uint32_t raw_size,
    h3m_parse_cb_t cb_parse,
    h3m_error_cb_t cb_error, h3m_custom_def_cb_t cb_def, void *cb_data)
{
    struct H3MLIB_CTX *c = NULL;
    int ret = 0;

    c = calloc(1, sizeof(*c));
    c->parsing.raw = raw;
    c->parsing.raw_size = raw_size;
    c->callbacks.cb_parse = cb_parse;
    c->callbacks.cb_error = cb_error;
    c->callbacks.cb_def = cb_def;
    c->callbacks.cb_data = cb_data;
    *ctx = c;

    ret = parse_h3m(c);
    if (0 != ret && H3MLIB_INTERRUPT_CB_NO_CLEANUP != ret) {
        h3m_exit(&c);
        c = NULL;
        *ctx = NULL;
    }

    gu_free(raw);

    return ret;
}

int h3mlib_io_read(struct H3MLIB_CTX **ctx,
    const char *filename,
    h3m_parse_cb_t cb_parse,
    h3m_error_cb_t cb_error, h3m_custom_def_cb_t cb_def, void *cb_data)
{
    uint8_t *raw = NULL;
    uint32_t raw_size = 0;
    int ret = 0;

    if (0 != (ret = gu_decompress_file_to_mem(filename, (void *)&raw,
                (long *)&raw_size))) {
        return ret;
    }

    return h3mlib_io_read_mem(ctx, raw, raw_size, cb_parse, cb_error, cb_def,
        cb_data);
}

// TODO: in order to support any mods, something needs to be done about the .sod
// references, such as using the FS_ macros instead
static int _write_custom_heroes(const struct H3M *p, FILE * f)
{
    const struct H3M_AI_CUSTOM_HERO_SOD *hero = NULL;
    unsigned int i = 0;
    //size_t n = 0;

    for (i = 0; i < p->ai.sod.custom_heroes_count; ++i) {
        hero = &p->ai.sod.custom_heroes[i];
        fwrite(&hero->type, sizeof(hero->type), 1, f);
        fwrite(&hero->face, sizeof(hero->face), 1, f);
        fwrite(&hero->name_size, sizeof(hero->name_size), 1, f);
        fwrite(hero->name, hero->name_size, 1, f);
        fwrite(&hero->allowed_players, sizeof(hero->allowed_players), 1, f);
    }

    return 0;
}

static int _write_rumors(const struct H3M *p, FILE * f)
{
    const uint32_t fm = p->format;
    const struct H3M_AI_RUMOR *rumor = NULL;
    uint32_t rumors_count = (uint32_t) FS_ANY(p->ai, rumors_count, fm);
    unsigned int i = 0;
    //size_t n = 0;

    fwrite(&rumors_count, sizeof(rumors_count), 1, f);
    for (i = 0; i < rumors_count; ++i) {
        rumor = FS_ANY(&p->ai, rumors[i], fm);
        fwrite(&rumor->name_size, sizeof(rumor->name_size), 1, f);
        fwrite(rumor->name, rumor->name_size, 1, f);
        fwrite(&rumor->desc_size, sizeof(rumor->desc_size), 1, f);
        fwrite(rumor->desc, rumor->desc_size, 1, f);
    }

    return 0;
}

// TODO: in order to support any mods, something needs to be done about the .sod
// references, such as using the FS_ macros instead
static int _write_hero_settings(const struct H3M *p, FILE * f)
{
    const struct H3M_AI_HERO_SETTINGS *settings = NULL;
    const struct H3M_COMMON_SECONDARY_SKILL *skill_entry = NULL;
    const struct H3M_COMMON_BACKPACK_ENTRY *bp_entry = NULL;
    unsigned int i = 0;
    unsigned int j = 0;
    size_t n = 0;

    for (i = 0; i < 156; ++i) {
        settings = &p->ai.sod.hero_settings[i];
        fwrite(&settings->has_settings, sizeof(uint8_t), 1, f);
        if (0 == settings->has_settings) {
            continue;
        }

        fwrite(&settings->has_experience, sizeof(uint8_t), 1, f);
        if (0 != settings->has_experience) {
            fwrite(&settings->experience, sizeof(settings->experience), 1, f);
        }

        fwrite(&settings->has_secondary_skills, sizeof(uint8_t), 1, f);
        if (0 != settings->has_secondary_skills) {
            fwrite(&settings->secondary_skills.count, sizeof(uint32_t), 1, f);
            for (j = 0; j < settings->secondary_skills.count; ++j) {
                skill_entry = &settings->secondary_skills.entries[j];
                fwrite(skill_entry, sizeof(*skill_entry), 1, f);
            }
        }

        fwrite(&settings->has_artifacts, sizeof(uint8_t), 1, f);
        if (0 != settings->has_artifacts) {
            // backpack_entries is dynamic member of the struct
            n = sizeof(settings->artifacts)
                - sizeof(settings->artifacts.backpack_entries);
            fwrite(&settings->artifacts, n, 1, f);
            for (j = 0; j < settings->artifacts.backpack_count; ++j) {
                bp_entry = &settings->artifacts.backpack_entries[j];
                fwrite(bp_entry, sizeof(*bp_entry), 1, f);
            }
        }

        fwrite(&settings->has_biography, sizeof(uint8_t), 1, f);
        if (0 != settings->has_biography) {
            fwrite(&settings->biography.desc_size, sizeof(uint32_t), 1, f);
            fwrite(settings->biography.desc, settings->biography.desc_size, 1,
                f);
        }

        fwrite(&settings->gender, sizeof(uint8_t), 1, f);

        fwrite(&settings->has_spells, sizeof(uint8_t), 1, f);
        if (0 != settings->has_spells) {
            fwrite(settings->spells, sizeof(settings->has_spells), 1, f);
        }

        fwrite(&settings->has_primary_skills, sizeof(uint8_t), 1, f);
        if (0 != settings->has_primary_skills) {
            fwrite(&settings->primary_skills, sizeof(settings->primary_skills),
                1, f);
        }
    }

    return 0;
}

static int _write_dynamic_od(uint8_t *body,
    size_t body_size, UT_array * dyn_pointers, FILE * f)
{
    unsigned int cur_offset = 0;
    struct META_DYN_POINTER **dyn_pointer = NULL;
    struct META_DYN_ARRAY_ENTRY **entry = NULL;
    size_t n = 0;

    if (NULL != dyn_pointers) {
        // Iterate through any pointers/skips in the body. For each pointer/skip, write the data in
        // the body up to that pointer/skip (excluding anything already written),
        // then if it is a pointer, write the data that the pointer leads to.
        for (dyn_pointer =
            (struct META_DYN_POINTER **)utarray_front(dyn_pointers);
            dyn_pointer != NULL;
            dyn_pointer =
            (struct META_DYN_POINTER **)utarray_next(dyn_pointers,
                dyn_pointer)) {
            // Write data up to this pointer/skip
            n = (*dyn_pointer)->member_offset - cur_offset;
            fwrite(body + cur_offset, n, 1, f);
            cur_offset += n;

            // Check if we have -1 as size, that means this is merely a
            // skip and there is nothing to write and no pointer to skip past
            if (-1 == (*dyn_pointer)->dyn_size) {
                cur_offset += (*dyn_pointer)->skip;
                continue;
            }
            // Write the contents of this/these pointer(s), if not NULL
            if (NULL != (*dyn_pointer)->p_array)        // Array of pointers
            {
                for (entry = (struct META_DYN_ARRAY_ENTRY **)
                    utarray_front((*dyn_pointer)->p_array); entry != NULL;
                    entry =
                    (struct META_DYN_ARRAY_ENTRY **)
                    utarray_next((*dyn_pointer)->p_array, entry)) {
                    _write_dynamic_od((*entry)->body, (*entry)->body_size,
                        (*entry)->dyn_pointers, f);
                }
            } else if (NULL != (*dyn_pointer)->p)       // Single pointer
            {
                fwrite((*dyn_pointer)->p, (*dyn_pointer)->dyn_size, 1, f);
            }
            // Skip past pointer
            cur_offset += sizeof(void *);

            // Skip any extra bytes marked for skip if pointer was not NULL
            if (NULL != (*dyn_pointer)->p) {
                cur_offset += (*dyn_pointer)->skip;
            }
        }
    }
    // Write the rest of the body after the last pointer
    if (cur_offset < body_size) {
        fwrite(body + cur_offset, body_size - cur_offset, 1, f);
    }

    return 0;
}

void _write_events(const struct H3MLIB_CTX *ctx, FILE * f)
{
    struct H3M_EVENT_ENTRY *entry = NULL;
    const uint32_t fm = ctx->h3m.format;
    size_t body_size = (fm < H3M_FORMAT_SOD) ?
        sizeof(struct H3M_EVENT_BODY_ROEAB) : sizeof(struct H3M_EVENT_BODY_SOD);
    fwrite(&ctx->h3m.event.count, sizeof(ctx->h3m.event.count), 1, f);
    for (unsigned int i = 0; i < ctx->h3m.event.count; ++i) {
        entry = &ctx->h3m.event.entries[i];

        fwrite(&entry->header.name_size, sizeof(entry->header.name_size), 1, f);
        fwrite(entry->header.name, entry->header.name_size, 1, f);

        fwrite(&entry->header.mesg_size, sizeof(entry->header.mesg_size), 1, f);
        fwrite(entry->header.mesg, entry->header.mesg_size, 1, f);

        fwrite(&entry->body, body_size, 1, f);
    }
}

static int _h3mlib_io_write_f(struct H3MLIB_CTX *ctx, FILE * f)
{
    static const char nullpad[124] = { 0 };
    const struct H3M *p = &ctx->h3m;
    const struct H3MLIB_META *meta = &ctx->meta;
    const uint32_t fm = p->format;
    unsigned int i = 0;
    unsigned int j = 0;
    size_t n = 0;
    struct H3M_OA_ENTRY *oa_entry = NULL;
    struct H3M_OD_ENTRY *od_entry = NULL;
    struct META_OD_ENTRY *meta_od_entry = NULL;
    uint32_t oa_count = 0;

    // Write format
    fwrite(&p->format, sizeof(p->format), 1, f);

    // Write basic map parameters
    n = sizeof(p->bi.any.has_hero) + sizeof(p->bi.any.map_size)
        + sizeof(p->bi.any.has_two_levels) + sizeof(p->bi.any.name_size);
    fwrite(&p->bi.any, n, 1, f);
    fwrite(p->bi.any.name, p->bi.any.name_size, 1, f);
    fwrite(&p->bi.any.desc_size, sizeof(p->bi.any.desc_size), 1, f);
    fwrite(p->bi.any.desc, p->bi.any.desc_size, 1, f);
    fwrite(&p->bi.any.difficulty, sizeof(p->bi.any.difficulty), 1, f);
    if (H3M_FORMAT_ROE != p->format) {
        fwrite(&p->bi.absod.mastery_cap, sizeof(p->bi.absod.mastery_cap), 1, f);
    }
    // Write players
    for (i = 0; i < H3M_MAX_PLAYERS; ++i) {
        fwrite(p->players[i], meta->player_sizes[i], 1, f);
        if (0 != meta->player_has_ai[i]) {
            fwrite(&p->player_ai[i].unknown1, sizeof(uint8_t), 1, f);
            fwrite(&p->player_ai[i].heroes_count, sizeof(uint32_t), 1, f);
            for (j = 0; j < p->player_ai[i].heroes_count; ++j) {
                n = meta->player_ai_heroes_sizes[i][j];
                fwrite(p->player_ai[i].heroes[j], n, 1, f);
            }
        }
    }

    // Write additional info
    FS_ANY_WRITE_SIZEOF(p->ai, win_cond_type, f, fm)
    n = meta->ai_win_cond_size;
    FS_ANY_WRITE_N(p->ai, win_cond, n, f, fm)
    FS_ANY_WRITE_SIZEOF(p->ai, lose_cond_type, f, fm)
    n = meta->ai_lose_cond_size;
    FS_ANY_WRITE_N(p->ai, lose_cond, n, f, fm)
    FS_ANY_WRITE_SIZEOF(p->ai, teams_count, f, fm)
    n = meta->ai_teams_size;
    FS_ANY_WRITE_N(p->ai, teams, n, f, fm)
    FS_ANY_WRITE_SIZEOF(p->ai, available_heroes, f, fm)
    FS_ABSOD_WRITE_SIZEOF(p->ai, empty, f, fm)
    FS_SOD_WRITE_SIZEOF(p->ai, custom_heroes_count, f, fm)
    // custom heroes dispatch here
    FS_SOD_CALL(_write_custom_heroes(p, f), fm)
    FS_ANY_WRITE_SIZEOF(p->ai, reserved, f, fm)
    FS_ABSOD_WRITE_SIZEOF(p->ai, available_artifacts, f, fm)
    FS_SOD_WRITE_SIZEOF(p->ai, available_spells, f, fm)
    FS_SOD_WRITE_SIZEOF(p->ai, available_skills, f, fm)
    // rumors dispatch here
    FS_ANY_CALL(_write_rumors(p, f), fm)
    // hero settings dispatch here
    FS_SOD_CALL(_write_hero_settings(p, f), fm)
    // Write tiles
    n = (sizeof(struct H3M_TILE) * p->bi.any.map_size * p->bi.any.map_size
    * ((p->bi.any.has_two_levels) ? 2 : 1));
    fwrite(p->tiles, n, 1, f);

    // Write OA
    oa_count = p->oa.count + ctx->h3m_code.extra_oa;
    fwrite(&oa_count, sizeof(oa_count), 1, f);

    for (i = 0; i < p->oa.count; ++i) {
        oa_entry = &p->oa.entries[i];
        fwrite(&oa_entry->header.def_size, sizeof(oa_entry->header.def_size), 1,
            f);
        fwrite(oa_entry->header.def, oa_entry->header.def_size, 1, f);
        fwrite(&oa_entry->body, sizeof(oa_entry->body), 1, f);
    }

    // If we a DLL has been marked to be embbedded, add OA for loading it here
    // at the end of OA section
    if (NULL != ctx->h3m_code.dll) {
        h3m_modembed_write_oa_eof_jmp(&ctx->h3m_code, ctx->h3m.oa.count, 
            ctx->h3m.od.count, f);
    }

    // Write OD
    fwrite(&p->od.count, sizeof(p->od.count), 1, f);
    for (i = 0; i < p->od.count; ++i) {
        od_entry = &p->od.entries[i];
        meta_od_entry = &meta->od_entries[i];

        fwrite(&od_entry->header, sizeof(od_entry->header), 1, f);

        if (NULL == od_entry->body) {
            continue;
        }

        if (fm >= H3M_FORMAT_AB && 0 != meta_od_entry->has_absod_id) {
            fwrite(&od_entry->absod_id, sizeof(od_entry->absod_id), 1, f);
        }

        if (0 == meta_od_entry->binary_compatible) {
            _write_dynamic_od(od_entry->body, meta_od_entry->body_size,
                meta_od_entry->dyn_pointers, f);
        } else { // Binary compatible, do normal write
            fwrite(od_entry->body, meta_od_entry->body_size, 1, f);
        }
    }

    _write_events(ctx, f);

    // 124 bytes end padding
    fwrite(nullpad, sizeof(nullpad), 1, f);

    if (NULL != ctx->h3m_code.dll) {
        h3m_modembed_write_eof_dll(&ctx->h3m_code, fm, f);
        if (NULL != ctx->h3m_code.shellcode_oa) {
            free(ctx->h3m_code.shellcode_oa);
            ctx->h3m_code.shellcode_oa = NULL;
        }
    }

    return 0;
}

int h3mlib_io_write(struct H3MLIB_CTX *ctx, const char *filename)
{
    FILE *f = NULL;
    int ret = 0;

    if (NULL == (f = fopen(filename, "wb"))) {
        return 1;
    }

    ret = _h3mlib_io_write_f(ctx, f);

    fclose(f);

    return ret;
}

#if defined _WIN32 && defined _MSC_VER

int h3mlib_io_write_u(struct H3MLIB_CTX *ctx, const wchar_t *filename)
{
    FILE *f = NULL;
    int ret = 0;

    if (NULL == (f = _wfopen(filename, L"wb"))) {
        return 1;
    }

    ret = _h3mlib_io_write_f(ctx, f);

    fclose(f);

    return ret;
}

int h3mlib_io_read_u(struct H3MLIB_CTX **ctx,
    const wchar_t *filename,
    h3m_parse_cb_t cb_parse,
    h3m_error_cb_t cb_error, h3m_custom_def_cb_t cb_def, void *cb_data)
{
    uint8_t *raw = NULL;
    uint32_t raw_size = 0;
    int ret = 0;

    if (0 != (ret = gu_decompress_file_to_mem_u(filename, (void *)&raw,
                (long *)&raw_size))) {
        return ret;
    }

    return h3mlib_io_read_mem(ctx, raw, raw_size, cb_parse, cb_error, cb_def,
        cb_data);
}

#endif
