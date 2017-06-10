// Created by John Åkerblom 2014-11-18 

#include "h3mlib.h"

#include "gen/def_bodies_array.h"
#include "gen/def_bodies_hex.h"
#include "h3m_editing/default_od_body.h"
#include "h3m_editing/h3mlib_alg_od_text.h"
#include "h3m_min_maps/min_roe.h"
#include "h3m_min_maps/min_ab.h"
#include "h3m_min_maps/min_sod.h"
#include "h3m_parsing/parse_h3m.h"
#include "internal/h3mlib_ctx.h"
#include "internal/h3mlib_ctx_cleanup.h"
#include "io/h3mlib_io.h"
#include "meta/meta_object_name.h"
#include "utils/format_select.h"
#include "utils/memmem.h"

#include <gen_tile_sprites.h>

#ifndef NO_ZLIB
#include <gzip_utils.h>
#else
#include "utils/gzip_empty.h"
#endif

#include <ctype.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#ifdef _MSC_VER
#pragma warning(disable:4996)   // M$ standard lib unsafe
#endif

static int _name_to_def_index(const char *name)
{
    extern int object_names_hash(const char *key);
    //extern const char *def_bodies_array[];
    char name_fixed[64] = { 0 };
    int index = 0;

    // Use string with 0 at end if no last char is a letter and not 
    // a number
    if (0 != isalpha((int)name[strlen(name) - 1])) {
        strncpy(name_fixed, name, sizeof(name_fixed) - 1);
        if (strlen(name_fixed) < sizeof(name_fixed) - 1) {
            strcat(name_fixed, "0");
        }
        name = name_fixed;
    }

    if (-1 == (index = object_names_hash(name))) {
        return -1;
    } else if (index >= sizeof(def_bodies_array) / sizeof(void *)) {
        return -1;
    }

    return index;
}

static const char *_name_to_def(const char *name)
{
    int index = _name_to_def_index(name);

    return (-1 == index) ? NULL : def_bodies_array[index];
}

int h3m_read(h3mlib_ctx_t *ctx, const char *filename)
{
    return h3mlib_io_read(ctx, filename, NULL, NULL, NULL, NULL);
}

#if defined _WIN32 && defined _MSC_VER
int h3m_read_u(h3mlib_ctx_t *ctx, const wchar_t *filename)
{
    return h3mlib_io_read_u(ctx, filename, NULL, NULL, NULL, NULL);
}
#endif

int h3m_read_with_cbs(h3mlib_ctx_t *ctx,
    const char *filename,
    h3m_parse_cb_t cb_parse,
    h3m_error_cb_t cb_error, h3m_custom_def_cb_t cb_def, void *cb_data)
{
    return h3mlib_io_read(ctx, filename, cb_parse, cb_error, cb_def, cb_data);
}

#if defined _WIN32 && defined _MSC_VER
int h3m_read_with_cbs_u(h3mlib_ctx_t *ctx,
    const wchar_t *filename,
    h3m_parse_cb_t cb_parse,
    h3m_error_cb_t cb_error, h3m_custom_def_cb_t cb_def, void *cb_data)
{
    return h3mlib_io_read_u(ctx, filename, cb_parse, cb_error, cb_def, cb_data);
}
#endif

int h3m_write(h3mlib_ctx_t ctx, const char *filename)
{
    return h3mlib_io_write(ctx, filename);
}

#if defined _WIN32 && defined _MSC_VER
int h3m_write_u(h3mlib_ctx_t ctx, const wchar_t *filename)
{
    return h3mlib_io_write_u(ctx, filename);
}
#endif

int h3m_exit(h3mlib_ctx_t *ctx)
{
    return h3mlib_cleanup(ctx);
}

int h3m_generate_tiles(const h3mlib_ctx_t ctx, int size, int z,
    uint8_t *terrain_types, uint8_t *road_types, uint8_t *river_types)
{
    struct H3M *p = &((struct H3MLIB_CTX *)ctx)->h3m;

    if (p->bi.any.map_size != size) {
        p->bi.any.map_size = size;
        // Could dynamically check for underground, or have it passed somehow
        // as argument here. For now just alloc space for underground too
        p->tiles = calloc(1, (p->bi.any.has_two_levels ? 2 : 1) * size * size *
            sizeof(struct H3M_TILE));
    }

    unsigned char *tiles = (unsigned char *)p->tiles;

    if (0 != z) {
        tiles +=
            p->bi.any.map_size * p->bi.any.map_size * sizeof(struct H3M_TILE);
    }

    return gen_tile_sprites(terrain_types, road_types, river_types,
        p->bi.any.map_size, tiles);
}

int h3m_terrain_fill(const h3mlib_ctx_t ctx, enum H3M_TERRAIN terrain)
{
    int ret = 0;
    uint8_t t[2 * H3M_MAX_SIZE * H3M_MAX_SIZE];
    uint8_t r[2 * H3M_MAX_SIZE * H3M_MAX_SIZE];

    memset(t, terrain, sizeof(t));
    memset(r, 0, sizeof(r));

    if (0 != (ret =
            h3m_generate_tiles(ctx, ctx->h3m.bi.any.map_size, 0, (uint8_t *)t,
                (uint8_t *)r, (uint8_t *)r))) {
        return ret;
    }

    if (0 != ctx->h3m.bi.any.has_two_levels) {
        ret =
            h3m_generate_tiles(ctx, ctx->h3m.bi.any.map_size, 1, (uint8_t *)t,
            (uint8_t *)r, (uint8_t *)r);
    }

    return ret;
}

