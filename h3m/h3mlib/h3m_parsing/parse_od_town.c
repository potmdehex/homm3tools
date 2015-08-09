// Created by John Åkerblom 2014-11-22

#include "parse_od.h"
#include "parsing_common.h"

int parse_od_town(struct H3MLIB_CTX *ctx,
    struct H3M_OD_ENTRY *od_entry, struct META_OD_ENTRY *meta_od_entry)
{
    struct H3M *p = &ctx->h3m;
    struct H3MLIB_PARSING *parsing = &ctx->parsing;
    //struct H3MLIB_META *meta = &ctx->meta;
    const uint32_t fm = p->format;
    struct H3M_OD_BODY_DYNAMIC_TOWN *body = NULL;
    struct H3M_OD_BODY_EXT_TOWN_EVENT *event = NULL;
    unsigned int i = 0;
    size_t n = 0;

    od_entry->body = calloc(1, sizeof(*body));
    body = (struct H3M_OD_BODY_DYNAMIC_TOWN *)od_entry->body;
    meta_od_entry->binary_compatible = 0;
    meta_od_entry->body_size = sizeof(*body);

    if (p->format >= H3M_FORMAT_AB) {
        SAFE_READ_SIZEOF(&od_entry->absod_id, parsing)
    }
    SAFE_READ_SIZEOF(&body->owner, parsing)

    SAFE_READ_SIZEOF(&body->has_name, parsing)
    if (0 != body->has_name) {
        SAFE_READ_COMMON_STRING(body->name, 32, parsing)
    }

    SAFE_READ_SIZEOF(&body->has_creatures, parsing);
    if (0 != body->has_creatures) {
        n = (H3M_FORMAT_ROE == fm) ? 
            sizeof(body->creatures->roe) : sizeof(body->creatures->absod);
        body->creatures = calloc(1, n);
        SAFE_READ_N(body->creatures, n, parsing)
    }

    SAFE_READ_SIZEOF(&body->formation, parsing)

    SAFE_READ_SIZEOF(&body->has_buildings, parsing)
    if (0 != body->has_buildings) {
        n = sizeof(*(body->buildings));
        body->buildings = calloc(1, n);
        SAFE_READ_SIZEOF(&body->buildings->built, parsing)
        SAFE_READ_SIZEOF(&body->buildings->disabled, parsing)
    } else {
        SAFE_READ_SIZEOF(&body->has_fort, parsing)
    }

    if (fm >= H3M_FORMAT_AB) {
        SAFE_READ_SIZEOF(&body->must_have_spells, parsing)
    }

    SAFE_READ_SIZEOF(&body->may_have_spells, parsing)

    SAFE_READ_SIZEOF(&body->event_count, parsing)
    if (0 != body->event_count) {
        n = body->event_count * sizeof(*(body->events));
        SAFE_ALLOC_N(body->events, n, 8192)
        for (i = 0; i < body->event_count; ++i) {
            event = &body->events[i];

            SAFE_READ_STRING(event->name_size, 512, event->name, parsing)
            SAFE_READ_STRING(event->mesg_size, 32768, event->mesg, parsing)

            SAFE_READ_SIZEOF(&event->resources, parsing);
            SAFE_READ_SIZEOF(&event->applies_to_players, parsing);
            if (fm >= H3M_FORMAT_SOD) {
                SAFE_READ_SIZEOF(&event->applies_to_human, parsing);
            }
            SAFE_READ_SIZEOF(&event->applies_to_computer, parsing);
            SAFE_READ_SIZEOF(&event->first_occurence, parsing);
            SAFE_READ_SIZEOF(&event->subsequent_occurences, parsing);
            SAFE_READ_SIZEOF_BUF(event->unknown1, parsing);
            SAFE_READ_SIZEOF(&event->buildings, parsing);
            //FS_ANY_READ_SIZEOF(event->creatures, slots, parsing, fm)
            SAFE_READ_SIZEOF_BUF(event->creature_quantities, parsing)
            SAFE_READ_SIZEOF_BUF(event->unknown2, parsing);
        };
    }

    if (fm >= H3M_FORMAT_SOD) {
        SAFE_READ_SIZEOF(&body->alignment, parsing)
    }

    SAFE_READ_SIZEOF(&body->unknown1, parsing)

    return 0;
}
