/* Created by John Åkerblom 2014-08-25 */
#include "../crossplatform.h"
#include "../h3m_tile.h"

#if defined _WIN32 && NO_CRT
        #include "../win/advapi_rand.h"
        #include <windows.h>
        #define rand advapi_rand_byte
#else
        #include <stdlib.h>
#endif

#define RANDOM_ELEMENT(a) a[(rand() + 1) % sizeof(a)]

static const unsigned char pool_ext_corner_water[] = { 0x00, 0x01, 0x02, 0x03 };
static const unsigned char pool_horizontal_water[] = { 0x04, 0x05, 0x06, 0x07 };
static const unsigned char pool_vertical_water[]   = { 0x08, 0x09, 0x0A, 0x0B };
static const unsigned char pool_int_corner_water[] = { 0x0C, 0x0D, 0x0E, 0x0F }; 
static const unsigned char pool_ext_diagonal_water[] = { 0x10, 0x11 };
static const unsigned char pool_int_diagonal_water[] = { 0x12, 0x13 };

void border_n_vertical_water(unsigned char *pic, unsigned char *mirr) 
{
        *pic = RANDOM_ELEMENT(pool_vertical_water);
}

void border_s_vertical_water(unsigned char *pic, unsigned char *mirr) 
{
        *pic = RANDOM_ELEMENT(pool_vertical_water);

        *mirr = BIT_VERTICAL;
}

void border_w_horizontal_water(unsigned char *pic, unsigned char *mirr) 
{
        *pic = RANDOM_ELEMENT(pool_horizontal_water);
}

void border_e_horizontal_water(unsigned char *pic, unsigned char *mirr) 
{
        *pic = RANDOM_ELEMENT(pool_horizontal_water);

        *mirr = BIT_HORIZONTAL;
}

void border_nw_ext_diagonal_water(unsigned char *pic, unsigned char *mirr) 
{
        *pic = RANDOM_ELEMENT(pool_ext_diagonal_water);
}

void border_ne_ext_diagonal_water(unsigned char *pic, unsigned char *mirr) 
{
        *pic = RANDOM_ELEMENT(pool_ext_diagonal_water);

        *mirr = BIT_HORIZONTAL;
}

void border_sw_ext_diagonal_water(unsigned char *pic, unsigned char *mirr) 
{
        *pic = RANDOM_ELEMENT(pool_ext_diagonal_water);

        *mirr = BIT_VERTICAL;
}

void border_se_ext_diagonal_water(unsigned char *pic, unsigned char *mirr) 
{
        *pic = RANDOM_ELEMENT(pool_ext_diagonal_water);

        *mirr = BIT_HORIZONTAL | BIT_VERTICAL;
}

void border_nw_int_diagonal_water(unsigned char *pic, unsigned char *mirr) 
{
        *pic = RANDOM_ELEMENT(pool_int_diagonal_water);

        *mirr = BIT_VERTICAL | BIT_HORIZONTAL;
}

void border_ne_int_diagonal_water(unsigned char *pic, unsigned char *mirr)
{
        *pic = RANDOM_ELEMENT(pool_int_diagonal_water);

        *mirr = BIT_VERTICAL;
}

void border_sw_int_diagonal_water(unsigned char *pic, unsigned char *mirr) 
{
        *pic = RANDOM_ELEMENT(pool_int_diagonal_water);

        *mirr = BIT_HORIZONTAL;
}

void border_se_int_diagonal_water(unsigned char *pic, unsigned char *mirr) 
{
        *pic = RANDOM_ELEMENT(pool_int_diagonal_water);
}

void border_nw_int_corner_water(unsigned char *pic, unsigned char *mirr)
{
        *pic = RANDOM_ELEMENT(pool_int_corner_water);
}

void border_ne_int_corner_water(unsigned char *pic, unsigned char *mirr) 
{
        *pic = RANDOM_ELEMENT(pool_int_corner_water);

        *mirr = BIT_HORIZONTAL;
}

void border_sw_int_corner_water(unsigned char *pic, unsigned char *mirr) 
{
        *pic = RANDOM_ELEMENT(pool_int_corner_water);

        *mirr = BIT_VERTICAL;
}

void border_se_int_corner_water(unsigned char *pic, unsigned char *mirr) 
{
        *pic = RANDOM_ELEMENT(pool_int_corner_water);

        *mirr = BIT_HORIZONTAL | BIT_VERTICAL;
}

void border_nw_ext_corner_water(unsigned char *pic, unsigned char *mirr)
{
        *pic = RANDOM_ELEMENT(pool_ext_corner_water);
}

void border_ne_ext_corner_water(unsigned char *pic, unsigned char *mirr) 
{
        *pic = RANDOM_ELEMENT(pool_ext_corner_water);

        *mirr = BIT_HORIZONTAL;
}

void border_sw_ext_corner_water(unsigned char *pic, unsigned char *mirr) 
{
        *pic = RANDOM_ELEMENT(pool_ext_corner_water);

        *mirr = BIT_VERTICAL;
}

void border_se_ext_corner_water(unsigned char *pic, unsigned char *mirr) 
{
        *pic = RANDOM_ELEMENT(pool_ext_corner_water);

        *mirr = BIT_HORIZONTAL | BIT_VERTICAL;
}