int h3m_terrain_set(const h3mlib_ctx_t ctx, int x, int y, int z,
    enum H3M_TERRAIN terrain)
{
    int ret = 0;
    uint8_t t[H3M_MAX_SIZE * H3M_MAX_SIZE];
    uint8_t r[H3M_MAX_SIZE * H3M_MAX_SIZE];

    memset(r, 0, sizeof(r));

    // Method here is meant for arrays so is not good for this, but currently no easy way to 
    // generate a single tile sprite is implemented.

    h3m_terrain_get_all(ctx, z, t, sizeof(t));
    t[H3M_2D_TO_1D(ctx->h3m.bi.any.map_size, x, y, z)] = terrain;

    if (0 != (ret =
            h3m_generate_tiles(ctx, ctx->h3m.bi.any.map_size, z, t, r, r))) {
        return ret;
    }

    return 0;
}

int h3m_terrain_set_all(const h3mlib_ctx_t ctx, int z, uint8_t *terrain_types)
{
    int ret = 0;
    uint8_t r[H3M_MAX_SIZE * H3M_MAX_SIZE];

    memset(r, 0, sizeof(r));

    if (0 != (ret =
            h3m_generate_tiles(ctx, ctx->h3m.bi.any.map_size, z, terrain_types,
                r, r))) {
        return ret;
    }

    return 0;
}

int h3m_compress(const char *input_filename, const char *output_filename)
{
    return gu_compress(input_filename, output_filename);
}

int h3m_decompress(const char *input_filename, const char *output_filename)
{
    return gu_decompress(input_filename, output_filename);
}

int h3m_init_min(h3mlib_ctx_t *ctx, uint32_t format, int size)
{
    struct H3MLIB_CTX *c = calloc(1, sizeof(*c));

    switch (format) {
    case H3M_FORMAT_ROE:
        c->parsing.raw = min_roe;
        c->parsing.raw_size = sizeof(min_roe);
        break;
    case H3M_FORMAT_AB:
        c->parsing.raw = min_ab;
        c->parsing.raw_size = sizeof(min_ab);
        break;
    case H3M_FORMAT_SOD:
        c->parsing.raw = min_sod;
        c->parsing.raw_size = sizeof(min_sod);
        break;
    default:
        return 1;
    }

    parse_h3m(c);
    if (c->h3m.bi.any.map_size != size) {
        free(c->h3m.tiles);
        c->h3m.bi.any.map_size = size;
        c->h3m.tiles = calloc(1, sizeof(struct H3M_TILE) * size * size * 2);
    }

    *ctx = c;

    return 0;
}

const struct H3M_OA_BODY *h3m_get_def_body(const char *def, int *oa_body_index)
{
    extern int def_bodies_hash(const char *key);
    struct H3M_OA_BODY *bodies = (struct H3M_OA_BODY *)output_def_bodies_hex;
    int index = 0;

    if (0 > (index = def_bodies_hash(def))) {
        return NULL;
    } else if (index >= sizeof(def_bodies_array) / sizeof(void *)) {
        return NULL;
    } else if (0 != strcmp(def_bodies_array[index], def)) {
        return NULL;
    }

    if (NULL != oa_body_index) {
        *oa_body_index = index;
    }

    return &bodies[index];
}

int h3m_add_oa_by_def(h3mlib_ctx_t ctx, const char *def, int *oa_index)
{
    struct H3M_OA_ENTRY *oa_entry = NULL;
    struct META_OA_HASH_ENTRY *oa_hash_entry = NULL;
    const struct H3M_OA_BODY *body = NULL;
    int oa_body_index = 0;

    if (NULL == (body = h3m_get_def_body(def, &oa_body_index))) {
        return 1;
    }

    *oa_index = ctx->h3m.oa.count++;
    ctx->h3m.oa.entries = realloc(ctx->h3m.oa.entries,
        sizeof(struct H3M_OA_ENTRY) * ctx->h3m.oa.count);
    oa_entry = &ctx->h3m.oa.entries[ctx->h3m.oa.count - 1];

    oa_entry->header.def_size = strlen(def);
    oa_entry->header.def = (uint8_t *)strdup(def);
    memcpy(&oa_entry->body, body, sizeof(struct H3M_OA_BODY));

    oa_hash_entry = calloc(1, sizeof(*oa_hash_entry));
    oa_hash_entry->def = strdup(def);
    oa_hash_entry->oa_body_index = oa_body_index;
    oa_hash_entry->oa_index = *oa_index;
    HASH_ADD_INT(ctx->meta.oa_hash, oa_body_index, oa_hash_entry);

    return 0;
}

