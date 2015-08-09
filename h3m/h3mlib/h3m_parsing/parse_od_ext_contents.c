// Created by John Åkerblom 2014-11-22

#include "parse_od.h"
#include "parsing_common.h"

int parse_od_ext_contents(struct H3MLIB_CTX *ctx,
    struct H3M_OD_BODY_EXT_CONTENTS *contents,
    struct META_OD_ENTRY *meta_od_entry)
{
    struct H3M *p = &ctx->h3m;
    struct H3MLIB_PARSING *parsing = &ctx->parsing;
    const uint32_t fm = p->format;
    size_t n = 0;
    int ret = 0;

    n = sizeof(contents->experience) + sizeof(contents->spell_points) +
        sizeof(contents->morale)
        + sizeof(contents->luck) + sizeof(contents->resources) +
        sizeof(contents->primary_skills)
        + sizeof(contents->secondary_skills_count);
    SAFE_READ_N(&contents->experience, n, parsing)
    if (0 != contents->secondary_skills_count) {
        n = contents->secondary_skills_count *
            sizeof(*(contents->secondary_skills));
        SAFE_ALLOC_N(contents->secondary_skills, n, 512)
        SAFE_READ_N(contents->secondary_skills, n, parsing)
    }

    SAFE_READ_SIZEOF(&contents->artifacts_count, parsing)
    if (0 != contents->artifacts_count) {
        n = contents->artifacts_count
            * ((H3M_FORMAT_ROE == fm) ? sizeof(contents->artifacts->roe)
            : sizeof(contents->artifacts->absod));
        SAFE_ALLOC_N(contents->artifacts, n, 512)
        SAFE_READ_N(contents->artifacts, n, parsing)
    }

    SAFE_READ_SIZEOF(&contents->spells_count, parsing)
    if (0 != contents->spells_count) {
        n = contents->spells_count * sizeof(*(contents->spells));
        SAFE_ALLOC_N(contents->spells, n, 512)
        SAFE_READ_N(contents->spells, n, parsing)
    }

    SAFE_READ_SIZEOF(&contents->creatures_count, parsing)
    if (0 != contents->creatures_count) {
        n = contents->creatures_count
            * ((H3M_FORMAT_ROE == fm) ? 
            sizeof(contents->creatures->roe.slots[0])
            : sizeof(contents->creatures->absod.slots[0]));
        SAFE_ALLOC_N(contents->creatures, n, 128)
        SAFE_READ_N(contents->creatures, n, parsing)
    }

    SAFE_READ_SIZEOF_BUF(contents->unknown1, parsing)

        return 0;
}
