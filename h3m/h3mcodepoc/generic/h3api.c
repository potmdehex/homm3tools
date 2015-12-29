#include "debug.h"
#include "h3api.h"
#include <stdint.h>

static void *orig_get_fog_ptr;

void h3api_fog_reveal(uint32_t coords)
{
	int x, y, z;
	uint8_t *fog_ptr = NULL;
		
	if (orig_get_fog_ptr == NULL) {
		DEBUG_LOG("NULL function in h3api_fog_set, h3api_init not called?");
	}

	H3API_UNFORMAT_COORDS(coords, x, y, z)

	__asm MOV EDX, x
	__asm MOV ECX, y
	__asm PUSH 1
	__asm CALL orig_get_fog_ptr
	__asm MOV fog_ptr, EAX

	*fog_ptr = 1;
}

int h3api_init()
{
	uint8_t needle_get_fog_ptr[] = {
		0x66, 0x8B, 0x04, 0x41, 0x5D, 0xC2, 0x04, 0x00, 0x90, 0x90, 0x90, 0x90,
		0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x55, 0x8B, 0xEC
	};
	int offset_get_fog_ptr = -0x16;

	orig_get_fog_ptr;

	return 0;
}
