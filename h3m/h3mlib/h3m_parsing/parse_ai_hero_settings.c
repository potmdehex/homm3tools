// Created by John Åkerblom 2014-11-21

#include "../h3mlib.h"
#include "parse_ai.h"
#include "../utils/safe_read.h"

#include <stdlib.h>
#include <string.h>

// TODO: in order to support any mods, something needs to be done about the .sod
// references, such as using the FS_ macros instead

int parse_ai_hero_settings(struct H3MLIB_CTX *ctx)
{
    struct H3M *p = &ctx->h3m;
    //struct H3MLIB_META*meta = &ctx->meta;
    struct H3MLIB_PARSING *parsing = &ctx->parsing;
    //int orig_off = ctx->parsing.offset;
    //int ret = 0;
    size_t n = 0;
    unsigned int i = 0;
    struct H3M_AI_HERO_SETTINGS *settings = NULL;

    for (i = 0; i < sizeof(p->ai.sod.hero_settings) / sizeof(*settings); ++i) {
        settings = &p->ai.sod.hero_settings[i];
        SAFE_READ_SIZEOF(&settings->has_settings, parsing)
        if (0 == settings->has_settings) {
            continue;
        }
        SAFE_READ_SIZEOF(&settings->has_experience, parsing)
        if (0 != settings->has_experience) {
            SAFE_READ_SIZEOF(&settings->experience, parsing)
        }
        SAFE_READ_SIZEOF(&settings->has_secondary_skills, parsing)
        if (0 != settings->has_secondary_skills) {
            SAFE_READ_SIZEOF(&settings->secondary_skills.count, parsing)
            n = settings->secondary_skills.count
                * sizeof(struct H3M_COMMON_SECONDARY_SKILL);
            SAFE_ALLOC_N(settings->secondary_skills.entries, n, 128)
            SAFE_READ_N(settings->secondary_skills.entries, n, parsing)
        }
        SAFE_READ_SIZEOF(&settings->has_artifacts, parsing)
        if (0 != settings->has_artifacts) {
            // backpack_entries is dynamic member of the struct
            n = sizeof(settings->artifacts)
                - sizeof(settings->artifacts.backpack_entries);
            SAFE_READ_N(&settings->artifacts, n, parsing)
            if (0 != settings->artifacts.backpack_count) {
                n = settings->artifacts.backpack_count
                    * sizeof(struct H3M_COMMON_BACKPACK_ENTRY);
                SAFE_ALLOC_N(settings->artifacts.backpack_entries, n, 255)
                SAFE_READ_N(settings->artifacts.backpack_entries, n,
                    parsing)
            }
        }
        SAFE_READ_SIZEOF(&settings->has_biography, parsing)
            if (0 != settings->has_biography) {
            SAFE_READ_SIZEOF(&settings->biography.desc_size, parsing)
            if (0 != settings->biography.desc_size) {
                n = settings->biography.desc_size;
                SAFE_ALLOC_N(settings->biography.desc, n, 4096)
                SAFE_READ_N(settings->biography.desc, n, parsing)
            }
        }
        SAFE_READ_SIZEOF(&settings->gender, parsing)
        SAFE_READ_SIZEOF(&settings->has_spells, parsing)
        if (0 != settings->has_spells) {
            SAFE_READ_SIZEOF(&settings->spells, parsing)
        }
        SAFE_READ_SIZEOF(&settings->has_primary_skills, parsing)
        if (0 != settings->has_primary_skills) {
            SAFE_READ_SIZEOF(&settings->primary_skills, parsing)
        }
    }

    return 0;
}
