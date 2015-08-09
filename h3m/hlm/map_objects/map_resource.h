#pragma once

#include "map_object.h"

#include <string>

class MapResource : public MapObject
{
public:
    MapResource(int x, int y, int z, const std::string& name)
        : MapObject(MapObjectType::RESOURCE, x, y, z, name)
    {}
    // type is decided by MapObject::m_name
    int m_quantity;
    std::string m_custom_message;
};
