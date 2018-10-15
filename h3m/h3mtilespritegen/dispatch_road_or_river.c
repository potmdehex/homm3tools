#if defined _WIN32 && NO_CRT
        #include "../win/advapi_rand.h"
        #define rand advapi_rand_byte
#else
        #include <stdlib.h>
        #include <time.h>
#endif

#include "dispatch_road_or_river.h"

#define RANDOM_ELEMENT(a) a[(rand() + 1) % sizeof(a)]

#define NW 0x01
#define N  0x02
#define NE 0x04
#define W  0x08
#define E  0x10
#define SW 0x20
#define S  0x40
#define SE 0x80

#define POOL_CASE(DIR, MIRR, POOL) \
    case DIR: \
        *sprite = RANDOM_ELEMENT(POOL); \
        *mirr |= MIRR; \
        break;

#define POOL_CASE_ANY_CORNER(DIR, MIRR, POOL) \
    case DIR                    : \
    case DIR | NE               : \
    case DIR |      NW          : \
    case DIR | NE | NW          : \
    case DIR |           SE     : \
    case DIR | NE |      SE     : \
    case DIR |      NW | SE     : \
    case DIR | NE | NW | SE     : \
    case DIR |                SW: \
    case DIR | NE |           SW: \
    case DIR |      NW |      SW: \
    case DIR | NE | NW |      SW: \
    case DIR |           SE | SW: \
    case DIR | NE |      SE | SW: \
    case DIR |      NW | SE | SW: \
    case DIR | NE | NW | SE | SW: \
        *sprite = RANDOM_ELEMENT(POOL); \
        *mirr |= MIRR; \
        break;

#define SINGLE_CASE(DIR, MIRR, SPRITE) \
    case DIR: \
        *sprite = SPRITE; \
        *mirr |= MIRR; \
        break;

#define SINGLE_CASE_ANY_CORNER(DIR, MIRR, SPRITE) \
    case DIR                    : \
    case DIR | NE               : \
    case DIR |      NW          : \
    case DIR | NE | NW          : \
    case DIR |           SE     : \
    case DIR | NE |      SE     : \
    case DIR |      NW | SE     : \
    case DIR | NE | NW | SE     : \
    case DIR |                SW: \
    case DIR | NE |           SW: \
    case DIR |      NW |      SW: \
    case DIR | NE | NW |      SW: \
    case DIR |           SE | SW: \
    case DIR | NE |      SE | SW: \
    case DIR |      NW | SE | SW: \
    case DIR | NE | NW | SE | SW: \
        *sprite = SPRITE; \
        *mirr |= MIRR; \
        break;

