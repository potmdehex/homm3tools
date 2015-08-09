// Created by John Åkerblom 2014-11-16

#include "c_interface.h"
#include "hlm.h"

#include <memory>

static std::unique_ptr<Hlm> hmap;

void hlm_init_defaults(int size, int underground)
{
    hmap = std::make_unique<Hlm>(size, false);
}

void hlm_init(int tile_size, int has_two_levels)
{
    hmap = std::make_unique<Hlm>(tile_size, has_two_levels ? true : false);
}

void hlm_exit(void)
{
    hmap.reset(nullptr);
}

int hlm_xyz_set_terrain(int x, int y, int z, int terrain)
{
    return hmap->xyzSetTerrain(x, y, z, (Terrain)terrain);
}

int hlm_xyz_set_road(int x, int y, int z, int road)
{
    return hmap->xyzSetRoad(x, y, z, (Road)road);
}

int hlm_xyz_set_river(int x, int y, int z, int river)
{
    return hmap->xyzSetRiver(x, y, z, (River)river);
}

int hlm_xyz_clear_obstacles(int x, int y, int z)
{
    return hmap->xyzClearObstacles(x, y, z);
}

int hlm_xyz2_set_terrain(int x1, int y1, int x2, int y2, int z, int terrain)
{
    return hmap->xyz2SetTerrain(x1, y1, x2, y2, z, (Terrain)terrain);
}

int hlm_xyz2_set_road(int x1, int y1, int x2, int y2, int z, int road)
{
    return hmap->xyz2SetRoad(x1, y1, x2, y2, z, (Road)road);
}

int hlm_xyz2_set_river(int x1, int y1, int x2, int y2, int z, int river)
{
    return hmap->xyz2SetRiver(x1, y1, x2, y2, z, (River)river);
}

int hlm_xyz2_clear_obstacles_rect(int x1, int y1, int x2, int y2, int z)
{
    return hmap->xyz2ClearObstaclesRect(x1, y1, x2, y2, z);
}

int hlm_xyz_add_object(int x, int y, int z, const char *name)
{
    return hmap->xyzAddObject(x, y, z, std::string(name));
}

int hlm_last_set_owner(int owner)
{
    return hmap->lastSetOwner(owner);
}

int hlm_last_set_quantity(int quantity)
{
    return hmap->lastSetQuantity(quantity);
}

int hlm_last_set_disposition(int disposition)
{
    return hmap->lastSetDisposition((Disposition)disposition);
}

int hlm_last_set_creatures(int slot, const char *name, int quantity)
{
    return hmap->lastSetCreatures(slot, std::string(name), quantity);
}

int hlm_last_set_random_creatures(void)
{
    return hmap->lastSetRandomCreatures();
}

int hlm_open_h3m(const char *filename)
{
    return 1;
}

int hlm_save_h3m(const char *filename)
{
    return hmap->saveH3M(filename);
}

void hlm_fill_terrain(int z, int terrain)
{
    return hmap->fillTerrain(z, (Terrain)terrain);
}

void hlm_fill_obstacles(int z)
{
    return hmap->fillObstacles(z);
}

int hlm_xyz_gen_island_terrain(int x, int y, int z, int terrain, int perimeter)
{
    return hmap->xyzGenIslandTerrain(x, y, z, (Terrain)terrain, perimeter);
}

DLL_PUBLIC int hlm_mirror(int z, int destination)
{
    return hmap->mirrorNW(z, (Corner)destination);
}
