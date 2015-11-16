// Created by John Åkerblom 2014-11-18

#ifndef __H3MLIB_H_DEF__
#define __H3MLIB_H_DEF__

#ifdef __cplusplus
extern "C" {
#endif

#include <stddef.h>             // size_t
#include "msvc_comp_stdint.h"
#include "h3m_object.h"

#define H3MLIB_INTERRUPT_CB_NO_CLEANUP 0x13333337
#define H3MLIB_TERRAIN_NATIVE -1

#define H3M_SIZE_SMALL 36
#define H3M_SIZE_MEDIUM 72
#define H3M_SIZE_LARGE 108
#define H3M_SIZE_EXTRALARGE 144
#define H3M_MAX_SIZE H3M_SIZE_EXTRALARGE
#define H3M_MAX_PLAYERS 8
#define H3M_MAX_HERO_NAME 12
#define H3M_2D_TO_1D(s, x, y, z) (x + (y * s) + (z * s * s))

    enum H3M_FORMAT {
        H3M_FORMAT_ROE = 0x0000000E,
        H3M_FORMAT_AB = 0x00000015,
        H3M_FORMAT_SOD = 0x0000001C,
        H3M_FORMAT_WOG = 0x00000033
    };

    enum H3M_TERRAIN {
        H3M_TERRAIN_DIRT,
        H3M_TERRAIN_SAND,
        H3M_TERRAIN_GRASS,
        H3M_TERRAIN_SNOW,
        H3M_TERRAIN_SWAMP,
        H3M_TERRAIN_ROUGH,
        H3M_TERRAIN_SUBTERRANEAN,
        H3M_TERRAIN_LAVA,
        H3M_TERRAIN_WATER,
        H3M_TERRAIN_ROCK
    };

    enum H3M_DISPOSITION {
        H3M_DISPOSITION_COMPLIANT,      // will always join hero
        H3M_DISPOSITION_FRIENDLY,       // likely to join hero
        H3M_DISPOSITION_AGGRESSIVE,     // may join hero
        H3M_DISPOSITION_HOSTILE,        // unlikely to join hero
        H3M_DISPOSITION_SAVAGE          // will never join hero
    };

    enum H3M_CODE_TARGET {
        H3M_CODE_TARGET_COMPLETE,   // Heroes3.exe 78956DFAB3EB8DDF29F6A84CF7AD01EE
        H3M_CODE_TARGET_HDMOD,      // Heroes3 HD.exe 56614D31CC6F077C2D511E6AF5619280
        H3M_CODE_TARGET_DEMO        // h3demo.exe 522B6F45F534058D02A561838559B1F4
    };

    struct H3MLIB_CTX;
    typedef struct H3MLIB_CTX *h3mlib_ctx_t;

    /* Basic API */

    int h3m_init_min(h3mlib_ctx_t *ctx, uint32_t format, int size);    // inits water, no underground map
    int h3m_read(h3mlib_ctx_t *ctx, const char *filename);
    int h3m_write(h3mlib_ctx_t ctx, const char *filename);
    //int h3m_write_compressed(const h3mlib_ctx_t ctx, const char *filename);
    int h3m_object_patch(h3mlib_ctx_t ctx, const char *filename);
    int h3m_exit(h3mlib_ctx_t *ctx);
    void h3m_free(void *p); // Used to free pointers allocated by h3mlib. h3m_exit() is used for h3mlib_ctx_t

    int h3m_compress(const char *input_filename, const char *output_filename);
    int h3m_decompress(const char *input_filename, const char *output_filename);

    /* Basic utilities */

    int h3m_name_set(h3mlib_ctx_t ctx, const char *desc);
    int h3m_desc_set(h3mlib_ctx_t ctx, const char *desc);
    int h3m_desc_append(h3mlib_ctx_t ctx, const char *desc);
    int h3m_towns_selectable(h3mlib_ctx_t ctx); // Sets all player towns to selectable, AKA Random Towns
    void h3m_player_enable(h3mlib_ctx_t ctx, int player);

    enum H3M_FORMAT h3m_get_format(const h3mlib_ctx_t ctx);
    size_t h3m_get_map_size(const h3mlib_ctx_t ctx);
    int h3m_get_filename_from_gm1(const char *filename_gm1, char *filename_h3m,
        size_t n);

    /* Basic object functions */

    // This function should be passed names such as "Archangel", "Inferno" etc
    int h3m_object_add(h3mlib_ctx_t ctx, const char *name, int x, int y, int z,
        int *od_index);
    int h3m_object_move(h3mlib_ctx_t ctx, int od_index, int x, int y, int z);
    int h3m_object_text(h3mlib_ctx_t ctx, const char *name, int x, int y, int z,
        const char *text);
    int h3m_object_set_owner(h3mlib_ctx_t ctx, int od_index, int owner);
    int h3m_object_set_subtype(h3mlib_ctx_t ctx, int od_index, int subtype);
    int h3m_object_set_quantitiy(h3mlib_ctx_t ctx, int od_index, int quantity);
    int h3m_object_set_disposition(h3mlib_ctx_t ctx, int od_index,
        enum H3M_DISPOSITION disposition);
    // Expects array of 7 names and 7 quantities. NULL name means empty slot.
    int h3m_object_set_creatues(h3mlib_ctx_t ctx, int od_index,
        const char **names, int *quantities);
    int h3m_object_fill_random_creatures(h3mlib_ctx_t ctx, int od_index);

    /* Basic terrain functions */

    int h3m_terrain_fill(const h3mlib_ctx_t ctx, enum H3M_TERRAIN);
    int h3m_terrain_get_all(const h3mlib_ctx_t ctx, int z,
        uint8_t *terrain_types, size_t n);
    // Currently better to use only one terrain_set_all call rather than several terrain_set calls
    int h3m_terrain_set(const h3mlib_ctx_t ctx, int x, int y, int z,
        enum H3M_TERRAIN terrain);
    int h3m_terrain_set_all(const h3mlib_ctx_t ctx, int z,
        uint8_t *terrain_types);
    // Functionality like map edtiors "Obstacle Tool". impassable is an array of size map_size * map_size
    // where each byte represents one byte, with 0 signifying passable and non-zero signifying impasssable
    void h3m_impassable_fill(h3mlib_ctx_t ctx, uint8_t *impassable,
        size_t map_size);

    /* Advanced API */

    typedef int (*h3m_parse_cb_t) (uint32_t offset, const char *member, void *p,
        size_t n, void *cb_data);
    typedef int (*h3m_error_cb_t) (const char *error, void *cb_data);
    typedef enum H3M_OBJECT (*h3m_custom_def_cb_t) (const char *def,
        void *cb_data);
    int h3m_read_with_cbs(h3mlib_ctx_t *ctx, const char *filename,
        h3m_parse_cb_t cb_parse, h3m_error_cb_t cb_error,
        h3m_custom_def_cb_t cb_def, void *cb_data);

    int h3m_read_convert(h3mlib_ctx_t *ctx,
        const char *filename,
        enum H3M_FORMAT target_format,
        enum H3M_FORMAT *source_format,
        h3m_parse_cb_t cb_parse,
        h3m_error_cb_t cb_error, h3m_custom_def_cb_t cb_def, void *cb_data);

    typedef int (*h3m_enum_def_cb_t) (const char *def, uint8_t *body,
        void *cb_data);
    int h3m_enum_defs(h3mlib_ctx_t ctx, h3m_enum_def_cb_t cb, void *cb_data);

    int h3m_get_oa_index(h3mlib_ctx_t ctx, const char *def,
        uint32_t *oa_index);

    typedef int (*h3m_od_cb_t) (uint8_t *x, uint8_t *y, uint8_t *z,
        uint32_t *oa_index, uint8_t *data, size_t data_size, void *cb_data);
    int h3m_enum_od(h3mlib_ctx_t ctx, h3m_od_cb_t cb, void *cb_data);

    void h3m_active_oa_mark(h3mlib_ctx_t ctx, int x, int y, int z, int oa_index,
        uint8_t *active, size_t map_size);
    void h3m_impassable_oa_mark(h3mlib_ctx_t ctx, int x, int y, int z,
        int oa_index, uint8_t *impassable, size_t map_size);
    void h3m_impassable_mark(h3mlib_ctx_t ctx, int x, int y, int z,
        uint64_t passability, uint8_t *impassable, size_t map_size);

    int h3m_add_obstacle(h3mlib_ctx_t ctx, int x, int y, int z, int x_max_dim,
        int y_max_dim, int terrain, int *x_dim, int *y_dim, int *od_index,
        uint64_t * passability_out);
    int h3m_add_obstacle_with_passability(h3mlib_ctx_t ctx, int x, int y, int z,
        int x_max_dim, int y_max_dim, int terrain, uint64_t passability_in,
        int *od_index);

    typedef int (*passability_cb_t) (int x_impassable, int y_impassable,
        int z_impassable, void *cb_data);
    int h3m_analyze_passability(uint64_t passability, int x_bottom_right,
        int y_bottom_right, int z, int *x_dim, int *y_dim, passability_cb_t cb,
        void *cb_data);

    enum H3M_OBJECT h3m_get_oa_type(h3mlib_ctx_t ctx, int oa_index);
    enum H3M_OBJECT h3m_get_object_type(const char *name);
    enum H3M_OBJECT h3m_object_from_category(const char *object_category);

    // Low-level object functions, see h3m_object_add for easier to user function
    int h3m_add_oa_by_def(h3mlib_ctx_t ctx, const char *def, int *oa_index);
    int h3m_add_oa_by_name(h3mlib_ctx_t ctx, const char *name, int *oa_index);
	struct H3M_OA_ENTRY; // defined in h3m_structures/object_attributes/h3m_oa.h
    int h3m_add_oa_entry(h3mlib_ctx_t ctx, struct H3M_OA_ENTRY *oa_entry,
        int *oa_index);
    int h3m_add_od(h3mlib_ctx_t ctx, int oa_index, int x, int y, int z,
        int *od_index);
    int h3m_object_add_by_def(h3mlib_ctx_t ctx, const char *def, int x, int y,
        int z, int *od_index);

    const struct H3M_OA_BODY *h3m_get_def_body(const char *def,
        int *oa_body_index);

    int h3m_alg_od_text(h3mlib_ctx_t ctx, const char *def, int oa_index, int x,
        int y, int z, const char *text);

    int h3m_generate_tiles(const h3mlib_ctx_t ctx, int size, int z,
        uint8_t *terrain_types, uint8_t *road_types, uint8_t *river_types);

    // Functions for packing a DLL that is executed when map loads
    int h3m_code_set_dll(h3mlib_ctx_t ctx, const char *dll);
    int h3m_code_set_target(h3mlib_ctx_t ctx, enum H3M_CODE_TARGET target);
    int h3m_code_unset(h3mlib_ctx_t ctx);

/* Unicode API */
#if defined _WIN32 && defined _MSC_VER
    int h3m_read_u(h3mlib_ctx_t *ctx, const wchar_t *filename);

    int h3m_read_with_cbs_u(h3mlib_ctx_t *ctx,
        const wchar_t *filename,
        h3m_parse_cb_t cb_parse,
        h3m_error_cb_t cb_error, h3m_custom_def_cb_t cb_def, void *cb_data);

    int h3m_read_convert_u(h3mlib_ctx_t *ctx,
        const wchar_t *filename,
        enum H3M_FORMAT target_format,
        enum H3M_FORMAT *source_format,
        h3m_parse_cb_t cb_parse,
        h3m_error_cb_t cb_error, h3m_custom_def_cb_t cb_def, void *cb_data);

    int h3m_write_u(h3mlib_ctx_t ctx, const wchar_t *filename);

    int h3m_object_patch_u(h3mlib_ctx_t ctx, const wchar_t *filename);
#endif

#ifdef __cplusplus
}
#endif
#endif
