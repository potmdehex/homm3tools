#pragma once

#include <string>

enum class MapObjectType
{
    GENERIC,
    ARTIFACT,
    HERO,
    MONSTER,
    RESOURCE,
    TOWN
};

class MapObject
{
public:
    MapObject(MapObjectType type, int x, int y, int z, const std::string& name) 
        : m_type(type),
        m_x(x),
        m_y(y),
        m_z(z),
        m_name(name)
    { }
    MapObjectType m_type;
    int m_x;
    int m_y;
    int m_z;
    std::string m_name;
};
