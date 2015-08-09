// Created by John Åkerblom 2014-11-22

#include "parse_od.h"
#include "parsing_common.h"

int parse_od_ext_guardians(struct H3MLIB_CTX *ctx,
    struct H3M_OD_BODY_EXT_GUARDIANS *guardians,
    struct META_OD_ENTRY *meta_od_entry)
{
    struct H3M *p = &ctx->h3m;
    struct H3MLIB_PARSING *parsing = &ctx->parsing;
    const uint32_t fm = p->format;
    size_t creatures_size = 0;

    // Fan map "Cloak of the Undead King" has >29000 bytes long mesg...
    SAFE_READ_STRING(guardians->mesg_size, 32768, guardians->mesg, parsing)

    SAFE_READ_SIZEOF(&guardians->has_creatures, parsing)
    if (0 != guardians->has_creatures) {
        creatures_size = (H3M_FORMAT_ROE == fm) ? 
            sizeof(guardians->creatures->roe)
            : sizeof(guardians->creatures->absod);
        guardians->creatures = calloc(1, creatures_size);
        SAFE_READ_N(guardians->creatures, creatures_size, parsing)
    }

    SAFE_READ_SIZEOF_BUF(guardians->unknown1, parsing)

    return 0;
}
