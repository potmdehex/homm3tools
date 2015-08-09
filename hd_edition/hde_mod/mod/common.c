// Created by John Åkerblom 2015-04-20, from hooked.c from 2015-01-29

#include "common.h"
#include "revisit.h"

#include "../hde/structures/hero.h"

#include <h3mlib.h>
#include <hook_utils.h>

#include <stdio.h>

#ifdef _MSC_VER
    #pragma warning(disable:4996)
#endif

typedef void (__stdcall *screen_refresh_t)(int a1, int a2, int a3, int a4);

screen_refresh_t orig_screen_refresh = (screen_refresh_t)NULL;

// This function makes a special stack move for move operations that are trying 
// to move the last stack in a hero's army to another hero, moving every creature
// in that stack over except 1 (normally the move is simply cancelled).
//
// stdcall to make it easier to call this function from assembly
int __stdcall common_move_last_stack(struct HDE_HERO *src, struct HDE_HERO *dest, int src_slot, int dest_slot)
{
    int stack_count = 0;

    // The special last stack is only for moving between heroes, do nothing
    // if this is a move within same hero army
    if (src == dest)
    {
        return ERROR_SAME_HERO;
    }

    // Iterate stacks in source hero, counting the stacks and preserving slot idx
    for (int i = 0; i < 7; ++i)
    {
        if (0xFFFFFFFF != src->creature_types[i] && 0 != src->creature_quantities[i])
        {
            ++stack_count;
            if (-1 == src_slot)
            {
                src_slot = i;
            }
        }
    }

    // If there is more than 1 stack behind or the single remaining stack has
    // less than two creatures or the destination slot is not same type/empty, 
    // do nothing
    if (dest->creature_types[dest_slot] != src->creature_types[src_slot]
        && (0xFFFFFFFF != dest->creature_types[dest_slot]
        || 0 != dest->creature_quantities[dest_slot]))
    {
        return ERROR_DEST_SLOT_DIFFERENT;
    }
    else if (stack_count > 1)
    {
        return ERROR_NOT_LAST_STACK;
    }
    else if (src->creature_quantities[src_slot] < 2)
    {
        return ERROR_SINGLE_CREATURE;
    }

    // Do the special move, leaving 1 creature behind
    dest->creature_types[dest_slot] = src->creature_types[src_slot];
    dest->creature_quantities[dest_slot] += src->creature_quantities[src_slot] - 1;
    src->creature_quantities[src_slot] = 1;

    return 0;
}

void __stdcall hooked_screen_refresh(int a1, int a2, int a3, int a4)
{
    extern BOOL g_disable_revisit_refresh;
    extern BOOL g_disable_trade_refresh;

    if (g_disable_trade_refresh || g_disable_revisit_refresh)
    {
        // Simply return without refreshing hero trade screen
        return;
    }

    orig_screen_refresh(a1, a2, a3, a4);
}

void common_init(void)
{
    ///////////////////////////////////////////////////////////////////////////////////////////////
    // screen_refresh: 89 4C 24 10 74 1A
    ///////////////////////////////////////////////////////////////////////////////////////////////
    unsigned char mem_screen_refresh[] = {
        0x89, 0x4C, 0x24, 0x10, // MOV DWORD PTR SS : [ESP + 10], ECX
        0x74, 0x1A              // JE SHORT <+0x1A>
    };
    int off_screen_refresh = -0x1E;

    HOOK_NEEDLE_FAIL_MSG(NULL, screen_refresh);
}