int h3m_add_oa_entry(h3mlib_ctx_t ctx, struct H3M_OA_ENTRY *oa_entry,
    int *oa_index)
{
    int oa_body_index = 0;
    struct H3M_OA_BODY *bodies = (struct H3M_OA_BODY *)output_def_bodies_hex;
    struct H3M_OA_ENTRY *oa_entry_out = NULL;

    *oa_index = ctx->h3m.oa.count++;

    ctx->h3m.oa.entries = realloc(ctx->h3m.oa.entries,
        sizeof(struct H3M_OA_ENTRY) * ctx->h3m.oa.count);
    oa_entry_out = &ctx->h3m.oa.entries[ctx->h3m.oa.count - 1];

    memcpy(oa_entry_out, oa_entry, sizeof(*oa_entry_out));

    // Don't add to hash

    return 0;
}

void h3m_player_enable(h3mlib_ctx_t ctx, int player)
{
    ctx->h3m.players[player]->ab.can_be_human = 1;
    ctx->h3m.players[player]->ab.can_be_computer = 1;
}

// TODO move these some good place, they (and more) could be autogenerated as well
extern int rand_impassable_DIRT(int, int);
extern int rand_impassable_SAND(int, int);
extern int rand_impassable_GRASS(int, int);
extern int rand_impassable_SNOW(int, int);
extern int rand_impassable_SWAMP(int, int);
extern int rand_impassable_ROUGH(int, int);
extern int rand_impassable_SUBTERRANEAN(int, int);
extern int rand_impassable_LAVA(int, int);
extern int rand_impassable_WATER(int, int);
int (*const rand_fns[]) (int, int) = {
rand_impassable_DIRT,
        rand_impassable_SAND,
        rand_impassable_GRASS,
        rand_impassable_SNOW,
        rand_impassable_SWAMP,
        rand_impassable_ROUGH,
        rand_impassable_SUBTERRANEAN,
        rand_impassable_LAVA, rand_impassable_WATER};

int h3m_add_obstacle_with_passability(h3mlib_ctx_t ctx, int x, int y, int z,
    int x_max_dim, int y_max_dim, int terrain, uint64_t passability_in,
    int *od_index)
{
    const char *def = NULL;
    int def_index = -1;
    int x_dim_tmp = 0;
    int y_dim_tmp = 0;
    struct H3M_OA_BODY *oa_bodies = (struct H3M_OA_BODY *)output_def_bodies_hex;
    struct H3M_OA_BODY *oa_body = NULL;

    if (H3MLIB_TERRAIN_NATIVE == terrain) {
        terrain =
            ctx->h3m.tiles[H3M_2D_TO_1D(ctx->h3m.bi.any.map_size, x, y,
                z)].terrain_type;
    } else if (terrain >= sizeof(rand_fns) / sizeof(rand_fns[0])) {
        return 1;
    }
    // TODO: Could use passability-based table here instead and loop would not be needed.
    // Only implemented like this because of time-constraint
    for (;;) {
        x_dim_tmp = (1 == x_max_dim) ? 1 : rand() % (x_max_dim) + 1;
        y_dim_tmp = (1 == y_max_dim) ? 1 : rand() % (y_max_dim) + 1;

        if (-1 == (def_index = rand_fns[terrain] (x_dim_tmp, y_dim_tmp))) {
            continue;
        }

        oa_body = &oa_bodies[def_index];

        if (0 == memcmp(oa_body->passable, &passability_in,
                sizeof(oa_body->passable))) {
            break;
        }
    }

    def = def_bodies_array[def_index];

    return h3m_object_add_by_def(ctx, def, x, y, z, od_index);
}

int h3m_add_obstacle(h3mlib_ctx_t ctx, int x, int y, int z, int x_max_dim,
    int y_max_dim, int terrain, int *x_dim, int *y_dim, int *od_index,
    uint64_t * passability_out)
{
    const char *def = NULL;
    int def_index = -1;
    int x_dim_tmp = 0;
    int y_dim_tmp = 0;
    struct H3M_OA_BODY *oa_bodies = (struct H3M_OA_BODY *)output_def_bodies_hex;
    struct H3M_OA_BODY *oa_body = NULL;

    if (H3MLIB_TERRAIN_NATIVE == terrain) {
        terrain =
            ctx->h3m.tiles[H3M_2D_TO_1D(ctx->h3m.bi.any.map_size, x, y,
                z)].terrain_type;
    } else if (terrain >= sizeof(rand_fns) / sizeof(rand_fns[0])) {
        return 1;
    }
    // Don't really need a loop here, could use an approach with other tables,
    // where for example you get an array with ALL objects up to a max size, then
    // just take a random element once out of that array

    do {
        x_dim_tmp = (1 == x_max_dim) ? 1 : rand() % (x_max_dim) + 1;
        y_dim_tmp = (1 == y_max_dim) ? 1 : rand() % (y_max_dim) + 1;
        def_index = rand_fns[terrain] (x_dim_tmp, y_dim_tmp);
    } while (-1 == def_index);
    def = def_bodies_array[def_index];

    if (NULL != passability_out) {
        oa_body = &oa_bodies[def_index];
        memcpy(passability_out, oa_body->passable, sizeof(oa_body->passable));
    }

    if (NULL != x_dim) {
        *x_dim = x_dim_tmp;
        *y_dim = y_dim_tmp;
    }

    return h3m_object_add_by_def(ctx, def, x, y, z, od_index);
}

