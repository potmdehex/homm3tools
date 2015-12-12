// Created by John Åkerblom 2014-11-22

#include "parse_od.h"
#include "parsing_common.h"

int parse_od_hero(struct H3MLIB_CTX *ctx,
    struct H3M_OD_ENTRY *od_entry, struct META_OD_ENTRY *meta_od_entry)
{
    struct H3M *p = &ctx->h3m;
    struct H3MLIB_PARSING *parsing = &ctx->parsing;
    //struct H3MLIB_META *meta = &ctx->meta;
    const uint32_t fm = p->format;
    struct H3M_OD_BODY_DYNAMIC_HERO *body = NULL;
    size_t n = 0;
    size_t old_size = 0;
    size_t count = 0;

    od_entry->body = calloc(1, sizeof(*body));
    body = (struct H3M_OD_BODY_DYNAMIC_HERO *)od_entry->body;
    meta_od_entry->binary_compatible = 0;
    meta_od_entry->body_size = sizeof(*body);

    if (p->format >= H3M_FORMAT_AB) {
        SAFE_READ_SIZEOF(&od_entry->absod_id, parsing)
        meta_od_entry->has_absod_id = 1;
    }

    SAFE_READ_SIZEOF(&body->owner, parsing)
    SAFE_READ_SIZEOF(&body->type, parsing)

    SAFE_READ_SIZEOF(&body->has_name, parsing)
    if (0 != body->has_name) {
        SAFE_READ_COMMON_STRING(body->name, 32, parsing)
    }

    if (fm >= H3M_FORMAT_SOD) {
        SAFE_READ_SIZEOF(&body->has_experience, parsing)
        if (0 != body->has_experience) {
            SAFE_READ_SIZEOF(&body->experience, parsing)
        }
    } else {
        SAFE_READ_SIZEOF(&body->experience, parsing)
    }

    SAFE_READ_SIZEOF(&body->has_face, parsing)
    if (0 != body->has_face) {
        SAFE_READ_SIZEOF(&body->face, parsing)
    }

    SAFE_READ_SIZEOF(&body->has_secondary_skills, parsing)
    if (0 != body->has_secondary_skills) {
        SAFE_READ_N(&count, sizeof(body->secondary_skills->count), parsing)
        if (0 != count) {
            n = count * sizeof(body->secondary_skills->skills[0]);
            SAFE_ALLOC_N(body->secondary_skills,
                sizeof(body->secondary_skills->count) + n, 128)
                body->secondary_skills->count = count;
            SAFE_READ_N(body->secondary_skills->skills, n, parsing)
        }
    }

    SAFE_READ_SIZEOF(&body->has_creatures, parsing)
    if (0 != body->has_creatures) {
        n = (H3M_FORMAT_ROE == fm) ? 
            sizeof(body->creatures->roe) : sizeof(body->creatures->absod);
        body->creatures = calloc(1, n);
        SAFE_READ_N(body->creatures, n, parsing)
    }

    SAFE_READ_SIZEOF(&body->formation, parsing)

    SAFE_READ_SIZEOF(&body->has_artifacts, parsing)
    if (0 != body->has_artifacts) {
        // Read worn artifacts and backpack count
        n = (H3M_FORMAT_ROE == fm) ? sizeof(body->artifacts->roe)
            : ((H3M_FORMAT_AB == fm) ? sizeof(body->artifacts->ab)
            : sizeof(body->artifacts->sod));
        body->artifacts = calloc(1, n);
        SAFE_READ_N(body->artifacts, n, parsing)
            count =
            *(uint16_t *)(&parsing->raw[parsing->offset - sizeof(uint16_t)]);
        // Setting artifacts_size member here is a hack, code can be written
        // so it is not needed
        if (0 != count) {
            old_size = n;
            n = count * ((H3M_FORMAT_ROE == fm) ?
                sizeof(body->artifacts->roe.backpack.artifacts[0])
                : sizeof(body->artifacts->absod.backpack.artifacts[0]));
            body->artifacts = realloc(body->artifacts, old_size + n);
            SAFE_READ_N((uint8_t *)body->artifacts + old_size, n, parsing)
        }
    }

    SAFE_READ_SIZEOF(&body->patrol_radius, parsing)

    if (fm >= H3M_FORMAT_AB) {
        SAFE_READ_SIZEOF(&body->has_biography, parsing)
        if (0 != body->has_biography) {
            // Map "Der Ringkrieg (Deluexe Edit EX)" has 5587 bytes long cool story here...
            // Map "Megalopolis" 12513 bytes, Map "W pogoni za Kaka³o" is a troll with 30000 spaces
            SAFE_READ_COMMON_STRING(body->biography, 32768, parsing)
        }
        SAFE_READ_SIZEOF(&body->gender, parsing)
    }

    if (fm >= H3M_FORMAT_SOD) {
        SAFE_READ_SIZEOF(&body->has_spells, parsing)
        if (0 != body->has_spells) {
            body->spells = calloc(1, sizeof(body->spells->sod));
            SAFE_READ_SIZEOF_BUF(body->spells->sod, parsing)
        }
    } else if (H3M_FORMAT_AB == fm) {
        // In AB, only one spell can be specified
        body->spells = calloc(1, sizeof(body->spells->ab));
        SAFE_READ_SIZEOF(&body->spells->ab, parsing)
    }

    if (fm >= H3M_FORMAT_SOD) {
        SAFE_READ_SIZEOF(&body->has_primary_skills, parsing)
        if (0 != body->has_primary_skills) {
            body->primary_skills = calloc(1, sizeof(*body->primary_skills));
            SAFE_READ_N(body->primary_skills, sizeof(*(body->primary_skills)),
                parsing)
        }
    }

    SAFE_READ_SIZEOF_BUF(body->unknown2, parsing)

    return 0;
}
