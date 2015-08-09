// Created by John Åkerblom 2014-11-22

#include "parse_od.h"
#include "parsing_common.h"

#include <stdio.h>

static int _parse_body(struct H3MLIB_CTX *ctx,
    struct H3M_OD_ENTRY *od_entry, struct META_OD_ENTRY *meta_od_entry)
{
    struct H3MLIB_PARSING *parsing = &ctx->parsing;
    const uint32_t fm = ctx->h3m.format;
    int ret = 0;

    // Explicitly default to 0 body size, binary compatible
    meta_od_entry->body_size = 0;
    meta_od_entry->binary_compatible = 1;

    switch (meta_od_entry->oa_type) {
    case H3M_OBJECT_PLACEHOLDER_HERO:
        // If hero type is not specified, one extra byte for power rating exists
        meta_od_entry->body_size
            =
            (0xFF ==
            ((struct H3M_OD_BODY_STATIC_PLACEHOLDER_HERO_ANY *)&parsing->
                raw[parsing->offset])->
            type) ? sizeof(struct H3M_OD_BODY_STATIC_PLACEHOLDER_HERO_ANY)
            : sizeof(struct H3M_OD_BODY_STATIC_PLACEHOLDER_HERO_SPECIFIC);
        break;
    case H3M_OBJECT_QUEST_GUARD:
        ret = parse_od_quest_guard(ctx, od_entry, meta_od_entry);
        break;
    case H3M_OBJECT_PANDORAS_BOX:
        ret = parse_od_pandoras_box(ctx, od_entry, meta_od_entry);
        break;
    case H3M_OBJECT_SIGN:
    case H3M_OBJECT_OCEAN_BOTTLE:
        ret = parse_od_message_bearer(ctx, od_entry, meta_od_entry);
        break;
    case H3M_OBJECT_GARRISON:
    case H3M_OBJECT_GARRISON_ABSOD:
        meta_od_entry->body_size =
            (H3M_FORMAT_ROE ==
            fm) ? sizeof(struct H3M_OD_BODY_STATIC_GARRISON_ROE)
            : sizeof(struct H3M_OD_BODY_STATIC_GARRISON_ABSOD);
        break;
    case H3M_OBJECT_EVENT:
        ret = parse_od_event(ctx, od_entry, meta_od_entry);
        break;
    case H3M_OBJECT_GRAIL:
        meta_od_entry->body_size = sizeof(struct H3M_OD_BODY_STATIC_GRAIL);
        break;
    case H3M_OBJECT_DWELLING:
    case H3M_OBJECT_DWELLING_ABSOD:
    case H3M_OBJECT_LIGHTHOUSE:
    case H3M_OBJECT_RESOURCE_GENERATOR:
    case H3M_OBJECT_SHIPYARD:
    case H3M_OBJECT_ABANDONED_MINE_ABSOD:
        meta_od_entry->body_size = sizeof(struct H3M_OD_BODY_STATIC_FLAGGED);
        break;
    case H3M_OBJECT_GENERIC_BOAT:
    case H3M_OBJECT_GENERIC_PASSABLE_TERRAIN:
    case H3M_OBJECT_GENERIC_PASSABLE_TERRAIN_SOD:
    case H3M_OBJECT_GENERIC_IMPASSABLE_TERRAIN:
    case H3M_OBJECT_GENERIC_IMPASSABLE_TERRAIN_ABSOD:
    case H3M_OBJECT_GENERIC_VISITABLE:
    case H3M_OBJECT_GENERIC_VISITABLE_ABSOD:
    case H3M_OBJECT_GENERIC_TREASURE:
    case H3M_OBJECT_MONOLITH_TWO_WAY:
    case H3M_OBJECT_SUBTERRANEAN_GATE:
        // Generic objects have no body
        break;
    case H3M_OBJECT_TOWN:
    case H3M_OBJECT_TOWN_ABSOD:
        ret = parse_od_town(ctx, od_entry, meta_od_entry);
        break;
    case H3M_OBJECT_RANDOM_DWELLING_ABSOD:
    case H3M_OBJECT_RANDOM_DWELLING_PRESET_ALIGNMENT_ABSOD:
    case H3M_OBJECT_RANDOM_DWELLING_PRESET_LEVEL_ABSOD:
        ret = parse_od_random_dwelling(ctx, od_entry, meta_od_entry);
        break;
    case H3M_OBJECT_HERO:
    case H3M_OBJECT_RANDOM_HERO:
    case H3M_OBJECT_PRISON:
        ret = parse_od_hero(ctx, od_entry, meta_od_entry);
        break;
    case H3M_OBJECT_MONSTER:
    case H3M_OBJECT_MONSTER_ABSOD:
        ret = parse_od_monster(ctx, od_entry, meta_od_entry);
        break;
    case H3M_OBJECT_ARTIFACT:
    case H3M_OBJECT_ARTIFACT_AB:
    case H3M_OBJECT_ARTIFACT_SOD:
        ret = parse_od_artifact(ctx, od_entry, meta_od_entry);
        break;
    case H3M_OBJECT_SHRINE:
        meta_od_entry->body_size = sizeof(struct H3M_OD_BODY_STATIC_SHRINE);
        break;
    case H3M_OBJECT_SPELL_SCROLL:
        ret = parse_od_spell_scroll(ctx, od_entry, meta_od_entry);
        break;
    case H3M_OBJECT_RESOURCE:
        ret = parse_od_resource(ctx, od_entry, meta_od_entry);
        break;
    case H3M_OBJECT_WITCH_HUT:
        if (fm >= H3M_FORMAT_AB)        // Witch Hut has body only in AB+
        {
            meta_od_entry->body_size =
                sizeof(struct H3M_OD_BODY_STATIC_WITCH_HUT_ABSOD);
        }
        break;
    case H3M_OBJECT_SEERS_HUT:
        ret = parse_od_seers_hut(ctx, od_entry, meta_od_entry);
        break;
    case H3M_OBJECT_SCHOLAR:
        meta_od_entry->body_size = sizeof(struct H3M_OD_BODY_STATIC_SCHOLAR);
        break;
    default:
        //printf("Unknown type %d!\n", meta_od_entry->oa_type);
        return 1;
    }