#define CREATE_DISPATCHER(                                                             \
    TYPE,                                                                              \
    pool_corners,                                                                      \
    pool_diagonals,                                                                    \
    pool_half_crossroads_hori,                                                         \
    pool_half_crossroads_vert,                                                         \
    pool_hori,                                                                         \
    pool_vert,                                                                         \
    SP_HORI_END,                                                                       \
    SP_VERT_END,                                                                       \
    SP_CROSSROADS,                                                                     \
    M_HORI,                                                                            \
    M_VERT,                                                                            \
    NWSE_DIAGONAL_MIRR,                                                                \
    NESW_DIAGONAL_MIRR                                                                 \
)                                                                                      \
int dispatch_##TYPE(unsigned char adjacent,                                            \
                    unsigned char *sprite,                                             \
                    unsigned char *mirr,                                               \
                    unsigned char n_type,                                              \
                    unsigned char n_sprite)                                            \
{                                                                                      \
    switch (adjacent)                                                                  \
    {                                                                                  \
        /* Crossroads */                                                               \
        SINGLE_CASE_ANY_CORNER(W | E | N | S, 0, SP_CROSSROADS)                        \
                                                                                       \
        /* Half-crossroads */                                                          \
        POOL_CASE_ANY_CORNER(E | N | S, 0, pool_half_crossroads_hori)                  \
        POOL_CASE_ANY_CORNER(W | N | S, M_HORI, pool_half_crossroads_hori)             \
        POOL_CASE_ANY_CORNER(W | E | S, M_HORI, pool_half_crossroads_vert)             \
        POOL_CASE_ANY_CORNER(W | E | N, M_VERT | M_HORI, pool_half_crossroads_vert)    \
                                                                                       \
        /* Diagonals */                                                                \
        POOL_CASE(N | E | NW | SE, NWSE_DIAGONAL_MIRR(M_HORI, M_VERT), pool_diagonals) \
        POOL_CASE(N | E | NW, NWSE_DIAGONAL_MIRR(M_HORI, M_VERT), pool_diagonals)      \
        POOL_CASE(N | E | SE, NWSE_DIAGONAL_MIRR(M_HORI, M_VERT), pool_diagonals)      \
        POOL_CASE(N | NW | SE, NWSE_DIAGONAL_MIRR(M_HORI, M_VERT), pool_diagonals)     \
        POOL_CASE(E | NW | SE, NWSE_DIAGONAL_MIRR(M_HORI, M_VERT), pool_diagonals)     \
                                                                                       \
        POOL_CASE(W | S | NW | SE, NWSE_DIAGONAL_MIRR(M_HORI, M_VERT), pool_diagonals) \
        POOL_CASE(W | S | NW, NWSE_DIAGONAL_MIRR(M_HORI, M_VERT), pool_diagonals)      \
        POOL_CASE(W | S | SE, NWSE_DIAGONAL_MIRR(M_HORI, M_VERT), pool_diagonals)      \
        POOL_CASE(S | NW | SE, NWSE_DIAGONAL_MIRR(M_HORI, M_VERT), pool_diagonals)     \
        POOL_CASE(W | NW | SE, NWSE_DIAGONAL_MIRR(M_HORI, M_VERT), pool_diagonals)     \
                                                                                       \
        POOL_CASE(N | W | NE | SW, NESW_DIAGONAL_MIRR(M_HORI, M_VERT), pool_diagonals) \
        POOL_CASE(N | W | NE, NESW_DIAGONAL_MIRR(M_HORI, M_VERT), pool_diagonals)      \
        POOL_CASE(N | W | SW, NESW_DIAGONAL_MIRR(M_HORI, M_VERT), pool_diagonals)      \
        POOL_CASE(N | NE | SW, NESW_DIAGONAL_MIRR(M_HORI, M_VERT), pool_diagonals)     \
                                                                                       \
        POOL_CASE(E | S | NE | SW, NESW_DIAGONAL_MIRR(M_HORI, M_VERT), pool_diagonals) \
        POOL_CASE(E | S | NE, NESW_DIAGONAL_MIRR(M_HORI, M_VERT), pool_diagonals)      \
        POOL_CASE(E | S | SW, NESW_DIAGONAL_MIRR(M_HORI, M_VERT), pool_diagonals)      \
        POOL_CASE(E | NE | SW, NESW_DIAGONAL_MIRR(M_HORI, M_VERT), pool_diagonals)     \
        POOL_CASE(S | NE | SW, NESW_DIAGONAL_MIRR(M_HORI, M_VERT), pool_diagonals)     \
                                                                                       \
        /* Corners */                                                                  \
        POOL_CASE(E | N                    ,          M_VERT, pool_corners)            \
        POOL_CASE(E | N |                SW,          M_VERT, pool_corners)            \
        POOL_CASE(E | N |           SE | SW,          M_VERT, pool_corners)            \
        POOL_CASE(E | N |      NW |      SW,          M_VERT, pool_corners)            \
        POOL_CASE(E | N | NE               ,          M_VERT, pool_corners)            \
        POOL_CASE(E | N | NE |      SE     ,          M_VERT, pool_corners)            \
        POOL_CASE(E | N | NE | NW          ,          M_VERT, pool_corners)            \
        POOL_CASE(E | N | NE | NW | SE     ,          M_VERT, pool_corners)            \
        POOL_CASE(E | S                    , 0,               pool_corners)            \
        POOL_CASE(E | S |           SE     , 0,               pool_corners)            \
        POOL_CASE(E | S |           SE | SW, 0,               pool_corners)            \
        POOL_CASE(E | S |      NW          , 0,               pool_corners)            \
        POOL_CASE(E | S |      NW |      SW, 0,               pool_corners)            \
        POOL_CASE(E | S | NE |      SE     , 0,               pool_corners)            \
        POOL_CASE(E | S | NE |      SE | SW, 0,               pool_corners)            \
        POOL_CASE(E | S | NE | NW          , 0,               pool_corners)            \
        POOL_CASE(W | N                    , M_HORI | M_VERT, pool_corners)            \
        POOL_CASE(W | N |           SE     , M_HORI | M_VERT, pool_corners)            \
        POOL_CASE(W | N |           SE | SW, M_HORI | M_VERT, pool_corners)            \
        POOL_CASE(W | N |      NW          , M_HORI | M_VERT, pool_corners)            \
        POOL_CASE(W | N |      NW |      SW, M_HORI | M_VERT, pool_corners)            \
        POOL_CASE(W | N | NE |      SE     , M_HORI | M_VERT, pool_corners)            \
        POOL_CASE(W | N | NE | NW          , M_HORI | M_VERT, pool_corners)            \
        POOL_CASE(W | N | NE | NW      | SW, M_HORI | M_VERT, pool_corners)            \
        POOL_CASE(W | S                    , M_HORI,          pool_corners)            \
        POOL_CASE(W | S |                SW, M_HORI,          pool_corners)            \
        POOL_CASE(W | S |           SE | SW, M_HORI,          pool_corners)            \
        POOL_CASE(W | S |      NW |      SW, M_HORI,          pool_corners)            \
        POOL_CASE(W | S |      NW | SE | SW, M_HORI,          pool_corners)            \
        POOL_CASE(W | S | NE               , M_HORI,          pool_corners)            \
        POOL_CASE(W | S | NE |      SE     , M_HORI,          pool_corners)            \
        POOL_CASE(W | S | NE | NW          , M_HORI,          pool_corners)            \
                                                                                       \
        /* Horizontal */                                                               \
        SINGLE_CASE(E, 0, SP_HORI_END)                                                 \
        SINGLE_CASE(NE | E | SE, 0, SP_HORI_END)                                       \
        SINGLE_CASE(NE | E, 0, SP_HORI_END)                                            \
        SINGLE_CASE(NW | E, 0, SP_HORI_END)                                            \
        SINGLE_CASE(E | SE, 0, SP_HORI_END)                                            \
        SINGLE_CASE(W, M_HORI, SP_HORI_END)                                            \
        SINGLE_CASE(NW | W | SW, M_HORI, SP_HORI_END)                                  \
        SINGLE_CASE(W | NE, M_HORI, SP_HORI_END)                                       \
        SINGLE_CASE(W | NE | SW, M_HORI, SP_HORI_END)                                  \
        SINGLE_CASE(NW | W | SW | NE, M_HORI, SP_HORI_END)                             \
        SINGLE_CASE(NW | W, M_HORI, SP_HORI_END)                                       \
        SINGLE_CASE(W | SW, M_HORI, SP_HORI_END)                                       \
        POOL_CASE(E | W, 0, pool_hori)                                                 \
        /* Oh boy here we go, horizontal continued */                                  \
        POOL_CASE(E | W | NE | NW | SE | SW, 0, pool_hori)                             \
                                                                                       \
        POOL_CASE(E | W | NW | SE | SW, 0, pool_hori)                                  \
        POOL_CASE(E | W | NE | SE | SW, 0, pool_hori)                                  \
        POOL_CASE(E | W | NE | NW | SW, 0, pool_hori)                                  \
        POOL_CASE(E | W | NE | NW | SE, 0, pool_hori)                                  \
                                                                                       \
        POOL_CASE(E | W | NE | NW, 0, pool_hori)                                       \
        POOL_CASE(E | W | NE | SW, 0, pool_hori)                                       \
        POOL_CASE(E | W | NE | SE, 0, pool_hori)                                       \
        POOL_CASE(E | W | NW | SW, 0, pool_hori)                                       \
        POOL_CASE(E | W | NW | SE, 0, pool_hori)                                       \
        POOL_CASE(E | W | SW | SE, 0, pool_hori)                                       \
                                                                                       \
        POOL_CASE(E | W | NE, 0, pool_hori)                                            \
        POOL_CASE(E | W | NW, 0, pool_hori)                                            \
        POOL_CASE(E | W | SE, 0, pool_hori)                                            \
        POOL_CASE(E | W | SW, 0, pool_hori)                                            \
                                                                                       \
        /* Vertical */                                                                 \
        SINGLE_CASE(S, 0, SP_VERT_END)                                                 \
        SINGLE_CASE(SW | S | SE, 0, SP_VERT_END)                                       \
        SINGLE_CASE(SW | S, 0, SP_VERT_END)                                            \
        SINGLE_CASE(S | SE, 0, SP_VERT_END)                                            \
        SINGLE_CASE(N, M_VERT, SP_VERT_END)                                            \
        SINGLE_CASE(NW | N | NE, M_VERT, SP_VERT_END)                                  \
        SINGLE_CASE(NW | N, M_VERT, SP_VERT_END)                                       \
        SINGLE_CASE(N | NE, M_VERT, SP_VERT_END)                                       \
        SINGLE_CASE(SE | N, M_VERT, SP_VERT_END)                                       \
        SINGLE_CASE(SW | N, M_VERT, SP_VERT_END)                                       \
        SINGLE_CASE(SE | N | NE | NW, M_VERT, SP_VERT_END)                             \
        SINGLE_CASE(SW | N | NE | NW, M_VERT, SP_VERT_END)                             \
        POOL_CASE(S | N, 0, pool_vert)                                                 \
                                                                                       \
        /* Oh boy here we go, vertical continued */                                    \
        POOL_CASE(S | N | NE | NW | SE | SW, 0, pool_vert)                             \
                                                                                       \
        POOL_CASE(S | N | NW | SE | SW, 0, pool_vert)                                  \
        POOL_CASE(S | N | NE | SE | SW, 0, pool_vert)                                  \
        POOL_CASE(S | N | NE | NW | SW, 0, pool_vert)                                  \
        POOL_CASE(S | N | NE | NW | SE, 0, pool_vert)                                  \
                                                                                       \
        POOL_CASE(S | N | NE | NW, 0, pool_vert)                                       \
        POOL_CASE(S | N | NE | SW, 0, pool_vert)                                       \
        POOL_CASE(S | N | NE | SE, 0, pool_vert)                                       \
        POOL_CASE(S | N | NW | SW, 0, pool_vert)                                       \
        POOL_CASE(S | N | NW | SE, 0, pool_vert)                                       \
        POOL_CASE(S | N | SW | SE, 0, pool_vert)                                       \
                                                                                       \
        POOL_CASE(S | N | NE, 0, pool_vert)                                            \
        POOL_CASE(S | N | NW, 0, pool_vert)                                            \
        POOL_CASE(S | N | SE, 0, pool_vert)                                            \
        POOL_CASE(S | N | SW, 0, pool_vert)                                            \
                                                                                       \
        /* Single tile */                                                              \
        SINGLE_CASE(0, 0, SP_VERT_END)                                                 \
                                                                                       \
        default:                                                                       \
            *sprite = SP_VERT_END;                                                     \
    }                                                                                  \
                                                                                       \
    return 0;                                                                          \
}

