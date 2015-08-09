// Created by John Åkerblom 2014-11-16
#pragma once

#include "crossplatform.h"
#ifdef __cplusplus
extern "C"
{
#endif

DLL_PUBLIC void hlm_init_defaults(int size, int underground);
DLL_PUBLIC void hlm_init(int tile_size, int has_two_levels);
DLL_PUBLIC void hlm_exit(void);

DLL_PUBLIC int hlm_xyz_set_terrain(int x, int y, int z, int terrain);
DLL_PUBLIC int hlm_xyz_set_road(int x, int y, int z, int road);
DLL_PUBLIC int hlm_xyz_set_river(int x, int y, int z, int river);
DLL_PUBLIC int hlm_xyz_clear_obstacles(int x, int y, int z);

DLL_PUBLIC int hlm_xyz2_set_terrain(int x1, int y1, int x2, int y2, int z, int terrain);
DLL_PUBLIC int hlm_xyz2_set_road(int x1, int y1, int x2, int y2, int z, int road);
DLL_PUBLIC int hlm_xyz2_set_river(int x1, int y1, int x2, int y2, int z, int river);

DLL_PUBLIC int hlm_xyz2_clear_obstacles_rect(int x1, int y1, int x2, int y2, int z);

DLL_PUBLIC int hlm_xyz_add_object(int x, int y, int z, const char *name);

DLL_PUBLIC int hlm_last_set_owner(int owner);
DLL_PUBLIC int hlm_last_set_quantity(int quantity);
DLL_PUBLIC int hlm_last_set_disposition(int disposition);
DLL_PUBLIC int hlm_last_set_creatures(int slot, const char *name, int quantity);
DLL_PUBLIC int hlm_last_set_random_creatures(void);

DLL_PUBLIC int hlm_open_h3m(const char *filename);
DLL_PUBLIC int hlm_save_h3m(const char *filename);

DLL_PUBLIC void hlm_fill_terrain(int z, int terrain);
DLL_PUBLIC void hlm_fill_obstacles(int z);
DLL_PUBLIC int hlm_xyz_gen_island_terrain(int x, int y, int z, int terrain, int perimeter);

DLL_PUBLIC int hlm_mirror(int z, int destination);

#ifdef __cplusplus
}
#endif
