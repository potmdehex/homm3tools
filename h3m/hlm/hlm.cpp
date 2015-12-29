// Created by John Åkerblom 2014-11-16

#include "hlm.h"
#include "conv/h3m_to_hlm.h"
#include "conv/hlm_to_h3m.h"
#include "algorithms/hbwang_impl.h"

#include <algorithm>
#include <cmath>
#include <fstream>
#include <memory>
#include <string>

// Used by fillTerrain, fillObstacles
template<typename T1, typename T2>
void fill2darray(T1& a, size_t size, T2 val)
{
    for (size_t i = 0; i < size; ++i)
    {
        a[i].fill(val);
    }
}

void Hlm::fillTerrain(int z, Terrain terrain)
{
    fill2darray(m_terrain[z], m_size, terrain);
}

void Hlm::fillObstacles(int z)
{
    fill2darray(m_obstacles[z], m_size, true);
    //Debugtest
    genHBWang();
}

int Hlm::xyzGenIslandTerrain(int x, int y, int z, Terrain terrain, int perimeter)
{
    return 1;
}

// Used by xyzSetTerrain, xyzSetRoad, xyzSetRiver
template<typename T1, typename T2>
int safeTileArrayWrite(int x, int y, int z, T1& a, T2 val)
{
    try {
        a.at(z).at(x).at(y) = val;
    }
    catch (std::out_of_range e) {
        return 1;
    }

    return 0;
}

// Used by xyz2SetTerrain, xyz2SetRoad, xyz2SetRiver
template<typename T1, typename T2>
int safeTileArrayWrite(int x1, int y1, int x2, int y2, int z, T1& a, T2 val, bool prevent_recurse = false)
{
    int x_diff = std::abs(x1 - x2);
    int y_diff = std::abs(y1 - y2);
    // If there is both an x_diff and y_diff they have to be equal so they form a diagonal
    // until there is logic for making anything but straight lines in this function
    if ((0 != x_diff && 0 != y_diff) && (x_diff != y_diff)) {
        return 1;
    }
    int x_inc = (x1 == x2)? 0 : (x1 < x2)? 1 : -1;
    int y_inc = (y1 == y2)? 0 : (y1 < y2)? 1 : -1;
    int ret;
    
    // Diagonals consist of vertical tile pairs, therefor an additional call at y-1 is needed for them.
    // Every diagonal road/river created with the map editor for example also has a road/river tile above each tile.
    // The conditions for terrain is a bit different, but it uses the same code for now 
    if (!prevent_recurse && 0 != x_inc && 0 != y_inc) {
        if (0 != (ret = safeTileArrayWrite(x1, y1 - 1, x2, y2 - 1, z, a, val, true))) {
            return ret;
        }
    }

    // Only need bounds checking for the extreme points, rest is fine if they are
    try {
        a.at(z).at(x1).at(y1) = val;
        a.at(z).at(x2).at(y2) = val;
    }
    catch (std::out_of_range e) {
        return 1;
    }

    // Set all other points apart from first and last without bounds checking
    for (int i = 1; i < std::max(y_diff, x_diff); ++i) {
        a[z][x1 + (x_inc * i)][y1 + (y_inc * i)] = val;
    }

    return 0;
}

int Hlm::xyzSetTerrain(int x, int y, int z, Terrain terrain)
{
    return safeTileArrayWrite(x, y, z, m_terrain, terrain);
}

int Hlm::xyzSetRiver(int x, int y, int z, River river)
{
    return safeTileArrayWrite(x, y, z, m_rivers, river);
}

int Hlm::xyzSetRoad(int x, int y, int z, Road road)
{
    return safeTileArrayWrite(x, y, z, m_roads, road);
}

int Hlm::xyzClearObstacles(int x, int y, int z)
{
     return safeTileArrayWrite(x, y, z, m_obstacles, false);
}

int Hlm::xyz2SetTerrain(int x1, int y1, int x2, int y2, int z, Terrain terrain)
{
    return safeTileArrayWrite(x1, y1, x2, y2, z, m_terrain, terrain);
}

int Hlm::xyz2SetRoad(int x1, int y1, int x2, int y2, int z, Road road)
{
    return safeTileArrayWrite(x1, y1, x2, y2, z, m_roads, road);
}

int Hlm::xyz2SetRiver(int x1, int y1, int x2, int y2, int z, River river)
{
    return safeTileArrayWrite(x1, y1, x2, y2, z, m_rivers, river);
}

int Hlm::xyz2ClearObstaclesRect(int x1, int y1, int x2, int y2, int z)
{
    int x_diff = std::abs(x1 - x2) + 1;
    int y_diff = std::abs(y1 - y2) + 1;
    int x_inc = (x1 == x2) ? 0 : (x1 < x2) ? 1 : -1;
    int y_inc = (y1 == y2) ? 0 : (y1 < y2) ? 1 : -1;
    auto& a = m_obstacles;

    // Only need bounds checking for the extreme points, rest is fine if they are
    try {
        a.at(z).at(x1).at(y1) = false;
        a.at(z).at(x2).at(y2) = false;
    }
    catch (std::out_of_range e) {
        return 1;
    }

    if (1 == x_diff && 1 == y_diff) {
        return 0;
    }

    for (int x = x_inc, y = 0; y < y_diff; ((x+= x_inc)> x_diff)? x = 0, (y += y_inc) : 1) {
        a[z][x1+x][y1+y] = false;
    }

    return 0;
}

