/* Created by John Åkerblom 2014-12-29, ported from deprecated Python code */

#if defined _WIN32 && NO_CRT
        #include "../win/advapi_rand.h"
        #define rand advapi_rand_byte
#else
        #include <stdlib.h>
        #include <time.h>
#endif

/* Start of part differing between dispatch_road.c and dispatch_river.c,
   apart from function name. Could merge them. */
#include "dispatch_river.h"

#define RANDOM_ELEMENT(a) a[(rand() + 1) % sizeof(a)]

static const unsigned char pool_corners[] = { 0x00, 0x01, 0x02, 0x03  };
#define pool_diagonals pool_corners
#define SP_CROSSROADS 0x04
static const unsigned char pool_half_crossroads_vert[] = { 0x05, 0x06 };
static const unsigned char pool_half_crossroads_hori[] = { 0x07, 0x08 };
static const unsigned char pool_vert[] = { 0x09, 0x0A };
static const unsigned char pool_hori[] = { 0x0B, 0x0C };
#define SP_VERT_END RANDOM_ELEMENT(pool_vert)
#define SP_HORI_END RANDOM_ELEMENT(pool_hori)



#define M_HORI 0x04
#define M_VERT 0x08

/* End of part differing between dispatch_road.c and dispatch_river.c */

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
#define SINGLE_CASE(DIR, MIRR, SPRITE) \
        case DIR: \
                *sprite = SPRITE; \
                *mirr |= MIRR; \
                break;

#define NWSE_DIAGONAL_MIRR \
        ((0 == n_type)? M_HORI : ((n_sprite < 0x06)? M_VERT : M_HORI))

#define NESW_DIAGONAL_MIRR \
        ((0 == n_type)? 0 : ((n_sprite < 0x06)? M_VERT | M_HORI: 0))

