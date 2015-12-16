// Created by John Åkerblom 2014-11-22

#include "../h3mlib.h"
#include "parse_event.h"

#include "../utils/format_select.h"
#include "../utils/safe_read.h"

#include <stdlib.h>
#include <string.h>

int parse_event(struct H3MLIB_CTX *ctx)
{
    struct H3M *p = &ctx->h3m;
    struct H3MLIB_PARSING *parsing = &ctx->parsing;
    int orig_off = parsing->offset;
    int ret = 0;
    size_t n = 0;
    unsigned int i = 0;
    struct H3M_EVENT *event = &p->event;
    struct H3M_EVENT_ENTRY *e_entry = NULL;
    const uint32_t fm = p->format;

    SAFE_READ_SIZEOF(&event->count, parsing)
    if (0 != event->count) {
        n = event->count * sizeof(*(event->entries));
        // Pandora's Box actually has 100 events, which means 6700 bytes needed here
        SAFE_ALLOC_N(event->entries, n, 8192)
        for (i = 0; i < event->count; ++i) {
            e_entry = &event->entries[i];

            SAFE_READ_STRING(e_entry->header.name_size, 512,
                e_entry->header.name, parsing)
            SAFE_READ_STRING(e_entry->header.mesg_size, 32768,
                e_entry->header.mesg, parsing)

            if (fm >= H3M_FORMAT_SOD) {
                SAFE_READ_SIZEOF(&e_entry->body.sod, parsing)
            } else {
                SAFE_READ_SIZEOF(&e_entry->body.roeab, parsing)
            }
        }
    }

    if (NULL != ctx->callbacks.cb_parse) {
        if (0 != (ret = ctx->callbacks.cb_parse(orig_off, "event",
                    &parsing->raw[orig_off], n, ctx->callbacks.cb_data))) {
            return ret;
        }
    }
#if 0
    // Last 124 (0 for HD Edition maps) bytes are padding, we should have reached them
    size_t diff = ctx->parsing.raw_size - ctx->parsing.offset;
    if (124 == diff || 0 == diff) {
        return 0;
    }
#endif

    return 0;
}
