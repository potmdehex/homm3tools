#pragma once

#include "map_object.h"

#include <string>

class MapGeneric : public MapObject
{
public:
    MapGeneric(int x, int y, int z, const std::string& name)
        : MapObject(MapObjectType::GENERIC, x, y, z, name)
    {}
};
