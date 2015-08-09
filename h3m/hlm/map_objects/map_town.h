#pragma once

#include "map_object.h"
#include "map_hero.h"

#include "enum_alignment.h"
#include "enum_owner.h"

#include <string>

class MapTown : public MapObject
{
public:
    MapTown(int x, int y, int z, const std::string& name)
        : MapObject(MapObjectType::TOWN, x, y, z, name),
        m_visiting_hero(x, y, z, ""),
        m_owner()
    {}
    // type is decided by MapObject::m_name
    Owner m_owner;
    Alignment m_alignment;
    std::string m_custom_name;

    bool m_has_visiting_hero;
    MapHero m_visiting_hero;
};
