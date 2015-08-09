// Created by John Åkerblom 2015-04-21, from hooked.c from 2015-01-29

#include "fps_increase.h"

#include <hook_utils.h>

#include <stdlib.h>
#include <windows.h>

typedef void (__stdcall *sdl_timer_cb_t)(void);

sdl_timer_cb_t orig_sdl_timer_cb = (sdl_timer_cb_t)NULL;

void __stdcall hooked_sdl_timer_cb(void)
{
    // Set FPS to 60 (59) FPS (1000 / 0x11 AKA 1000 / 17)
    __asm MOV [ESI + 0xC], 0x11
    orig_sdl_timer_cb();
}

void fps_increase_init(void)
{
    ///////////////////////////////////////////////////////////////////////////////////////////////
    // sdl_timer_cb: 6A 00 6A 02 8D 44 24 18
    ///////////////////////////////////////////////////////////////////////////////////////////////
    unsigned char mem_sdl_timer_cb[] = {
        0x6A, 0x00,             // PUSH 0
        0x6A, 0x02,             // PUSH 2
        0x8D, 0x44, 0x24, 0x18  // LEA EAX, [ESP + 18]
    };
    int off_sdl_timer_cb = -0xAC;
    
    HOOK_NEEDLE_FAIL_MSG(NULL, sdl_timer_cb);
}
