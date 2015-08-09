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

/* These pools define border pics for generic terrain meeting sand only */
static const unsigned char pool_ext_corner_sand_only[]   = { 0x14, 0x15, 0x16, 0x17 };
static const unsigned char pool_horizontal_sand_only[]   = { 0x18, 0x19, 0x1A, 0x1B };
static const unsigned char pool_vertical_sand_only[]     = { 0x1C, 0x1D, 0x1E, 0x1F };
static const unsigned char pool_int_corner_sand_only[]   = { 0x20, 0x21, 0x22, 0x23 }; 
static const unsigned char pool_ext_diagonal_sand_only[] = { 0x24, 0x25 };
static const unsigned char pool_int_diagonal_sand_only[] = { 0x26, 0x27 };

#define PIC_double_diagonal_sand_only 0x2A

#define RANDOM_ELEMENT(a) a[(rand() + 1) % sizeof(a)]
#define DOUBLE_DIAGONAL_SAND_ONLY(a) PIC_double_diagonal_sand_only

#define MIRR_SAND_ONLY_def_n(mirr)
#define MIRR_SAND_ONLY_def_s(mirr) *mirr = BIT_VERTICAL
#define MIRR_SAND_ONLY_def_w(mirr) 
#define MIRR_SAND_ONLY_def_e(mirr) *mirr = BIT_HORIZONTAL

#define MIRR_SAND_ONLY_def_nw(mirr) 
#define MIRR_SAND_ONLY_def_ne(mirr) *mirr = BIT_HORIZONTAL
#define MIRR_SAND_ONLY_def_sw(mirr) *mirr = BIT_VERTICAL
#define MIRR_SAND_ONLY_def_se(mirr) *mirr = BIT_HORIZONTAL | BIT_VERTICAL

/* int diagonals have their own mirroring scheme... */
#define MIRR_SAND_ONLY_int_diagonal_nw(mirr) *mirr = BIT_HORIZONTAL | BIT_VERTICAL
#define MIRR_SAND_ONLY_int_diagonal_ne(mirr) *mirr = BIT_VERTICAL
#define MIRR_SAND_ONLY_int_diagonal_sw(mirr) *mirr = BIT_HORIZONTAL
#define MIRR_SAND_ONLY_int_diagonal_se(mirr) 

#define MIRR_SAND_ONLY_def_nwse(mirr)
/* The game map editor always flips vertically rather than 
 * horizontally here, although either way looks pretty okay. */
#define MIRR_SAND_ONLY_def_nesw(mirr) *mirr = BIT_VERTICAL

#define GENERATE_SINGLE_SPECIAL_SAND_ONLY(mirr_prefix, dir, segment, pic_macro) \
        void border_##dir##_##segment##_special_sand_only(unsigned char *pic,   \
                                                          unsigned char *mirr)  \
        {                                                                       \
                *pic = pic_macro(pool_##segment##_sand_only);                   \
                MIRR_SAND_ONLY_##mirr_prefix##_##dir(mirr);                     \
        }                                                                       \

#define GENERATE_ALL_DIAGONALS_SAND_ONLY_FOR_SEGMENT(segment)                   \
        GENERATE_SINGLE_SPECIAL_SAND_ONLY(def, nw, segment, RANDOM_ELEMENT)   \
        GENERATE_SINGLE_SPECIAL_SAND_ONLY(def, ne, segment, RANDOM_ELEMENT)   \
        GENERATE_SINGLE_SPECIAL_SAND_ONLY(def, sw, segment, RANDOM_ELEMENT)   \
        GENERATE_SINGLE_SPECIAL_SAND_ONLY(def, se, segment, RANDOM_ELEMENT)

#define GENERATE_ALL_DIAGONALS_SPECIAL_SAND_ONLY()                              \
        /*GENERATE_ALL_DIAGONALS_SAND_ONLY_FOR_SEGMENT(int_diagonal)*/          \
        GENERATE_ALL_DIAGONALS_SAND_ONLY_FOR_SEGMENT(ext_diagonal)              \
        GENERATE_ALL_DIAGONALS_SAND_ONLY_FOR_SEGMENT(int_corner)                \
        GENERATE_ALL_DIAGONALS_SAND_ONLY_FOR_SEGMENT(ext_corner)

GENERATE_ALL_DIAGONALS_SPECIAL_SAND_ONLY() 
GENERATE_SINGLE_SPECIAL_SAND_ONLY(def, n, vertical, RANDOM_ELEMENT) 
GENERATE_SINGLE_SPECIAL_SAND_ONLY(def, s, vertical, RANDOM_ELEMENT) 
GENERATE_SINGLE_SPECIAL_SAND_ONLY(def, w, horizontal, RANDOM_ELEMENT) 
GENERATE_SINGLE_SPECIAL_SAND_ONLY(def, e, horizontal, RANDOM_ELEMENT) 

GENERATE_SINGLE_SPECIAL_SAND_ONLY(def, nwse, double_diagonal, DOUBLE_DIAGONAL_SAND_ONLY)
GENERATE_SINGLE_SPECIAL_SAND_ONLY(def, nesw, double_diagonal, DOUBLE_DIAGONAL_SAND_ONLY)

GENERATE_SINGLE_SPECIAL_SAND_ONLY(int_diagonal, nw, int_diagonal, RANDOM_ELEMENT)
GENERATE_SINGLE_SPECIAL_SAND_ONLY(int_diagonal, ne, int_diagonal, RANDOM_ELEMENT)
GENERATE_SINGLE_SPECIAL_SAND_ONLY(int_diagonal, sw, int_diagonal, RANDOM_ELEMENT)
GENERATE_SINGLE_SPECIAL_SAND_ONLY(int_diagonal, se, int_diagonal, RANDOM_ELEMENT)

