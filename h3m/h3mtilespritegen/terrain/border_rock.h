/* Created by John Åkerblom 2014-09-19 */
#include "../crossplatform.h"

#if defined _WIN32 && NO_CRT
        #include "../win/advapi_rand.h"
        #include <windows.h>
        #define rand advapi_rand_byte
#else
        #include <stdlib.h>
#endif

/* Since the values here are sequential some smarter auto-generation 
 * could easily be used... */
static const unsigned char pool_nw_ext_corner_rock[]   = { 0x08, 0x09 };
static const unsigned char pool_ne_ext_corner_rock[]   = { 0x0A, 0x0B };
static const unsigned char pool_sw_ext_corner_rock[]   = { 0x0C, 0x0D };
static const unsigned char pool_se_ext_corner_rock[]   = { 0x0E, 0x0F };

static const unsigned char pool_w_horizontal_rock[]    = { 0x10, 0x11 };
static const unsigned char pool_e_horizontal_rock[]    = { 0x12, 0x13 };
static const unsigned char pool_n_vertical_rock[]      = { 0x14, 0x15 };
static const unsigned char pool_s_vertical_rock[]      = { 0x16, 0x17 };

static const unsigned char pool_nw_int_corner_rock[]   = { 0x18, 0x19 };
static const unsigned char pool_ne_int_corner_rock[]   = { 0x1A, 0x1B };
static const unsigned char pool_sw_int_corner_rock[]   = { 0x1C, 0x1D };
static const unsigned char pool_se_int_corner_rock[]   = { 0x1E, 0x1F };

static const unsigned char pool_nw_ext_diagonal_rock[] = { 0x20, 0x21 };
static const unsigned char pool_ne_ext_diagonal_rock[] = { 0x22, 0x23 };
static const unsigned char pool_sw_ext_diagonal_rock[] = { 0x24, 0x25 };
static const unsigned char pool_se_ext_diagonal_rock[] = { 0x26, 0x27 };

/* Note: These are flipped compared to the rest */
static const unsigned char pool_se_int_diagonal_rock[] = { 0x28, 0x29 };
static const unsigned char pool_sw_int_diagonal_rock[] = { 0x2A, 0x2B };
static const unsigned char pool_ne_int_diagonal_rock[] = { 0x2C, 0x2D };
static const unsigned char pool_nw_int_diagonal_rock[] = { 0x2E, 0x2F };

#define RANDOM_ELEMENT(a) a[(rand() + 1) % sizeof(a)]

#define GENERATE_SINGLE_FUNCTION(infix) \
        void border_##infix##_rock(unsigned char *pic) \
        { \
                *pic = RANDOM_ELEMENT(pool_##infix##_rock); \
        }

#define GENERATE_DIAGONAL_FUNCTIONS(segment) \
        GENERATE_SINGLE_FUNCTION(nw_##segment) \
        GENERATE_SINGLE_FUNCTION(ne_##segment) \
        GENERATE_SINGLE_FUNCTION(sw_##segment) \
        GENERATE_SINGLE_FUNCTION(se_##segment)

GENERATE_SINGLE_FUNCTION(w_horizontal)
GENERATE_SINGLE_FUNCTION(e_horizontal)
GENERATE_SINGLE_FUNCTION(n_vertical)
GENERATE_SINGLE_FUNCTION(s_vertical)

GENERATE_DIAGONAL_FUNCTIONS(int_corner)
GENERATE_DIAGONAL_FUNCTIONS(ext_corner)
GENERATE_DIAGONAL_FUNCTIONS(int_diagonal)
GENERATE_DIAGONAL_FUNCTIONS(ext_diagonal)

