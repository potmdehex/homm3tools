#pragma once

#include "map_object.h"

#include <string>

class MapArtifact : public MapObject
{
public:
    MapArtifact(int x, int y, int z, const std::string& name)
        : MapObject(MapObjectType::ARTIFACT, x, y, z, name)
    {}
    
    // type is decided by MapObject::m_name
    std::string m_custom_message;
};
