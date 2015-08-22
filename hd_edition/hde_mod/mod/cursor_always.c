// Created by John Åkerblom 2015-05-10

#include "common.h"
#include "cursor_always.h"

#include <hook_utils.h>

typedef HCURSOR (WINAPI *SetCursor_t)(HCURSOR hCursor);

SetCursor_t orig_SetCursor = (SetCursor_t)NULL;

HCURSOR WINAPI hooked_SetCursor(HCURSOR hCursor)
{
    // Prevent call if setting NULL cursor
    if (NULL == hCursor)
    {
        return NULL;
    }

    return orig_SetCursor(hCursor);
}

void cursor_always_init(void)
{
    hook_trampoline_dis_x86_by_name("user32.dll", "SetCursor", hooked_SetCursor, (void**)&orig_SetCursor);
}
