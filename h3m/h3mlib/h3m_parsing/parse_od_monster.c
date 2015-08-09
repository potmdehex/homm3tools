// Created by John Åkerblom 2014-11-22

#include "parse_od.h"
#include "parsing_common.h"

int parse_od_monster(struct H3MLIB_CTX *ctx,
    struct H3M_OD_ENTRY *od_entry, struct META_OD_ENTRY *meta_od_entry)
{
    struct H3M *p = &ctx->h3m;
    struct H3MLIB_PARSING *parsing = &ctx->parsing;
    const uint32_t fm = p->format;
    struct H3M_OD_BODY_DYNAMIC_MONSTER *body = NULL;
    size_t n = 0;

    if (fm >= H3M_FORMAT_AB) {
        SAFE_READ_SIZEOF(&od_entry->absod_id, parsing)
            meta_od_entry->has_absod_id = 1;
    }

    if (0 == ((struct H3M_OD_BODY_STATIC_MONSTER *)
        &parsing->raw[parsing->offset])->has_mesg_and_treasure) {
        meta_od_entry->binary_compatible = 1;
        meta_od_entry->body_size = sizeof(struct H3M_OD_BODY_STATIC_MONSTER);
        return 0;
    }

    od_entry->body = calloc(1, sizeof(*body));
    body = (struct H3M_OD_BODY_DYNAMIC_MONSTER *)od_entry->body;
    meta_od_entry->binary_compatible = 0;
    meta_od_entry->body_size = sizeof(*body);

    n = sizeof(body->quantity) + sizeof(body->disposition) +
        sizeof(body->has_mesg_and_treasure);
    SAFE_READ_N(body, n, parsing)

    if (0 != body->has_mesg_and_treasure) {
        // Map "Shar's War" 5551 bytes long mesg
        SAFE_READ_STRING(body->mesg_and_treasure.mesg_size, 8192,
            body->mesg_and_treasure.mesg, parsing)

        n =((H3M_FORMAT_ROE == fm) ? 
            sizeof(body->mesg_and_treasure.treasure->roe)
            : sizeof(body->mesg_and_treasure.treasure->absod));
        body->mesg_and_treasure.treasure = calloc(1, n);
        SAFE_READ_N(body->mesg_and_treasure.treasure, n, parsing)
    }

    n = sizeof(body->never_flees) + sizeof(body->does_not_grow) +
        sizeof(body->unknown1);
    SAFE_READ_N(&body->never_flees, n, parsing)

    return 0;
}
