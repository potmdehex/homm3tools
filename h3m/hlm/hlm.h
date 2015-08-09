// Created by John Åkerblom 2014-11-16
#pragma once

#include <h3mlib.h> // H3M_MAX_SIZE

#include "map_objects/map_object.h"
#include "map_objects/map_artifact.h"
#include "map_objects/map_generic.h"
#include "map_objects/map_hero.h"
#include "map_objects/map_monster.h"
#include "map_objects/map_resource.h"
#include "map_objects/map_town.h"

#include <array>
#include <memory>
#include <string>
#include <vector>

enum class Terrain
{
    DIRT,
    SAND,
    GRASS,
    SNOW,
    SWAMP,
    ROUGH,
    SUBTERRANEAN,
    LAVA,
    WATER,
    ROCK
};

enum class Road
{
    DIRT,
    GRAVEL,
    COBBLESTONE
};

enum class River
{
    CLEAR,
    ICY,
    MUDDY,
    LAVA
};

enum class Corner
{
    ALL = -1,
    NW,
    NE,
    SW,
    SE
};

template<typename T, size_t N>
using array2d = std::array<std::array<T, N>, N>;

class Hlm
{
public:
    Hlm(size_t tile_size, bool has_underground)
        : m_size(tile_size),
        m_has_underground(has_underground),
        m_obstacles(),
        m_terrain(),
        m_rivers(),
        m_roads()
    { }

    // Sets all terrain at level z
    void fillTerrain(int z, Terrain terrain);

    // Equivalent to using map editor's obstacle tool on entire map
    void fillObstacles(int z);

    void genHBWang();

    int xyzGenIslandTerrain(int x, int y, int z, Terrain terrain, int perimeter);

    // For setting a single point
    int xyzSetTerrain(int x, int y, int z, Terrain terrain);
    int xyzSetRiver(int x, int y, int z, River river);
    int xyzSetRoad(int x, int y, int z, Road road);

    int xyzClearObstacles(int x, int y, int z);

    // For forming horizontal, vertical or diagonal lines between two points
    int xyz2SetTerrain(int x1, int y1, int x2, int y2, int z, Terrain terrain);
    int xyz2SetRiver(int x1, int y1, int x2, int y2, int z, River river);
    int xyz2SetRoad(int x1, int y1, int x2, int y2, int z, Road road);

    // x1, y1 left and top, x2, y2 right and bottom of rect to clear
    int xyz2ClearObstaclesRect(int x1, int y1, int x2, int y2, int z);

    int xyzAddObject(int x, int y, int z, const std::string& name);
    void xyzAddTown(int x, int y, int z, MapTown town);

    // Generates an island at a random location, keeping distance from islands 
    // previously generated with this function
    int randGenIslandTerrain(Terrain terrain, int perimeter);

    // These functions operate on the last added object
    int lastSetOwner(int owner);
    int lastSetQuantity(int quantity);
    int lastSetDisposition(Disposition disposition);
    int lastSetCreatures(int slot, const std::string& name, int quantity);
    int lastSetRandomCreatures();

    int openH3M(const std::string& filename);
    int openHlm(const std::string& filename);
    int saveH3M(const std::string& filename);
    int saveHlm(const std::string& filename);

    int mirrorNW(int z, Corner destination);

    // tile width/height of map
    size_t m_size;

    bool m_has_underground;

    std::weak_ptr<MapObject> m_last;

    // This vector contains weak pointers to all objects
    std::vector<std::weak_ptr<MapObject> > m_map_objects;

    std::vector<std::shared_ptr<MapGeneric> > m_map_generic;
    std::vector<std::shared_ptr<MapArtifact> > m_map_artifacts;
    std::vector<std::shared_ptr<MapHero> > m_map_heroes;
    std::vector<std::shared_ptr<MapMonster> > m_map_monsters;
    std::vector<std::shared_ptr<MapResource> > m_map_resources;
    std::vector<std::shared_ptr<MapTown> > m_map_towns;

    // Similar to "Obstacle Tool" in map editor, these arrays mark tiles where
    // random obstacles are to be placed
    std::array<array2d<bool, H3M_MAX_SIZE>, 2> m_obstacles;

    // Terrains, roads, rivers
    std::array<array2d<Terrain, H3M_MAX_SIZE>, 2> m_terrain;
    std::array<array2d<River, H3M_MAX_SIZE>, 2> m_rivers;
    std::array<array2d<Road, H3M_MAX_SIZE>, 2> m_roads;
};