template< typename T>
std::weak_ptr<MapObject> addObject(T& v, int x, int y, int z, const std::string& name)
{
    auto sp = std::make_shared<typename T::value_type::element_type>(x, y, z, name);
    v.push_back(sp);
    return std::weak_ptr<MapObject>(sp);
}

int Hlm::xyzAddObject(int x, int y, int z, const std::string& name)
{
    std::weak_ptr<MapObject> wp;
    enum META_OBJECT type = h3m_get_object_type(name.c_str());

    // Init object and add to corresponding vector, inferring object type from the 
    // vector passed to addObject.
    switch (type)
    {
    case META_OBJECT_RANDOM_HERO:
    case META_OBJECT_HERO:
        wp = addObject(m_map_heroes, x, y, z, name);
        break;
    case META_OBJECT_GENERIC_VISITABLE:
        wp = addObject(m_map_generic, x, y, z, name);
        break;
    case META_OBJECT_MONSTER:
        wp = addObject(m_map_monsters, x, y, z, name);
        break;
    default:
        return 1;
    }

    m_map_objects.push_back(wp);
    m_last = wp;

    return 0;
}

void Hlm::xyzAddTown(int x, int y, int z, MapTown town)
{

}

int Hlm::lastSetOwner(int owner)
{
    auto sp = m_last.lock();

    if (!sp) {
        return 1;
    }

    switch (sp->m_type)
    {
    case MapObjectType::HERO:
        reinterpret_cast<MapHero *>(sp.get())->m_owner = (Owner)owner;
        break;
    case MapObjectType::TOWN:
        reinterpret_cast<MapTown *>(sp.get())->m_owner = (Owner)owner;
        break;
    }

    return 0;
}

int Hlm::lastSetQuantity(int quantity)
{
    auto sp = m_last.lock();

    if (!sp) {
        return 1;
    }

    switch (sp->m_type)
    {
    case MapObjectType::MONSTER:
        reinterpret_cast<MapMonster *>(sp.get())->m_quantity = quantity;
        break;
    default:
        return 1;
    }

    return 0;
}

int Hlm::lastSetDisposition(Disposition disposition)
{
    auto sp = m_last.lock();

    if (!sp) {
        return 1;
    }

    switch (sp->m_type)
    {
    case MapObjectType::MONSTER:
        reinterpret_cast<MapMonster *>(sp.get())->m_disposition = disposition;
        break;
    default:
        return 1;
    }

    return 0;
}

int Hlm::lastSetCreatures(int slot, const std::string& name, int quantity)
{
    auto sp = m_last.lock();

    if (!sp) {
        return 1;
    }

    switch (sp->m_type)
    {
    case MapObjectType::HERO:
        reinterpret_cast<MapHero *>(sp.get())->m_creatures[slot].name = name;
        reinterpret_cast<MapHero *>(sp.get())->m_creatures[slot].quantity = quantity;
        reinterpret_cast<MapHero *>(sp.get())->m_has_creatures = true;
        break;
    //case MapObjectType::TOWN:
    //    break;
    default:
        return 1;
    }

    return 0;
}

int Hlm::lastSetRandomCreatures()
{
    auto sp = m_last.lock();

    if (!sp) {
        return 1;
    }

    switch (sp->m_type)
    {
    case MapObjectType::HERO:
        reinterpret_cast<MapHero *>(sp.get())->m_has_random_creatures = true;
        break;
    //case MapObjectType::TOWN:
    //    break;
    default:
        return 1;
    }

    return 0;
}

int Hlm::openH3M(const std::string& filename)
{
    return 1;
}

int Hlm::openHlm(const std::string& filename)
{
    int ret = 0;
    h3mlib_ctx_t h3m;

    if (0 != (ret = h3m_read(&h3m, filename.c_str()))) {
        return ret;
    }

    ret = H3MToHlm(h3m, this);

    h3m_exit(&h3m);

    return ret;
}

int Hlm::saveH3M(const std::string& filename)
{
    int ret = 0;
    h3mlib_ctx_t h3m = HlmToH3M(this);

    if (NULL == h3m) {
        return 1;
    }

    ret = h3m_write(h3m, filename.c_str());
    h3m_exit(&h3m);

    return ret;
}

int Hlm::saveHlm(const std::string& filename)
{
    return 1;
}

int Hlm::mirrorNW(int z, Corner destination)
{
    for (size_t y = 0; y < m_size / 2; ++y) {
        for (size_t x = 0; x < m_size / 2; ++x) {
            Terrain t = m_terrain[z][x][y];
            bool obstacle = m_obstacles[z][x][y];
            size_t x_mirr = m_size - 1 - x;
            size_t y_mirr = m_size - 1 - y;

            switch (destination)
            {
            case Corner::ALL:
                // NE
                xyzSetTerrain(x_mirr, y, z, t); 
                m_obstacles[z][x_mirr][y] = obstacle;
                // SW
                xyzSetTerrain(x, y_mirr, z, t);
                m_obstacles[z][x][y_mirr] = obstacle;
                // SE
                xyzSetTerrain(x_mirr, y_mirr, z, t);
                m_obstacles[z][x_mirr][y_mirr] = obstacle;
                break;
            default: // strongly typed enum should always prevent default 
                return 1;
            }
        }
    }

    return 0;
}

void Hlm::genHBWang()
{
    hbwang_gen_from_file("bw_round_rooms_diagonal_corridors.png", m_size, 
        reinterpret_cast<unsigned char *>(m_obstacles[0].data()), H3M_MAX_SIZE);
}
