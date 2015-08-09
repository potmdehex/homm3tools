// Created by John Åkerblom 2014-12-23

#include "h3mlib.h"
#include "h3m_object.h"
#include "gen/od_default_bodies.h"

#include <string.h>
#include <stdlib.h>
#include <stdio.h>              // DEBUG printf

#define _UNIQUE_CASE(T) \
    case H3M_OBJECT_##T: \
    src = T##_default_od_hex; \
    *body_size = sizeof(T##_default_od_hex); \
    break;

int get_default_od_body(enum H3M_OBJECT type, uint8_t **body,
    int *binary_compatible, size_t *body_size)
{
    uint8_t *src = NULL;

    *body = NULL;
    *binary_compatible = 1;

    switch (type) {
        // TODO: ABSOD objects
        //_UNIQUE_CASE(QUEST_GUARD)
        _UNIQUE_CASE(SEERS_HUT)
        _UNIQUE_CASE(PANDORAS_BOX)
        _UNIQUE_CASE(EVENT)
        _UNIQUE_CASE(TOWN)
        _UNIQUE_CASE(HERO)
        _UNIQUE_CASE(RANDOM_HERO)
        _UNIQUE_CASE(PRISON)
        _UNIQUE_CASE(MONSTER)
        _UNIQUE_CASE(ARTIFACT)
        _UNIQUE_CASE(RESOURCE)
        _UNIQUE_CASE(GARRISON)
        _UNIQUE_CASE(SPELL_SCROLL)

    case H3M_OBJECT_OCEAN_BOTTLE:
    case H3M_OBJECT_SIGN:
        src = MESSAGE_CONTAINER_default_od_hex;
        *body_size = sizeof(MESSAGE_CONTAINER_default_od_hex);
        break;
    case H3M_OBJECT_LIGHTHOUSE:
    case H3M_OBJECT_SHIPYARD:
    case H3M_OBJECT_RESOURCE_GENERATOR:
    case H3M_OBJECT_DWELLING:
        src = FLAGGED_default_od_hex;
        *body_size = sizeof(FLAGGED_default_od_hex);
        break;
    case H3M_OBJECT_GENERIC_IMPASSABLE_TERRAIN:
    case H3M_OBJECT_GENERIC_PASSABLE_TERRAIN:
    case H3M_OBJECT_GENERIC_VISITABLE:
    case H3M_OBJECT_GENERIC_TREASURE:
    case H3M_OBJECT_GENERIC_BOAT:
    case H3M_OBJECT_MONOLITH_TWO_WAY:
    case H3M_OBJECT_SUBTERRANEAN_GATE:
        // Generic objects have no body
        break;
    default:
        return 1;
    }

    if (NULL != src) {
        *body = malloc(*body_size);
        memcpy(*body, src, *body_size);
    }

    return 0;
}
