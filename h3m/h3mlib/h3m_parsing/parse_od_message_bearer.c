// Created by John Åkerblom 2014-11-22

#include "parse_od.h"
#include "parsing_common.h"

int parse_od_message_bearer(struct H3MLIB_CTX *ctx,
    struct H3M_OD_ENTRY *od_entry, struct META_OD_ENTRY *meta_od_entry)
{
    struct H3M *p = &ctx->h3m;
    struct H3MLIB_PARSING *parsing = &ctx->parsing;
    const uint32_t fm = p->format;
    struct H3M_OD_BODY_DYNAMIC_MESSAGE_BEARER *body = NULL;
    size_t n = 0;

    od_entry->body = calloc(1, sizeof(*body));
    body = (struct H3M_OD_BODY_DYNAMIC_MESSAGE_BEARER *)od_entry->body;
    meta_od_entry->binary_compatible = 0;
    meta_od_entry->body_size = sizeof(*body);

    SAFE_READ_STRING(body->mesg_size, 1024, body->mesg, parsing)

    SAFE_READ_SIZEOF_BUF(body->unknown1, parsing)

    return 0;
}
