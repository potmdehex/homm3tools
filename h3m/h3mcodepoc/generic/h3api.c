#include "debug.h"
#include "h3api.h"
#include <hook_utils.h>

#include <stdint.h>

static void *orig_get_fog_ptr;

void h3api_fog_reveal(uint32_t coords)
{
	unsigned int x, y, z;
		
	if (orig_get_fog_ptr == NULL) {
		DEBUG_LOG("NULL function in h3api_fog_set, h3api_init not called?");
	}

	H3API_UNFORMAT_COORDS(coords, x, y, z)

	__asm MOV EDX, x
	__asm MOV ECX, y
	__asm PUSH z
	__asm CALL orig_get_fog_ptr
    __asm MOV [EAX], 1
}

void h3api_init(void)
{
	uint8_t needle_get_fog_ptr[] = {
		0x66, 0x8B, 0x04, 0x41, 0x5D, 0xC2, 0x04, 0x00, 0x90, 0x90, 0x90, 0x90,
		0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x55, 0x8B, 0xEC
	};
	int offset_get_fog_ptr = 0x13; // point down to next function from needle

    orig_get_fog_ptr = hook_find_by_needle(GetModuleHandle(NULL), needle_get_fog_ptr, sizeof(needle_get_fog_ptr)) + offset_get_fog_ptr;
}
