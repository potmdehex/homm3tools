// Created by John Åkerblom 2014-12-23

#include "../h3mlib.h"
#include "../meta/meta_object.h"
#include "../gen/od_default_bodies.h"

#include <string.h>
#include <stdlib.h>
#include <stdio.h>              // DEBUG printf

// TODO: scrap the whole contents of this file and replace with actual code
// setting the individual members of each od body to its defaults values
// instead of copying the default blobs.

#define _UNIQUE_CASE(T) \
    case META_OBJECT_##T: \
    src = T##_default_od_hex; \
    *body_size = sizeof(T##_default_od_hex); \
    break;

int get_default_od_body(uint32_t fm, enum META_OBJECT type, uint8_t **body,
    int *binary_compatible, size_t *body_size)
{
    uint8_t *src = NULL;

    *body = NULL;
    *binary_compatible = 1;

    switch (type) {
    // TODO add placeholder hero object,
    // random dwellings and any other missing objects

    // Objects with different default body depending on format 
    case META_OBJECT_GARRISON:
        if (H3M_FORMAT_ROE == fm) {
            src = GARRISON_default_od_hex;
            *body_size = sizeof(GARRISON_default_od_hex);
        } else {
            src = GARRISON_ABSOD_default_od_hex;
            *body_size = sizeof(GARRISON_ABSOD_default_od_hex);
        }
        break;
    case META_OBJECT_HERO:
    case META_OBJECT_RANDOM_HERO:
    case META_OBJECT_PRISON:
        switch (fm) {
        case H3M_FORMAT_ROE: 
            src = HERO_default_od_hex;
            *body_size = sizeof(HERO_default_od_hex);
            break;
        case H3M_FORMAT_AB:
            src = HERO_AB_default_od_hex;
            *body_size = sizeof(HERO_AB_default_od_hex);
            break;
        case H3M_FORMAT_SOD:
            src = HERO_SOD_default_od_hex;
            *body_size = sizeof(HERO_SOD_default_od_hex);
            break;
        default:
            return 1;
        }
        break;
    case META_OBJECT_SEERS_HUT:
        if (H3M_FORMAT_ROE == fm) {
            src = SEERS_HUT_default_od_hex;
            *body_size = sizeof(SEERS_HUT_default_od_hex);
        }
        else {
            src = SEERS_HUT_ABSOD_default_od_hex;
            *body_size = sizeof(SEERS_HUT_ABSOD_default_od_hex);
        }
        break;
    case META_OBJECT_TOWN:
        switch (fm) {
        case H3M_FORMAT_ROE:
            src = TOWN_default_od_hex;
            *body_size = sizeof(TOWN_default_od_hex);
            break;
        case H3M_FORMAT_AB:
            src = TOWN_AB_default_od_hex;
            *body_size = sizeof(TOWN_AB_default_od_hex);
            break;
        case H3M_FORMAT_SOD:
            src = TOWN_SOD_default_od_hex;
            *body_size = sizeof(TOWN_SOD_default_od_hex);
            break;
        default:
            return 1;
        }
        break;

    // Objects which have unique bodies and dont depend on format
    _UNIQUE_CASE(PANDORAS_BOX)
    _UNIQUE_CASE(EVENT)
    _UNIQUE_CASE(MONSTER)
    _UNIQUE_CASE(ARTIFACT)
    _UNIQUE_CASE(RESOURCE)
    _UNIQUE_CASE(SPELL_SCROLL)
    _UNIQUE_CASE(QUEST_GUARD)

    // Objects where multiple objects have same body and dont depend on format
    case META_OBJECT_OCEAN_BOTTLE:
    case META_OBJECT_SIGN:
        src = MESSAGE_CONTAINER_default_od_hex;
        *body_size = sizeof(MESSAGE_CONTAINER_default_od_hex);
        break;
    case META_OBJECT_LIGHTHOUSE:
    case META_OBJECT_SHIPYARD:
    case META_OBJECT_RESOURCE_GENERATOR:
    case META_OBJECT_DWELLING:
        src = FLAGGED_default_od_hex;
        *body_size = sizeof(FLAGGED_default_od_hex);
        break;
    case META_OBJECT_GENERIC_IMPASSABLE_TERRAIN:
    case META_OBJECT_GENERIC_IMPASSABLE_TERRAIN_ABSOD:
    case META_OBJECT_GENERIC_PASSABLE_TERRAIN:
    case META_OBJECT_GENERIC_PASSABLE_TERRAIN_SOD:
    case META_OBJECT_GENERIC_VISITABLE:
    case META_OBJECT_GENERIC_TREASURE:
    case META_OBJECT_GENERIC_BOAT:
    case META_OBJECT_MONOLITH_TWO_WAY:
    case META_OBJECT_SUBTERRANEAN_GATE:
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
