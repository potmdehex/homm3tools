#pragma once

#include "map_object.h"

#include "enum_alignment.h"
#include "enum_owner.h"

#include <string>

class MapHero : public MapObject
{
public:
    MapHero(int x, int y, int z, const std::string& name)
        : MapObject(MapObjectType::HERO, x, y, z, name),
        m_owner(),
        m_has_creatures(),
        m_has_random_creatures()
    {}

    // type is decided by MapObject::m_name
    Owner m_owner;
    std::string m_custom_name;

    bool m_formation_grouped;
    bool m_has_creatures;
    bool m_has_random_creatures;
    struct Creature
    {
        std::string name;
        int quantity;
    } m_creatures[7];
};