/* Rivers. */
static const unsigned char river_pool_corners[] = { 0x00, 0x01, 0x02, 0x03 };
static const unsigned char river_pool_diagonals[] = { 0x00, 0x01, 0x02, 0x03 };
static const unsigned char river_pool_half_crossroads_hori[] = { 0x07, 0x08 };
static const unsigned char river_pool_half_crossroads_vert[] = { 0x05, 0x06 };
static const unsigned char river_pool_hori[] = { 0x0B, 0x0C };
static const unsigned char river_pool_vert[] = { 0x09, 0x0A };

#define RIVER_SP_HORI_END RANDOM_ELEMENT(river_pool_hori)
#define RIVER_SP_VERT_END RANDOM_ELEMENT(river_pool_vert)
#define RIVER_SP_CROSSROADS 0x04

#define RIVER_M_HORI 0x04
#define RIVER_M_VERT 0x08

#define RIVER_NWSE_DIAGONAL_MIRR(M_HORI, M_VERT) \
    ((0 == n_type)? M_HORI: M_VERT)

#define RIVER_NESW_DIAGONAL_MIRR(M_HORI, M_VERT) \
    ((0 == n_type)? 0: M_VERT | M_HORI)

CREATE_DISPATCHER(
    river,
    river_pool_corners,
    river_pool_diagonals,
    river_pool_half_crossroads_hori,
    river_pool_half_crossroads_vert,
    river_pool_hori,
    river_pool_vert,
    RIVER_SP_HORI_END,
    RIVER_SP_VERT_END,
    RIVER_SP_CROSSROADS,
    RIVER_M_HORI,
    RIVER_M_VERT,
    RIVER_NWSE_DIAGONAL_MIRR,
    RIVER_NESW_DIAGONAL_MIRR
)