int h3m_analyze_passability(uint64_t passability, int x_bottom_right,
    int y_bottom_right, int z, int *x_dim, int *y_dim, passability_cb_t cb,
    void *cb_data)
{
    const int MAX_PASS_X = 8;
    const int MAX_PASS_Y = 6;
    static const unsigned char bits[] = { 0x80, 0x40, 0x20, 0x10,
        0x08, 0x04, 0x02, 0x01
    };
    int x = 0;
    int x_start = x_bottom_right - (MAX_PASS_X - 1);
    int x_min = MAX_PASS_X + 1;
    int x_max = -1;
    int y = 0;
    int y_min = MAX_PASS_Y + 1;
    int y_max = -1;
    int y_start = y_bottom_right - (MAX_PASS_Y - 1);
    uint8_t *p = (uint8_t *)& passability;

    // Passability is a 8x6 region starting at x_bottom_right-7,y_bottom_right-5
    // Iterate and call cb for any impassable tiles
    for (y = 0, y = 0; y < MAX_PASS_Y; ++y) {
        for (x = 0; x < MAX_PASS_X; ++x) {
            // If bit is set then this tile is not impassasable
            if (0 != (p[y] & bits[x])) {
                continue;
            }

            if (NULL != cb) {
                int cur_x = x_start + ((MAX_PASS_X - 1) - x);
                int cur_y = y_start + y;
                cb(cur_x, cur_y, z, cb_data);
            }

            x_min = (x_min > x) ? x : x_min;
            x_max = (x_max < x) ? x : x_max;
            y_min = (y_min > y) ? y : y_min;
            y_max = (y_max < y) ? y : y_max;
        }
    }

    if (x_min > MAX_PASS_X || -1 == x_max || y_min > MAX_PASS_Y || -1 == y_max) {
        return 1;
    }

    if (x_dim != NULL) {
        *x_dim = (x_max - x_min) + 1;
    }
    if (y_dim != NULL) {
        *y_dim = (y_max - y_min) + 1;
    }

    return 0;
}

int h3m_add_oa_by_name(h3mlib_ctx_t ctx, const char *name, int *oa_index)
{
    const char *def = _name_to_def(name);
    return (NULL == def) ? -1 : h3m_add_oa_by_def(ctx, def, oa_index);
}

int h3m_object_add(h3mlib_ctx_t ctx, const char *name, int x, int y, int z,
    int *od_index)
{
    const char *def = _name_to_def(name);
    return (NULL == def) ? -1 : h3m_object_add_by_def(ctx, def, x, y, z,
        od_index);
}

int h3m_object_text(h3mlib_ctx_t ctx, const char *name, int x, int y, int z,
    const char *text)
{
    const char *def = _name_to_def(name);
    int ret = 0;
    int oa_index = 0;

    if (NULL == def) {
        return 1;
    }

    if (0 != (ret = h3m_add_oa_by_def(ctx, def, &oa_index))) {
        return ret;
    }

    return h3m_alg_od_text(ctx, def, oa_index, x, y, z, text);
}

int h3m_add_od(h3mlib_ctx_t ctx, int oa_index, int x, int y, int z,
    int *od_index)
{
    extern int def_types_hash(const char *);
    struct H3M_OD_ENTRY *od_entry = NULL;
    struct META_OD_ENTRY *meta_od_entry = NULL;
    int meta_od_index = 0;
    int oa_type = 0;
    uint8_t *body = NULL;
    size_t body_size = 0;
    size_t n = 0;
    int binary_compatible = 0;
    int idx = 0;
    const char *def = NULL;
    char def_lower[16] = { 0 };
    char *cp = NULL;

    idx = ctx->h3m.od.count++;
    ctx->h3m.od.entries = realloc(ctx->h3m.od.entries,
        sizeof(struct H3M_OA_ENTRY) * ctx->h3m.od.count);
    od_entry = &ctx->h3m.od.entries[idx];

    memset(od_entry, 0, sizeof(*od_entry));
    od_entry->header.oa_index = oa_index;
    od_entry->header.x = x;
    od_entry->header.y = y;
    od_entry->header.z = z;

    meta_od_index = ctx->meta.od_entries_count++;
    n = ctx->meta.od_entries_count * sizeof(struct META_OD_ENTRY);
    ctx->meta.od_entries =
        (NULL != ctx->meta.od_entries) ? realloc(ctx->meta.od_entries, n)
        : calloc(1, n);
    meta_od_entry = &ctx->meta.od_entries[meta_od_index];

    // Add default OD body, looking it up in hash by lowercase def name

    def = (char *)ctx->h3m.oa.entries[oa_index].header.def;

    // TODO reused downcasing function
    strncpy(def_lower, def, sizeof(def_lower)-1);
    cp = def_lower;
    for (; *cp; ++cp)
        *cp = (char)tolower((int)*cp);
    def = def_lower;

    oa_type = def_types_hash(def);
    get_default_od_body(ctx->h3m.format, oa_type, &body, &binary_compatible,
        &body_size);

    od_entry->body = body;
    memset(meta_od_entry, 0, sizeof(*(meta_od_entry)));
    meta_od_entry->binary_compatible = binary_compatible;
    meta_od_entry->body_size = body_size;
    meta_od_entry->oa_type = oa_type;
    meta_od_entry->dyn_pointers = NULL;

    // Only towns, heroes and monsters have the absod id 
    // (used for win cond/defeat x quests)
    if (META_OBJECT_TOWN == oa_type || META_OBJECT_MONSTER == oa_type 
            || META_OBJECT_HERO == oa_type) {
        // TODO throughout h3mlib absod id support needs to be improved,
        // so e.g several objects with their own ids can be added
        meta_od_entry->has_absod_id = 1; 
        od_entry->absod_id = 0x13333337;
    } else {
        meta_od_entry->has_absod_id = 0;
    }

    if (NULL != od_index) {
        *od_index = idx;
    }

    return 0;
}

