#include "../crossplatform.h"

#if defined _WIN32 && NO_CRT
        #include "../win/advapi_rand.h"
        #include <windows.h>
        #define rand advapi_rand_byte
#else
        #include <stdlib.h>
#endif

static const unsigned char pool_standard_generic[]   = { 0x31, 0x32, 0x33, 0x34, 
                                                         0x35, 0x36, 0x37, 0x38 };
static const unsigned char pool_decorative_generic[] = { 0x39, 0x3A, 0x3B, 0x3C, 
                                                         0x3D, 0x3E, 0x3F, 0x41, 
                                                         0x40, 0x41, 0x42, 0x43, 
                                                         0x44, 0x45, 0x46, 0x47,
                                                         0x48 };

static const unsigned char pool_standard_dirt[]      = { 0x15, 0x16, 0x17, 0x18, 
                                                         0x19, 0x1A, 0x1B, 0x1C };
static const unsigned char pool_decorative_dirt[]    = { 0x1D, 0x1E, 0x1F, 0x20, 
                                                         0x21, 0x22, 0x23, 0x24,
                                                         0x25, 0x26, 0x27, 0x28, 
                                                         0x29, 0x2A, 0x2B, 0x2C };

static const unsigned char pool_standard_water[]     = { 0x14, 0x15, 0x16, 0x17, 
                                                         0x18, 0x19, 0x1A, 0x1B, 
                                                         0x1C, 0x1D, 0x1E, 0x1F, 
                                                         0x20 };
/* Water has no decorative pool */

static const unsigned char pool_standard_sand[]      = { 0x00, 0x01, 0x02, 0x03, 
                                                         0x04, 0x05, 0x06, 0x07, 
                                                         0x08, 0x09, 0x0A };
static const unsigned char pool_decorative_sand[]    = { 0x0B, 0x0C, 0x0D, 0x0E, 
                                                         0x0F, 0x10, 0x11, 0x12, 
                                                         0x13, 0x14, 0x15, 0x16,
                                                         0x17 };

/* All the 8 sprites in the rock standard set are just fully black tiles... */
static const unsigned char pool_standard_rock[]    = { 0x00, 0x01, 0x02, 0x03, 
                                                       0x04, 0x05, 0x06, 0x07 };

#define RANDOM_ELEMENT(a) a[(rand() + 1) % sizeof(a)] 

#define GENERATE_2POOL_FUNC(suffix) \
        void borderless_##suffix(unsigned char *pic) \
        { \
                if (0 != (rand() + 1) % 6) \
                        *pic = RANDOM_ELEMENT(pool_standard_##suffix); \
                else \
                        *pic = RANDOM_ELEMENT(pool_decorative_##suffix); \
        }

#define GENERATE_1POOL_FUNC(suffix) \
        void borderless_##suffix(unsigned char *pic) \
        { \
                *pic = RANDOM_ELEMENT(pool_standard_##suffix); \
        }

GENERATE_2POOL_FUNC(dirt)
GENERATE_2POOL_FUNC(sand)
GENERATE_2POOL_FUNC(generic)

GENERATE_1POOL_FUNC(water)
GENERATE_1POOL_FUNC(rock)

