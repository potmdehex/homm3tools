#ifndef __H3API_H_DEF__
#define __H3API_H_DEF__

#include <stdint.h>

#define H3API_FORMAT_COORDS(x, y, z) ( \
	x + \
	(y << 16) + \
	(((0 == z) ? 0 : 0x04) << 24) \
)
#define H3API_UNFORMAT_COORDS(c, x, y, z) \
	x = c & ~0xFFFFFF00; \
	y = ((c & ~0xFF00FFFF) >> 16); \
	z = ((c & ~0x00FFFFFF) == 0x04000000);

#define H3API_COMPARE_COORDS(a, b) ((a & ~0x0000FF00) == (b & ~0x0000FF00))

void h3api_fog_reveal(uint32_t coords);

#endif
