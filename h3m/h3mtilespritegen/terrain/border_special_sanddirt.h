/* Created by John Åkerblom 2014-8-25 */
#include "../crossplatform.h"
#include "../h3m_tile.h"

#if defined _WIN32 && NO_CRT
	#include "../win/advapi_rand.h"
	#include <windows.h>
	#define rand advapi_rand_byte
#else
	#include <stdlib.h>
#endif

/**
 * Generation of diagonals and corners that contain both sand and dirt
**/
#define PIC_ext_corner_sand_short_hori 0x2B
#define PIC_ext_corner_sand_short_vert 0x2C
#define PIC_ext_corner_sand_long_vert  0x2F
#define PIC_ext_corner_sand_long_hori  0x30
#define PIC_ext_diagonal_sand_short_hori PIC_ext_corner_sand_short_hori 
#define PIC_ext_diagonal_sand_short_vert PIC_ext_corner_sand_short_vert 
#define PIC_ext_diagonal_sand_long_vert  PIC_ext_corner_sand_long_vert 
#define PIC_ext_diagonal_sand_long_hori  PIC_ext_corner_sand_long_hori 

#define MIRR_SANDDIRT_ne(mirr) *mirr = BIT_VERTICAL
#define MIRR_SANDDIRT_nw(mirr) *mirr = BIT_HORIZONTAL | BIT_VERTICAL
#define MIRR_SANDDIRT_sw(mirr) *mirr = BIT_HORIZONTAL
#define MIRR_SANDDIRT_se(mirr) 

#define GENERATE_SINGLE_DIAGONAL_SPECIAL_SANDDIRT(dir, segment, variant)        \
        void border_##dir##_##segment##_special_##variant(                      \
                                                          unsigned char *pic,   \
                                                          unsigned char *mirr)  \
        {                                                                       \
                *pic = PIC_##segment##_##variant;                               \
                MIRR_SANDDIRT_##dir(mirr);                                      \
        }

#define GENERATE_ALL_VARIANTS_FOR_DIAGONAL(dir, segment)                        \
        GENERATE_SINGLE_DIAGONAL_SPECIAL_SANDDIRT(dir, segment, sand_short_hori)\
        GENERATE_SINGLE_DIAGONAL_SPECIAL_SANDDIRT(dir, segment, sand_short_vert)\
        GENERATE_SINGLE_DIAGONAL_SPECIAL_SANDDIRT(dir, segment, sand_long_hori) \
        GENERATE_SINGLE_DIAGONAL_SPECIAL_SANDDIRT(dir, segment, sand_long_vert)

#define GENERATE_ALL_DIAGONALS_FOR_SEGMENT(segment)                             \
        GENERATE_ALL_VARIANTS_FOR_DIAGONAL(nw, segment)                         \
        GENERATE_ALL_VARIANTS_FOR_DIAGONAL(ne, segment)                         \
        GENERATE_ALL_VARIANTS_FOR_DIAGONAL(sw, segment)                         \
        GENERATE_ALL_VARIANTS_FOR_DIAGONAL(se, segment)

GENERATE_ALL_DIAGONALS_FOR_SEGMENT(ext_diagonal)
GENERATE_ALL_DIAGONALS_FOR_SEGMENT(ext_corner)

/**
 * Generation of verticals, horizontals and double diagonals 
 * that contain both sand and dirt.
**/
#define PIC_vertical_halfsand   0x2E
#define PIC_horizontal_halfsand 0x2D
#define PIC_double_diagonal_halfsand 0x29

#define GENERATE_SINGLE_STRAIGHT_SPECIAL_SANDDIRT(dir, segment, variant,        \
                                                  mirr_bits)                    \
        void DLL_PUBLIC border_##dir##_##segment##_special_##variant(           \
                                                          unsigned char *pic,   \
                                                          unsigned char *mirr)  \
        {                                                                       \
                *pic = PIC_##segment##_halfsand;                                \
                *mirr = mirr_bits;                                              \
        }

GENERATE_SINGLE_STRAIGHT_SPECIAL_SANDDIRT(w, horizontal, sand_n, BIT_VERT | BIT_HORI)
GENERATE_SINGLE_STRAIGHT_SPECIAL_SANDDIRT(w, horizontal, sand_s, BIT_HORI)
GENERATE_SINGLE_STRAIGHT_SPECIAL_SANDDIRT(e, horizontal, sand_n, BIT_VERT)
GENERATE_SINGLE_STRAIGHT_SPECIAL_SANDDIRT(e, horizontal, sand_s, 0)

GENERATE_SINGLE_STRAIGHT_SPECIAL_SANDDIRT(n, vertical, sand_w, BIT_VERT | BIT_HORI)
GENERATE_SINGLE_STRAIGHT_SPECIAL_SANDDIRT(n, vertical, sand_e, BIT_VERT)
GENERATE_SINGLE_STRAIGHT_SPECIAL_SANDDIRT(s, vertical, sand_w, BIT_HORI)
GENERATE_SINGLE_STRAIGHT_SPECIAL_SANDDIRT(s, vertical, sand_e, 0)

GENERATE_SINGLE_STRAIGHT_SPECIAL_SANDDIRT(nwse, double_diagonal, sand_n, BIT_VERT)
GENERATE_SINGLE_STRAIGHT_SPECIAL_SANDDIRT(nwse, double_diagonal, sand_s, BIT_HORI)
GENERATE_SINGLE_STRAIGHT_SPECIAL_SANDDIRT(nesw, double_diagonal, sand_n, BIT_VERT | BIT_HORI)
GENERATE_SINGLE_STRAIGHT_SPECIAL_SANDDIRT(nesw, double_diagonal, sand_s, 0)

