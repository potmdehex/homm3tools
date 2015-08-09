#pragma once

#include "map_object.h"

#include "enum_disposition.h"

#include <string>

class MapMonster : public MapObject
{
public:
    MapMonster(int x, int y, int z, const std::string& name)
        : MapObject(MapObjectType::MONSTER, x, y, z, name),
        m_quantity(),
        m_disposition(Disposition::MayJoin)
    {}
    // type is decided by MapObject::m_name
    int m_quantity;
    std::string m_custom_message;
    Disposition m_disposition;
};