/* Roads. */
static const unsigned char road_pool_corners[] = { 0x00, 0x01 }; /* unused 0x02 */
static const unsigned char road_pool_diagonals[] = { 0x03, 0x04 }; /* unused 0x05 */
static const unsigned char road_pool_half_crossroads_hori[] = { 0x06, 0x07 };
static const unsigned char road_pool_half_crossroads_vert[] = { 0x08, 0x09 };
static const unsigned char road_pool_hori[] = { 0x0C, 0x0D };
static const unsigned char road_pool_vert[] = { 0x0A, 0x0B };

#define ROAD_SP_CROSSROADS 0x10
#define ROAD_SP_HORI_END 0x0F
#define ROAD_SP_VERT_END 0x0E

#define ROAD_M_HORI 0x10
#define ROAD_M_VERT 0x20

#define ROAD_NWSE_DIAGONAL_MIRR(M_HORI, M_VERT) \
    ((0 == n_type)? M_HORI : ((n_sprite > 0x02)? M_VERT : M_HORI))

#define ROAD_NESW_DIAGONAL_MIRR(M_HORI, M_VERT) \
    ((0 == n_type)? 0 : ((n_sprite > 0x02)? M_VERT | M_HORI: 0))

CREATE_DISPATCHER(
    road,
    road_pool_corners,
    road_pool_diagonals,
    road_pool_half_crossroads_hori,
    road_pool_half_crossroads_vert,
    road_pool_hori,
    road_pool_vert,
    ROAD_SP_HORI_END,
    ROAD_SP_VERT_END,
    ROAD_SP_CROSSROADS,
    ROAD_M_HORI,
    ROAD_M_VERT,
    ROAD_NWSE_DIAGONAL_MIRR,
    ROAD_NESW_DIAGONAL_MIRR
)