int h3m_object_move(h3mlib_ctx_t ctx, int od_index, int x, int y, int z)
{
    struct H3M_OD_ENTRY *od_entry = &ctx->h3m.od.entries[od_index];
    od_entry->header.x = x;
    od_entry->header.y = y;
    od_entry->header.z = z;
    return 0;
}

int h3m_object_set_owner(h3mlib_ctx_t ctx, int od_index, int owner)
{
    struct H3M_OD_ENTRY *od_entry = &ctx->h3m.od.entries[od_index];
    struct META_OD_ENTRY *meta_od_entry = &ctx->meta.od_entries[od_index];
    uint8_t *body = od_entry->body;

    // TODO: need to also diffrentiate between different sub-versions
    // of objects, e.g if a town has been customized with name, use
    // correct struct/offset
    switch (meta_od_entry->oa_type) {
    case META_OBJECT_DWELLING:
    case META_OBJECT_DWELLING_ABSOD:
    case META_OBJECT_LIGHTHOUSE:
    case META_OBJECT_RESOURCE_GENERATOR:
    case META_OBJECT_SHIPYARD:
        ((struct H3M_OD_BODY_STATIC_FLAGGED *)body)->owner = owner;     // Owner is first element - no worries about offset
        break;
    case META_OBJECT_RANDOM_HERO:
    case META_OBJECT_HERO:
        ((struct H3M_OD_BODY_DYNAMIC_HERO *)body)->owner = owner;       // Owner is first element - no worries about offset
        break;
    case META_OBJECT_TOWN:
        ((struct H3M_OD_BODY_DYNAMIC_TOWN *)body)->owner = owner;       // Owner is first element - no worries about offset
        break;
    default:
        return 1;
    }

    return 0;
}

int h3m_object_set_subtype(h3mlib_ctx_t ctx, int od_index, int subtype)
{
    struct H3M_OD_ENTRY *od_entry = &ctx->h3m.od.entries[od_index];
    struct META_OD_ENTRY *meta_od_entry = &ctx->meta.od_entries[od_index];
    uint8_t *body = od_entry->body;

    // TODO: need to also diffrentiate between different sub-versions
    // of objects, e.g if a town has been customized with name, use
    // correct struct/offset
    switch (meta_od_entry->oa_type) {
    case META_OBJECT_HERO:
        ((struct H3M_OD_BODY_DYNAMIC_HERO *)body)->type = subtype;
        break;
    case META_OBJECT_SPELL_SCROLL:
        ((struct H3M_OD_BODY_STATIC_SPELL_SCROLL *)body)->spell = subtype;
        break;
    default:
        return 1;
    }

    return 0;
}

int h3m_object_set_quantitiy(h3mlib_ctx_t ctx, int od_index, int quantity)
{
    struct H3M_OD_ENTRY *od_entry = &ctx->h3m.od.entries[od_index];
    struct META_OD_ENTRY *meta_od_entry = &ctx->meta.od_entries[od_index];
    uint8_t *body = od_entry->body;

    // TODO: need to also diffrentiate between different sub-versions
    // of objects, e.g if a town has been customized with name, use
    // correct struct/offset
    switch (meta_od_entry->oa_type) {
    case META_OBJECT_RESOURCE:
        ((struct H3M_OD_BODY_STATIC_RESOURCE *)body)->quantity = quantity;
        break;
    case META_OBJECT_MONSTER:
        ((struct H3M_OD_BODY_STATIC_MONSTER *)body)->quantity = quantity;
        break;
    default:
        return 1;
    }

    return 0;
}

int h3m_object_set_disposition(h3mlib_ctx_t ctx, int od_index,
    enum H3M_DISPOSITION disposition)
{
    struct H3M_OD_ENTRY *od_entry = &ctx->h3m.od.entries[od_index];
    struct META_OD_ENTRY *meta_od_entry = &ctx->meta.od_entries[od_index];
    uint8_t *body = od_entry->body;

    if (META_OBJECT_MONSTER != meta_od_entry->oa_type) {
        return 1;
    }

