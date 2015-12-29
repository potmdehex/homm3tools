// Created by John Åkerblom 2015-06-06

#ifndef __PARSE_OA_TYPE_H_DEF__
#define __PARSE_OA_TYPE_H_DEF__

#include "../h3mlib.h"
#include "../internal/h3mlib_ctx.h"

// Determines meta type, i.e h3mlib's internal constant corresponding to the 
// def/object constants of this OA entry
int parse_oa_meta_type(struct H3M_OA_ENTRY *oa_entry, int *meta_type,
    int *is_custom, h3m_custom_def_cb_t cb, void *cb_data);

#endif