    if (0 == ret) {
        // If we have a body size and the od is binary compatible, do a simple read
        if (0 != meta_od_entry->body_size
            && 0 != meta_od_entry->binary_compatible) {
            od_entry->body = calloc(1, meta_od_entry->body_size);
            SAFE_READ_N(od_entry->body, meta_od_entry->body_size, parsing)
        } else if (0 != meta_od_entry->body_size)       // Mark up binary incompatible body
        {
            meta_push_od(fm, od_entry->body, meta_od_entry);
        }
    }

    return ret;
}

int parse_od(struct H3MLIB_CTX *ctx)
{
    struct H3M *p = &ctx->h3m;
    struct H3MLIB_PARSING *parsing = &ctx->parsing;
    struct H3MLIB_META *meta = &ctx->meta;
    int orig_off = parsing->offset;
    int ret = 0;
    size_t n = 0;
    unsigned int i = 0;
    struct H3M_OD *od = &p->od;
    struct H3M_OD_ENTRY *od_entry = NULL;
    struct META_OA_ENTRY *meta_oa_entry = NULL;
    struct META_OD_ENTRY *meta_od_entry = NULL;
    char error[512] = { 0 };

    SAFE_READ_SIZEOF(&od->count, parsing)
    meta->od_entries_count = od->count;
    // The count is a 32-bit value, but somewhere above 32k objects doesn't seem legit
    SAFE_CHECK_N(od->count, 32768)
    if (0 != od->count) {
        od->entries = calloc(od->count, sizeof(*(od->entries)));
        meta->od_entries = calloc(od->count, sizeof(*(meta->od_entries)));
        for (i = 0; i < od->count; ++i) {
            od_entry = &od->entries[i];
            meta_od_entry = &meta->od_entries[i];

            SAFE_READ_SIZEOF(&od_entry->header, parsing)

            SAFE_CHECK_N(od_entry->header.z, 1)
            SAFE_CHECK_N(od_entry->header.oa_index, meta->oa_entries_count)
            meta_oa_entry = &meta->oa_entries[od_entry->header.oa_index];
            meta_od_entry->oa_type = meta_oa_entry->type;

            // TODO this define should be autogenerated in h3m_object.h
#define H3M_OBJECT_GREATEST H3M_OBJECT_WITCH_HUT
            //if (meta_od_entry->oa_type >= H3M_OBJECT_GREATEST)
            if (0xFF == meta_od_entry->oa_type) {
                if (NULL != ctx->callbacks.cb_error) {
                    _snprintf(error, sizeof(error) - 1,
                        "At coordinates '%d,%d,%d' in map '%s': unknown def '%s'"
                        " with unknown object size, cannot continue parsing",
                        od_entry->header.x,
                        od_entry->header.y,
                        od_entry->header.z,
                        (char *)p->bi.any.name,
                        (char *)p->oa.entries[od_entry->header.oa_index].header.
                        def);
                    ctx->callbacks.cb_error(error, ctx->callbacks.cb_data);
                }
                return 1;
            }

            if (0 != _parse_body(ctx, od_entry, meta_od_entry)) {
                SAFE_CHECK_NOT_PASSED()
            }
        }
    }

    if (NULL != ctx->callbacks.cb_parse) {
        n = orig_off - parsing->offset;
        ret = ctx->callbacks.cb_parse(orig_off, "od", &parsing->raw[orig_off], 
            n, ctx->callbacks.cb_data);
    }

    return ret;
}