    ((struct H3M_OD_BODY_STATIC_MONSTER *)body)->disposition = disposition;

    return 0;
}

static int _set_creatures(h3mlib_ctx_t ctx, int od_index, int *types,
    int *quantities)
{
    extern int creature_names_hash(const char *);
    struct H3M_OD_ENTRY *od_entry = &ctx->h3m.od.entries[od_index];
    struct META_OD_ENTRY *meta_od_entry = &ctx->meta.od_entries[od_index];
    uint8_t *body = od_entry->body;
    //size_t body_size = 0;
    //int binary_compatible;
    struct H3M_OD_BODY_DYNAMIC_HERO *hero_body = NULL;
    union H3M_COMMON_ARMY *creatures = NULL;
    int i = 0;

    switch (meta_od_entry->oa_type) {
    //case META_OBJECT_HERO:
    //    hero_body->owner = calloc
    //    hero_body->owner = ((struct H3M_OD_BODY_DYNAMIC_HERO *)body)->owner;
    //    hero_body->type = ((struct H3M_OD_BODY_DYNAMIC_HERO *)body)->type;
    //    hero_body->has_creatures = 1;
    //    hero_body->patrol_radius = 0xFF;
    //    creatures = hero_body->creatures;
    //    body = (uint8_t *)hero_body;
    //    //body_size = sizeof(*hero_body);
    //    break;
    //    //case META_OBJECT_TOWN:
    //    //    break;
    case META_OBJECT_GARRISON:
    case META_OBJECT_GARRISON_ABSOD:
        creatures = (union H3M_COMMON_ARMY *)
            (&((union H3M_OD_BODY_STATIC_GARRISON *)body)->any.creatures);
        break;
    default:
        return 1;
    }

    if (H3M_FORMAT_ROE == ctx->h3m.format) {
        for (i = 0; i < 7; ++i) {
            creatures->roe.slots[i].type = types[i];
            creatures->roe.slots[i].quantity = quantities[i];
        }
    } else {
        for (i = 0; i < 7; ++i) {
            creatures->absod.slots[i].type = types[i];
            creatures->absod.slots[i].quantity = types[i];
        }
    }

    od_entry->body = body;
    //meta_od_entry->body_size = body_size;
    //meta_od_entry->binary_compatible = binary_compatible;

    return 0;
}

int h3m_object_fill_random_creatures(h3mlib_ctx_t ctx, int od_index)
{
    int i;
    int types[7];
    int quantities[7];

    for (i = 0; i < 7; ++i) {
        types[i] = (rand() + 1) % 117; // TODO better way of randoming creatures
        quantities[i] = (rand() + 1) % 1000;
    }

    return _set_creatures(ctx, od_index, types, quantities);
}

int h3m_object_set_creatues(h3mlib_ctx_t ctx, int od_index, const char **names,
    int *quantities)
{
    extern int creature_names_hash(const char *);
    int i;
    int types[7];

    for (i = 0; i < 7; ++i) {
        if (NULL == names[i] || 0 == strlen(names[i])) {
            types[i] = 0xFF;
            continue;
        }

        types[i] = creature_names_hash(names[i]);
    }

    return _set_creatures(ctx, od_index, types, quantities);
}

int h3m_object_add_by_def(h3mlib_ctx_t ctx, const char *def, int x, int y,
    int z, int *od_index)
{
    struct META_OA_HASH_ENTRY *oa_hash_entry = NULL;
    int oa_index = 0;
    int oa_body_index = 0;

    // Currently oa_body_index is used as key in the
    // oa hash... The current code is a bit unoptimal as if this function
    // ends up calling h3m_add_oa_by_def below, an additional
    // hash lookup for this hash inside that function will be performed.
    if (NULL == h3m_get_def_body(def, &oa_body_index)) {
        return 1;
    }
    // if oa for this object has not yet been added to map, add it,
    // otherwise use the existing oa_index for the oa
    HASH_FIND_INT(ctx->meta.oa_hash, &oa_body_index, oa_hash_entry);
    if (NULL == oa_hash_entry) {
        h3m_add_oa_by_def(ctx, def, &oa_index);
    } else {
        oa_index = oa_hash_entry->oa_index;
    }

    return h3m_add_od(ctx, oa_index, x, y, z, od_index);
}

int h3m_alg_od_text(h3mlib_ctx_t ctx, const char *def, int oa_index, int x,
    int y, int z, const char *text)
{
    return h3mlib_alg_od_text(ctx, def, oa_index, x, y, z, text);
}

int h3m_terrain_get_all(const h3mlib_ctx_t ctx, int z, uint8_t *terrain_types,
    size_t n)
{
    unsigned int count = ctx->h3m.bi.any.map_size * ctx->h3m.bi.any.map_size;
    unsigned int i = 0;

    if (0 != z) {
        count *= 2;
    }

    if (n < count) {
        return 1;
    }

    for (i = 0; i < count; ++i) {
        terrain_types[i] = ctx->h3m.tiles[i].terrain_type;
    }

    return 0;
}

void h3m_free(void *p)
{
    free(p);
}

