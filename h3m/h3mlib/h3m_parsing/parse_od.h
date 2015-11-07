// Created by John Åkerblom 2014-11-22

#ifndef __PARSE_OD_H_DEF__
#define __PARSE_OD_H_DEF__

#include "../h3mlib.h"
#include "../h3mlib_ctx.h"
#include "../h3m_structures/h3m_od_body_ext_contents.h"
#include "../h3m_structures/h3m_od_body_ext_guardians.h"
#include "../h3m_structures/h3m_od_body_ext_quest.h"

#include "../../../3rdparty/uthash/src/utarray.h"

int parse_od(struct H3MLIB_CTX *ctx);

int parse_od_event(struct H3MLIB_CTX *ctx,
    struct H3M_OD_ENTRY *od_entry, struct META_OD_ENTRY *meta_od_entry);
int parse_od_town(struct H3MLIB_CTX *ctx, struct H3M_OD_ENTRY *od_entry,
    struct META_OD_ENTRY *meta_od_entry);
int parse_od_hero(struct H3MLIB_CTX *ctx, struct H3M_OD_ENTRY *od_entry,
    struct META_OD_ENTRY *meta_od_entry);
int parse_od_random_dwelling(struct H3MLIB_CTX *ctx,
    struct H3M_OD_ENTRY *od_entry, struct META_OD_ENTRY *meta_od_entry);
int parse_od_seers_hut(struct H3MLIB_CTX *ctx, struct H3M_OD_ENTRY *od_entry,
    struct META_OD_ENTRY *meta_od_entry);
int parse_od_artifact(struct H3MLIB_CTX *ctx, struct H3M_OD_ENTRY *od_entry,
    struct META_OD_ENTRY *meta_od_entry);
int parse_od_message_bearer(struct H3MLIB_CTX *ctx,
    struct H3M_OD_ENTRY *od_entry, struct META_OD_ENTRY *meta_od_entry);
int parse_od_monster(struct H3MLIB_CTX *ctx, struct H3M_OD_ENTRY *od_entry,
    struct META_OD_ENTRY *meta_od_entry);
int parse_od_pandoras_box(struct H3MLIB_CTX *ctx, struct H3M_OD_ENTRY *od_entry,
    struct META_OD_ENTRY *meta_od_entry);
int parse_od_resource(struct H3MLIB_CTX *ctx, struct H3M_OD_ENTRY *od_entry,
    struct META_OD_ENTRY *meta_od_entry);
int parse_od_spell_scroll(struct H3MLIB_CTX *ctx, struct H3M_OD_ENTRY *od_entry,
    struct META_OD_ENTRY *meta_od_entry);
int parse_od_quest_guard(struct H3MLIB_CTX *ctx, struct H3M_OD_ENTRY *od_entry,
    struct META_OD_ENTRY *meta_od_entry);

int parse_od_ext_contents(struct H3MLIB_CTX *ctx,
    struct H3M_OD_BODY_EXT_CONTENTS *contents,
    struct META_OD_ENTRY *meta_od_entry);
int parse_od_ext_guardians(struct H3MLIB_CTX *ctx,
    struct H3M_OD_BODY_EXT_GUARDIANS *guardians,
    struct META_OD_ENTRY *meta_od_entry);
int parse_od_ext_quest(struct H3MLIB_CTX *ctx,
    uint8_t quest_type,
    struct H3M_OD_BODY_EXT_QUEST *quest, struct META_OD_ENTRY *meta_od_entry);

size_t sizeof_quest(uint8_t *quest, uint8_t quest_type);

#endif
