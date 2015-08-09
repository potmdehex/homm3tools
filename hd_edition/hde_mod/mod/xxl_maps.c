// Created by John Åkerblom 2015-06-02

#include <Windows.h>

#include <hook_utils.h>

unsigned char *orig_render_minimap = NULL;

void xxl_maps_init(void)
{
    // render_minimap: 74 3A 80 BE DA 00 00 00 00
    unsigned char mem_render_minimap[] = { 
        0x74, 0x3A, // JE SHORT <+0x34>
        0x80, 0xBE, 0xDA, 0x00, 0x00, 0x00, 0x00  // CMP BYTE PTR DS : [ESI + 0DA], 0
    };
    unsigned int off_render_minimap = -0x55;
    
    orig_render_minimap = hook_find_by_needle(NULL, mem_render_minimap, sizeof(mem_render_minimap)) + off_render_minimap;

    // Patch away minimap rendering to prevent crash on XXL maps
    unsigned char patch_render_minimap[] = { 0xC2, 0x18, 0x00 }; // RETN 18
    DWORD dwOldProtect = 0;
    VirtualProtect((LPVOID)orig_render_minimap, sizeof(patch_render_minimap), PAGE_EXECUTE_READWRITE, &dwOldProtect);
    memcpy(orig_render_minimap, patch_render_minimap, sizeof(patch_render_minimap));
    VirtualProtect((LPVOID)orig_render_minimap, sizeof(patch_render_minimap), dwOldProtect, &dwOldProtect);
}