int h3m_enum_defs(h3mlib_ctx_t ctx, h3m_enum_def_cb_t cb, void *cb_data)
{
    int ret = 0;
    unsigned int i = 0;
    struct H3M_OA_ENTRY *oa_entry = NULL;

    for (i = 0; i < ctx->h3m.oa.count; ++i) {
        oa_entry = &ctx->h3m.oa.entries[i];

        if (0 != (ret =
                cb((char *)oa_entry->header.def, (uint8_t *)& oa_entry->body,
                    cb_data))) {
            return ret;
        }
    }

    return 0;
}

enum META_OBJECT h3m_get_oa_type(h3mlib_ctx_t ctx, int oa_index)
{
    return ctx->meta.oa_entries[oa_index].type;
}

enum META_OBJECT h3m_get_object_type(const char *name)
{
    extern int def_types_hash(const char *);
    const char *def = _name_to_def(name);

    if (NULL == def) {
        return -1;
    }
    // TODO bounds check
    return def_types_hash(def);
}

enum H3M_FORMAT h3m_get_format(const h3mlib_ctx_t ctx)
{
    return ctx->h3m.format;
}

size_t h3m_get_map_size(const h3mlib_ctx_t ctx)
{
    return ctx->h3m.bi.any.map_size;
}

struct MERGE_CTX {
    size_t orig_oa_count;
};

int _merge_oa(h3mlib_ctx_t ctx_main, const h3mlib_ctx_t ctx_append,
    struct MERGE_CTX *merge)
{
    // -2 for the everpresent oa
    size_t new_oa_count = ctx_main->h3m.oa.count + ctx_append->h3m.oa.count - 2;
    size_t n = 0;
    unsigned int i = 0;
    struct H3M_OA_ENTRY *entry_main = NULL;
    struct H3M_OA_ENTRY *entry_append = NULL;
    int cur_main_index = ctx_main->h3m.oa.count;

    merge->orig_oa_count = ctx_main->h3m.oa.count;

    n = sizeof(*(ctx_main->h3m.oa.entries)) * new_oa_count;
    ctx_main->h3m.oa.entries = realloc(ctx_main->h3m.oa.entries, n);

    for (i = 0; i < ctx_append->h3m.oa.count - 2; ++i) {
        entry_main = &ctx_main->h3m.oa.entries[cur_main_index];
        entry_append = &ctx_append->h3m.oa.entries[i + 2];      // +2 for everpresent oa

        // Copying over into main means we take ownership of def name pointer
        memcpy(entry_main, entry_append, sizeof(*(entry_main)));
        entry_append->header.def = NULL;

        ++cur_main_index;
    }

    ctx_main->h3m.oa.count += i;

    return 0;
}

int _merge_od(h3mlib_ctx_t ctx_main, const h3mlib_ctx_t ctx_append,
    struct MERGE_CTX *merge)
{
    size_t new_od_count = ctx_main->h3m.od.count + ctx_append->h3m.od.count;
    size_t n = 0;
    unsigned int i = 0;
    int cur_main_index = ctx_main->h3m.od.count;
    struct H3M_OD_ENTRY *entry_main = NULL;
    struct H3M_OD_ENTRY *entry_append = NULL;
    struct META_OD_ENTRY *meta_main = NULL;
    struct META_OD_ENTRY *meta_append = NULL;
    int extra_oa_index = merge->orig_oa_count - 2;      //-2 for everpresent oa

    n = sizeof(*(ctx_main->h3m.od.entries)) * new_od_count;
    ctx_main->h3m.od.entries = realloc(ctx_main->h3m.od.entries, n);
    n = sizeof(*(ctx_main->meta.od_entries)) * new_od_count;
    ctx_main->meta.od_entries = realloc(ctx_main->meta.od_entries, n);

    for (i = 0; i < ctx_append->h3m.od.count; ++i) {
        entry_main = &ctx_main->h3m.od.entries[cur_main_index];
        meta_main = &ctx_main->meta.od_entries[cur_main_index];
        entry_append = &ctx_append->h3m.od.entries[i];
        meta_append = &ctx_append->meta.od_entries[i];

        memcpy(entry_main, entry_append, sizeof(*(entry_main)));
        memcpy(meta_main, meta_append, sizeof(*(meta_main)));

        // Copying over into main means we take ownership of some pointers
        memcpy(entry_main, entry_append, sizeof(*(entry_main)));
        memcpy(meta_main, meta_append, sizeof(*(meta_main)));
        entry_append->body = NULL;
        meta_append->dyn_pointers = NULL;

        // Fix oa index to point into the appended map's metadata
        entry_main->header.oa_index += extra_oa_index;

        ++cur_main_index;
    }

    ctx_main->h3m.od.count += i;

    return 0;
}

int h3m_object_patch(h3mlib_ctx_t ctx_main, const char *filename)
{
    struct MERGE_CTX merge = { 0 };
    h3mlib_ctx_t ctx_append = NULL;

    if (0 != h3m_read(&ctx_append, filename)) {
        return 1;
    }

    _merge_oa(ctx_main, ctx_append, &merge);
    _merge_od(ctx_main, ctx_append, &merge);

    h3m_exit(&ctx_append);

    return 0;
}

