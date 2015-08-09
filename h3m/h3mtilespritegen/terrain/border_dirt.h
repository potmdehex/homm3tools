/* Created by John Åkerblom 2014-8-25 */
#include "../crossplatform.h"
#include "../h3m_tile.h"

#if defined _WIN32 && NO_CRT
        #include "../win/advapi_rand.h"
        #define rand advapi_rand_byte
#else
        #include <stdlib.h>
#endif

#define DOUBLE_DIAGONAL_DIRT_ON_SAND 0x14

#define RANDOM_ELEMENT(a) a[(rand() + 1) % sizeof(a)]

static const unsigned char pool_ext_corner_dirt[]   = { 0x00, 0x01, 0x02, 0x03 };
static const unsigned char pool_horizontal_dirt[]   = { 0x04, 0x05, 0x06, 0x07 };
static const unsigned char pool_vertical_dirt[]     = { 0x08, 0x09, 0x0A, 0x0B };
static const unsigned char pool_int_corner_dirt[]   = { 0x0C, 0x0D, 0x0E, 0x0F }; 
static const unsigned char pool_ext_diagonal_dirt[] = { 0x10, 0x11 };
static const unsigned char pool_int_diagonal_dirt[] = { 0x12, 0x13 };

void border_n_vertical_dirt(unsigned char *pic, unsigned char *mirr) 
{
        *pic = RANDOM_ELEMENT(pool_vertical_dirt);
}

void border_s_vertical_dirt(unsigned char *pic, unsigned char *mirr) 
{
        *pic = RANDOM_ELEMENT(pool_vertical_dirt);

        *mirr = BIT_VERTICAL;
}

void border_w_horizontal_dirt(unsigned char *pic, unsigned char *mirr) 
{
        *pic = RANDOM_ELEMENT(pool_horizontal_dirt);
}

void border_e_horizontal_dirt(unsigned char *pic, unsigned char *mirr) 
{
        *pic = RANDOM_ELEMENT(pool_horizontal_dirt);

        *mirr = BIT_HORIZONTAL;
}

void border_nw_ext_diagonal_dirt(unsigned char *pic, unsigned char *mirr) 
{
        *pic = RANDOM_ELEMENT(pool_ext_diagonal_dirt);
}

void border_ne_ext_diagonal_dirt(unsigned char *pic, unsigned char *mirr) 
{
        *pic = RANDOM_ELEMENT(pool_ext_diagonal_dirt);

        *mirr = BIT_HORIZONTAL;
}

void border_sw_ext_diagonal_dirt(unsigned char *pic, unsigned char *mirr) 
{
        *pic = RANDOM_ELEMENT(pool_ext_diagonal_dirt);

        *mirr = BIT_VERTICAL;
}

void border_se_ext_diagonal_dirt(unsigned char *pic, unsigned char *mirr) 
{
        *pic = RANDOM_ELEMENT(pool_ext_diagonal_dirt);

        *mirr = BIT_HORIZONTAL | BIT_VERTICAL;
}

void border_nw_int_diagonal_dirt(unsigned char *pic, unsigned char *mirr) 
{
        *pic = RANDOM_ELEMENT(pool_int_diagonal_dirt);

        *mirr = BIT_VERTICAL | BIT_HORIZONTAL;
}

void border_ne_int_diagonal_dirt(unsigned char *pic, unsigned char *mirr)
{
        *pic = RANDOM_ELEMENT(pool_int_diagonal_dirt);

        *mirr = BIT_VERTICAL;
}

void border_sw_int_diagonal_dirt(unsigned char *pic, unsigned char *mirr) 
{
        *pic = RANDOM_ELEMENT(pool_int_diagonal_dirt);

        *mirr = BIT_HORIZONTAL;
}

void border_se_int_diagonal_dirt(unsigned char *pic, unsigned char *mirr) 
{
        *pic = RANDOM_ELEMENT(pool_int_diagonal_dirt);
}

void border_nwse_double_diagonal_dirt(unsigned char *pic, unsigned char *mirr) 
{
        *pic = DOUBLE_DIAGONAL_DIRT_ON_SAND;
        /* The game map editor always flips vertically rather than 
         * horizontally here, although either way looks pretty okay. 
         * Currently we do as the map editor here. */
        *mirr = BIT_VERTICAL;
}

void border_nesw_double_diagonal_dirt(unsigned char *pic, unsigned char *mirr) 
{
        *pic = DOUBLE_DIAGONAL_DIRT_ON_SAND;
}

void border_nw_int_corner_dirt(unsigned char *pic, unsigned char *mirr)
{
        *pic = RANDOM_ELEMENT(pool_int_corner_dirt);
}

void border_ne_int_corner_dirt(unsigned char *pic, unsigned char *mirr) 
{
        *pic = RANDOM_ELEMENT(pool_int_corner_dirt);

        *mirr = BIT_HORIZONTAL;
}

void border_sw_int_corner_dirt(unsigned char *pic, unsigned char *mirr) 
{
        *pic = RANDOM_ELEMENT(pool_int_corner_dirt);

        *mirr = BIT_VERTICAL;
}

void border_se_int_corner_dirt(unsigned char *pic, unsigned char *mirr) 
{
        *pic = RANDOM_ELEMENT(pool_int_corner_dirt);

        *mirr = BIT_HORIZONTAL | BIT_VERTICAL;
}

void border_nw_ext_corner_dirt(unsigned char *pic, unsigned char *mirr)
{
        *pic = RANDOM_ELEMENT(pool_ext_corner_dirt);
}

void border_ne_ext_corner_dirt(unsigned char *pic, unsigned char *mirr) 
{
        *pic = RANDOM_ELEMENT(pool_ext_corner_dirt);

        *mirr = BIT_HORIZONTAL;
}

void border_sw_ext_corner_dirt(unsigned char *pic, unsigned char *mirr) 
{
        *pic = RANDOM_ELEMENT(pool_ext_corner_dirt);

        *mirr = BIT_VERTICAL;
}

void border_se_ext_corner_dirt(unsigned char *pic, unsigned char *mirr) 
{
        *pic = RANDOM_ELEMENT(pool_ext_corner_dirt);

        *mirr = BIT_HORIZONTAL | BIT_VERTICAL;
}

