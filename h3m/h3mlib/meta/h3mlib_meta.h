// Created by John Åkerblom 2014-11-18

// Metadata used internally by h3mlib. This data is not present in the h3m
// format, rather it is data that describes the data that actually is present.
// Keeping track of for example sizes of dynamic members simplifies code
// interacting those members, such as the code used to write .h3m files, as that
// code then does not need any logic to figure out the size.

#ifndef __H3MLIB_META_H_DEF__
#define __H3MLIB_META_H_DEF__

#include "../msvc_comp_stdint.h"
#include "../h3m_structures/h3m.h"

#ifdef _MSC_VER
#pragma warning(disable:4996)   // for utarray.h strdup
#else
extern char *strdup(const char *);      // utarray.h references strdup, not used
#endif

#include "../../../3rdparty/uthash/src/utarray.h"
#include "../../../3rdparty/uthash/src/uthash.h"

struct META_OA_HASH_ENTRY {
    int oa_body_index;          // key
    char *def;                  // value
    int oa_index;
    UT_hash_handle hh;
};

struct META_OA_ENTRY {
    size_t size;
    int type;                   // enum OA_DEF_TYPES
};

struct META_DYN_ARRAY_ENTRY {
    uint8_t *body;
    size_t body_size;
    UT_array *dyn_pointers;
};

struct META_DYN_POINTER {
    uint8_t *body;
    void *p;
    UT_array *p_array;
    size_t dyn_size;
    unsigned int member_offset;
    unsigned int skip;
};

struct META_OD_ENTRY {
    int binary_compatible;
    size_t body_size;           // irrelevant if binary_compatible is 0
    int oa_type;
    UT_array *dyn_pointers;
    int has_absod_id;
};

struct H3MLIB_META {
    // TODO could put these player arrays in struct
    uint32_t player_sizes[H3M_MAX_PLAYERS];
    uint8_t player_ext_types[H3M_MAX_PLAYERS];
    uint8_t player_has_ai[H3M_MAX_PLAYERS];     // AB/SOD
    uint8_t *player_ai_heroes_sizes[H3M_MAX_PLAYERS];   // AB/SOD
    // Always (7 * map_size * map_ size * (has_two_levels? 2: 1)) bytes
    uint32_t oa_size;
    uint32_t od_size;
    uint32_t unknown_oaod_size;

    uint32_t ai_win_cond_size;
    uint32_t ai_lose_cond_size;
    uint32_t ai_teams_size;     // Just either 0/8
    uint32_t *ai_custom_heroes_sizes;   // AB/SOD

    uint32_t oa_entries_count;
    struct META_OA_ENTRY *oa_entries;

    struct META_OA_HASH_ENTRY *oa_hash;

    uint32_t od_entries_count;
    struct META_OD_ENTRY *od_entries;
};

#endif