#if defined _WIN32 && defined _MSC_VER
int h3m_object_patch_u(h3mlib_ctx_t ctx_main, const wchar_t *filename)
{
    struct MERGE_CTX merge = { 0 };
    h3mlib_ctx_t ctx_append = NULL;

    if (0 != h3m_read_u(&ctx_append, filename)) {
        return 1;
    }

    _merge_oa(ctx_main, ctx_append, &merge);
    _merge_od(ctx_main, ctx_append, &merge);

    h3m_exit(&ctx_append);

    return 0;
}
#endif

int h3m_get_oa_index(h3mlib_ctx_t ctx, const char *def, uint32_t *oa_index)
{
    struct META_OA_HASH_ENTRY *oa_hash_entry = NULL;
    int oa_body_index = 0;

    if (NULL == h3m_get_def_body(def, &oa_body_index)) {
        return 1;
    }

    HASH_FIND_INT(ctx->meta.oa_hash, &oa_body_index, oa_hash_entry);
    if (NULL != oa_hash_entry) {
        *oa_index = oa_hash_entry->oa_index;
        return 0;
    }

    return 1;
}

int h3m_enum_od(h3mlib_ctx_t ctx, h3m_od_cb_t cb, void *cb_data)
{
    struct H3M_OD_ENTRY *od_entry = NULL;
    struct META_OD_ENTRY *meta_od_entry = NULL;
    int ret = 0;

    for (unsigned int i = 0; i < ctx->h3m.od.count; ++i) {
        od_entry = &ctx->h3m.od.entries[i];
        meta_od_entry = &ctx->meta.od_entries[i];

        if (0 != (ret = cb(&od_entry->header.x, &od_entry->header.y,
                    &od_entry->header.z, &od_entry->header.oa_index,
                    od_entry->body, meta_od_entry->body_size, cb_data))) {
            return ret;
        }
    }

    return 0;
}

int h3m_name_set(h3mlib_ctx_t ctx, const char *name)
{
    size_t n = strlen(name) + 1;
    ctx->h3m.bi.any.name = (NULL == ctx->h3m.bi.any.name) ?
        malloc(n + 1) : realloc(ctx->h3m.bi.any.name, n + 1);
    strncpy((char *)ctx->h3m.bi.any.name, name, n);
    ctx->h3m.bi.any.name_size = n;

    return 0;
}

int h3m_desc_set(h3mlib_ctx_t ctx, const char *desc)
{
    size_t n = strlen(desc);
    ctx->h3m.bi.any.desc = (NULL == ctx->h3m.bi.any.desc) ?
        malloc(n + 1) : realloc(ctx->h3m.bi.any.desc, n + 1);
    strncpy((char *)ctx->h3m.bi.any.desc, desc, n);
    ctx->h3m.bi.any.desc_size = n;

    return 0;
}

int h3m_desc_append(h3mlib_ctx_t ctx, const char *desc)
{
    size_t n = ctx->h3m.bi.any.desc_size + strlen(desc);
    ctx->h3m.bi.any.desc = realloc(ctx->h3m.bi.any.desc, n + 1);
    sprintf((char *)&ctx->h3m.bi.any.desc[ctx->h3m.bi.any.desc_size], "%s",
        desc);
    ctx->h3m.bi.any.desc_size = n;

    return 0;
}

int h3m_towns_selectable(h3mlib_ctx_t ctx)
{
    for (unsigned int i = 0; i < H3M_MAX_PLAYERS; ++i) {
        ctx->h3m.players[i]->roe.town_types = 0xFF;
        ctx->h3m.players[i]->roe.unknown1 = 1;
    }

    return 0;
}

int h3m_get_filename_from_gm1(const char *filename_gm1, char *filename_h3m,
    size_t n)
{
    unsigned char *buf = NULL;
    long buf_size = 0;
    size_t len = 0;
    unsigned char *loc = NULL;

    if (0 != gu_decompress_file_to_mem(filename_gm1, (void **)&buf, &buf_size)) {
        return 1;
    }

    if (NULL == (loc = memmem(buf, (size_t)buf_size, (uint8_t *)".h3m", sizeof(".h3m") - 1))) {
        gu_free(buf);
        return 2;
    }

    while (((isalnum(*(loc - 1))) || (isspace(*(loc - 1)))
            || (ispunct(*(loc - 1)))
            || ('(' == *(loc - 1)) || ('(' == *(loc - 1))
            || ('_' == *(loc - 1)))
        && loc > buf) {
        --loc;
    }

    if (n <= (len = strlen((char *)loc))) {
        gu_free(buf);
        return 3;
    }

    strcpy(filename_h3m, (char *)loc);

    gu_free(buf);

    return 0;
}

enum META_OBJECT h3m_object_from_category(const char *object_category)
{
    extern int h3m_object_hash(const char *);
    int type = h3m_object_hash(object_category);

    if (type > META_GREATEST) {
        return -1;
    } else if (0 != strcmp(META_OBJECT_NAME[type], object_category)) {
        return -1;
    }

    return type;
}