int dispatch_river(unsigned char adjacent,
        unsigned char *sprite,
        unsigned char *mirr,
        unsigned char n_type,
        unsigned char n_sprite)
{
        switch (adjacent)
        {
        /* Crossroads */
        SINGLE_CASE(W | E | N | S, 0, SP_CROSSROADS)

        /* Half-crossroads */
        POOL_CASE(E | N | S, 0, pool_half_crossroads_hori)
        POOL_CASE(W | N | S, M_HORI, pool_half_crossroads_hori)
        POOL_CASE(W | E | S, M_HORI, pool_half_crossroads_vert)
        POOL_CASE(W | E | N, M_VERT | M_HORI, pool_half_crossroads_vert)

        /* Diagonals */
        POOL_CASE(N | E | NW | SE, NWSE_DIAGONAL_MIRR, pool_diagonals)
        POOL_CASE(N | E | NW, NWSE_DIAGONAL_MIRR, pool_diagonals)
        POOL_CASE(N | E | SE, NWSE_DIAGONAL_MIRR ^ (M_HORI | M_VERT), pool_diagonals)

        POOL_CASE(W | S | NW | SE, NWSE_DIAGONAL_MIRR, pool_diagonals)
        POOL_CASE(W | S | NW, NWSE_DIAGONAL_MIRR, pool_diagonals)
        POOL_CASE(W | S | SE, NWSE_DIAGONAL_MIRR, pool_diagonals)
        
        POOL_CASE(N | W | NE | SW, NESW_DIAGONAL_MIRR, pool_diagonals)
        POOL_CASE(N | W | NE, NESW_DIAGONAL_MIRR, pool_diagonals)
        POOL_CASE(N | W | SW, NESW_DIAGONAL_MIRR  ^ (M_HORI | M_VERT), pool_diagonals)

        POOL_CASE(E | S | NE | SW, NESW_DIAGONAL_MIRR, pool_diagonals)
        POOL_CASE(E | S | NE, NESW_DIAGONAL_MIRR, pool_diagonals)
        POOL_CASE(E | S | SW, NESW_DIAGONAL_MIRR, pool_diagonals)

        /* Corners */
        POOL_CASE(E | S, 0, pool_corners)
        POOL_CASE(E | S | SE, 0, pool_corners)
        POOL_CASE(W | S, M_HORI, pool_corners)
        POOL_CASE(W | S | SW, M_HORI, pool_corners)
        POOL_CASE(E | N, M_VERT, pool_corners)
        POOL_CASE(E | N | NE, M_VERT, pool_corners)
        POOL_CASE(W | N, M_HORI | M_VERT, pool_corners)
        POOL_CASE(W | N | NW, M_HORI | M_VERT, pool_corners)

        /* Horizontal */
        SINGLE_CASE(E, 0, SP_HORI_END)
        SINGLE_CASE(NE | E | SE, 0, SP_HORI_END)
        SINGLE_CASE(NE | E, 0, SP_HORI_END)
        SINGLE_CASE(E | SE, 0, SP_HORI_END)
        SINGLE_CASE(W, M_HORI, SP_HORI_END)
        SINGLE_CASE(NW | W | SW, M_HORI, SP_HORI_END)
        SINGLE_CASE(NW | W, M_HORI, SP_HORI_END)
        SINGLE_CASE(W | SW, M_HORI, SP_HORI_END)
        POOL_CASE(E | W, 0, pool_hori)
        /* Oh boy here we go, horizontal continued */
        POOL_CASE(E | W | NE | NW | SE | SW, 0, pool_hori)

        POOL_CASE(E | W | NW | SE | SW, 0, pool_hori)
        POOL_CASE(E | W | NE | SE | SW, 0, pool_hori)
        POOL_CASE(E | W | NE | NW | SW, 0, pool_hori)
        POOL_CASE(E | W | NE | NW | SE, 0, pool_hori)

        POOL_CASE(E | W | NE | NW, 0, pool_hori)
        POOL_CASE(E | W | NE | SW, 0, pool_hori)
        POOL_CASE(E | W | NE | SE, 0, pool_hori)
        POOL_CASE(E | W | NW | SW, 0, pool_hori)
        POOL_CASE(E | W | NW | SE, 0, pool_hori)
        POOL_CASE(E | W | SW | SE, 0, pool_hori)

        POOL_CASE(E | W | NE, 0, pool_hori)
        POOL_CASE(E | W | NW, 0, pool_hori)
        POOL_CASE(E | W | SE, 0, pool_hori)
        POOL_CASE(E | W | SW, 0, pool_hori)

        /* Vertical */
        SINGLE_CASE(S, 0, SP_VERT_END)
        SINGLE_CASE(SW | S | SE, 0, SP_VERT_END)
        SINGLE_CASE(SW | S, 0, SP_VERT_END)
        SINGLE_CASE(S | SE, 0, SP_VERT_END)
        SINGLE_CASE(N, M_VERT, SP_VERT_END)
        SINGLE_CASE(NW | N | NE, M_VERT, SP_VERT_END)
        SINGLE_CASE(NW | N, M_VERT, SP_VERT_END)
        SINGLE_CASE(N | NE, M_VERT, SP_VERT_END)
        POOL_CASE(S | N, 0, pool_vert)
        /* Oh boy here we go, vertical continued */
        POOL_CASE(S | N | NE | NW | SE | SW, 0, pool_vert)

        POOL_CASE(S | N | NW | SE | SW, 0, pool_vert)
        POOL_CASE(S | N | NE | SE | SW, 0, pool_vert)
        POOL_CASE(S | N | NE | NW | SW, 0, pool_vert)
        POOL_CASE(S | N | NE | NW | SE, 0, pool_vert)

        POOL_CASE(S | N | NE | NW, 0, pool_vert)
        POOL_CASE(S | N | NE | SW, 0, pool_vert)
        POOL_CASE(S | N | NE | SE, 0, pool_vert)
        POOL_CASE(S | N | NW | SW, 0, pool_vert)
        POOL_CASE(S | N | NW | SE, 0, pool_vert)
        POOL_CASE(S | N | SW | SE, 0, pool_vert)

        POOL_CASE(S | N | NE, 0, pool_vert)
        POOL_CASE(S | N | NW, 0, pool_vert)
        POOL_CASE(S | N | SE, 0, pool_vert)
        POOL_CASE(S | N | SW, 0, pool_vert)

        /* Single tile */
        SINGLE_CASE(0, 0, SP_VERT_END)
        default:
                /* return 1; */
                *sprite = SP_VERT_END;
                return 0;
        }
        return 0;
}

