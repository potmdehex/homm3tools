// Created by John Åkerblom 2014-11-29

#include <stdint.h>
#include <windows.h>

/**
 * A patch that makes it so that the map editor
 * does NOT reset the map view to coordinate 0,0
 * when opening (/re-opening) a map. 
 *
 */
static void _patch_reset_view(void)
{
    DWORD dwOldProtect = 0;
    static const char new_code[] = "\x53" // PUSH EBX
        "\x50" // PUSH EAX
        "\x8B\x5E\x60"; // MOV EBX,DWORD PTR DS:[ESI+60]

    //
    // h3maped has a data structure containing, amongst other things, x and y
    // coordinates of the map view. 
    // This patch accomplishes 2 things:
    // 1. Prevents x,y in structure from being set to 0 upon map open
    // 2. Makes it so current x,y is passed to SetScrollPos rather than 0
    //

    VirtualProtect((LPVOID)0x469ACC, (0x00469AD1 + (sizeof(new_code)-1)) - 0x469ACC, PAGE_EXECUTE_READWRITE, &dwOldProtect);

    //
    // FROM: MOV DWORD PTR DS:[ESI+5C],EAX
    // TO:   MOV EAX, DWORD PTR DS:[ESI+5C]
    //
    *((uint8_t *)0x469ACC) = 0x8B;

    //
    // FROM: MOV DWORD PTR DS:[ESI+60],ECX
    //       PUSH EBX
    //       PUSH EBX
    //  
    // TO:   PUSH EAX
    //       PUSH EBX
    //       MOV EBX, DWORD PTR DS:[ESI+60]
    //
    memcpy((void *)0x00469AD1, new_code, sizeof(new_code)-1);

    VirtualProtect((LPVOID)0x469ACC, 0x469ADF - 0x469ACC, dwOldProtect, &dwOldProtect);
}

void patches_apply(void)
{
    _patch_reset_view();
}
