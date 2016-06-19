// Created by John Åkerblom 2015-04-21, from hooked.c from 2015-01-29

#include "common.h"
#include "hero_trade.h"
#include "../hde/structures/hero.h"
#include "../hde/structures/swapmgr.h"

#include <hook_utils.h>

#include <Windows.h>

#define MOVABLE_WORN_MAX 16
#define BACKPACK_MAX 64
#define LEFT_HERO_WORN_SLOT_0 0x1B
#define RIGHT_HERO_WORN_SLOT_0 0x2D
#define LEFT_HERO_BACKPACK_SLOT_0 0x57
#define RIGHT_HERO_BACKPACK_SLOT_0 0x5C

#pragma pack(push, 1)
struct HDE_SWAPMGR_ACTION
{
    uint32_t action_id1; // Should be 0x200 for artifact slot click
    uint32_t action_id2; // Should be 0x0C for artifact slot click

    // Left  hero worn slot ids: 0x1B -> 0x2C, for left  hero's worn slot 0-17
    // Right hero worn slot ids: 0x2D -> 0x3E, for right hero's worn slot 0-17
    // Left  hero backpack slot ids: 0x57 -> 0x5B, for left  hero's backpack slot 0-4
    // Right hero backpack slot ids: 0x5C -> 0x60, for right hero's backpack slot 0-4
    // The backpack slots are the only 5 slots that are clickable.
    uint32_t slot_id;
};
#pragma pack(pop)

// HoMM 3 HD 2.0.exe
typedef void (__stdcall *swapmgr_display_t)(void); // Displays at least seconday skills
typedef int  (__stdcall *swapmgr_update_t)(int a1);
typedef void (__stdcall *swapmgr_backpack_action_t)(int a1, int a2, int a3); // Has MSVC thiscall convention, taking hero this pointer in ECX
typedef void (__stdcall *swapmgr_backpack_select_t)(int a1); // Has MSVC thiscall convention, taking hero this pointer in ECX
typedef void (__stdcall *swapmgr_artifact_stats_update_t)(void); // Actually takes 1 arg

swapmgr_update_t orig_swapmgr_update = (swapmgr_update_t)NULL;
swapmgr_display_t orig_swapmgr_display = (swapmgr_display_t)NULL;
swapmgr_backpack_action_t orig_swapmgr_backpack_action = (swapmgr_backpack_action_t)NULL;
swapmgr_backpack_select_t orig_swapmgr_backpack_select = (swapmgr_backpack_select_t)NULL;
swapmgr_artifact_stats_update_t orig_swapmgr_artifact_stats_update = (swapmgr_artifact_stats_update_t)NULL;

static BOOL f_disable_backpack_select;
BOOL g_disable_trade_refresh;
uint32_t *var_artifact_dragging = NULL; // Artifact currently dragged. Compared to -1 in swapmgr_backpack_action

// TODO fix artifact swapping bugs:
// 1. Sometimes the spellbook can stil be seen in backpack after the hacky refresh (but not used)
// 2. Backpack is not always refreshed. Ideally the spellbook hack should be trashed, it is 0/10
// 3. Pressing for example 1 two times leads to some artifacts being swapped back to hero 2

int swap_move_stack(struct HDE_HERO *src, struct HDE_HERO *dest, int src_slot, int dest_slot)
{
    // Try to do a last stack move, leaving only 1 creature behind.
    // Will only return 0 if conditions were met and a last stack move performed
    int ret = common_move_last_stack(src, dest, src_slot, dest_slot);

    if (0 == ret) {
        return 0;
    }
    else if (ERROR_DEST_SLOT_DIFFERENT == ret) {
        return 1;
    }
    else if (ERROR_NOT_LAST_STACK == ret) {
        dest->creature_quantities[dest_slot] += src->creature_quantities[src_slot];
        dest->creature_types[dest_slot] = src->creature_types[src_slot];
        src->creature_quantities[src_slot] = 0;
        src->creature_types[src_slot] = -1;
        return 2;
    }

    return 1;
}

void move_army(struct HDE_HERO *src, struct HDE_HERO *dest)
{
    int move_ret = 0;

    // Loop through stacks in source hero, moving them over to dest hero
    // Reverse loop as as simple solution to moving weakest creature last, 
    // as weakest is usually in first slot (methodology can be improved of course)
    for (int i = 6; i >= 0; --i) {
        if (-1 == src->creature_types[i] || 0 == src->creature_quantities[i]) {
            continue;
        }
        move_ret = -1;
        // See if there is already a stack of this type in dest hero to move into
        for (int j = 0; j < 7; ++j) {
            if (dest->creature_types[j] == src->creature_types[i]) {
                move_ret = swap_move_stack(src, dest, i, j);
                break;
            }
        }

        // Break if performed last stack move, continue if normal move
        if (0 == move_ret)
            break;
        else if (2 == move_ret)
            continue;

        // Try to do a move into same slot in dest as we are moving from in src
        if (-1 == dest->creature_types[i]) {
            move_ret = swap_move_stack(src, dest, i, i);
        }

        // Break if performed last stack move, continue if normal move
        if (0 == move_ret)
            break;
        else if (2 == move_ret)
            continue;

        // See if there is a free slot to move to.
        for (int j = 0; j < 7; ++j) {
            if (-1 == dest->creature_types[j]) {
                move_ret = swap_move_stack(src, dest, i, j);
                break;
            }
        }

        // Break if performed last stack move, continue if normal move.
        // if move_ret is still -1, still cannot break here as we might be able to 
        // move in other stacks into existing stacks of same types
        if (0 == move_ret)
            break;
        else if (2 == move_ret)
            continue;
    }
}

// FORCEINLINE is essential here - this function can only be used from swapmgr_update hook,
// with correct stack & register state
FORCEINLINE void move_artifacts(struct HDE_SWAPMGR *swapmgr, struct HDE_SWAPMGR_ACTION *action, int update_arg, int to_left)
{
    int src_worn_slot_0 = to_left ? RIGHT_HERO_WORN_SLOT_0 : LEFT_HERO_WORN_SLOT_0;
    int dst_worn_slot_0 = to_left ? LEFT_HERO_WORN_SLOT_0 : RIGHT_HERO_WORN_SLOT_0;
    int src_backpack_slot_0 = to_left ? RIGHT_HERO_BACKPACK_SLOT_0 : LEFT_HERO_BACKPACK_SLOT_0;
    int dst_backpack_slot_0 = to_left ? LEFT_HERO_BACKPACK_SLOT_0 : RIGHT_HERO_BACKPACK_SLOT_0;
    struct HDE_HERO *src_hero = to_left ? swapmgr->hero_right : swapmgr->hero_left;
    struct HDE_HERO *dst_hero = to_left ? swapmgr->hero_left : swapmgr->hero_right;
 
    g_disable_trade_refresh = TRUE;

    action->action_id1 = 0x200;
    action->action_id2 = 0x0C;

    for (int i = 0; i < MOVABLE_WORN_MAX; ++i)
    {
        if (-1 == src_hero->artifacts.worn[i])
            continue;
        
        // Pick up artifact
        action->slot_id = src_worn_slot_0 + i;
        __asm POPAD
        __asm MOV ECX, swapmgr
        orig_swapmgr_update(update_arg);
        __asm PUSHAD
        
        // Drop into same worn slot if possible, otherwise drop into backpack
        if (-1 == dst_hero->artifacts.worn[i])
        {
            action->slot_id = dst_worn_slot_0 + i;
        }
        else
        {
            //if (dst_hero->artifacts.backpack_count >= BACKPACK_MAX)
            //{
            //    continue;
            //}

            action->slot_id = dst_backpack_slot_0;
        }
        
        __asm POPAD
        __asm MOV ECX, swapmgr
        orig_swapmgr_update(update_arg);
        __asm PUSHAD

        src_hero->artifacts.worn[i] = -1;
    }

    for (int i = 0; i < BACKPACK_MAX; ++i)
    {
        if (-1 == src_hero->artifacts.backpack[i])
            continue;

        // Find free slot and move artifact there
        for (int j = 0; j < BACKPACK_MAX; ++j)
        {
            if (-1 == dst_hero->artifacts.backpack[j])
            {
                dst_hero->artifacts.backpack[j] = src_hero->artifacts.backpack[i];
                src_hero->artifacts.backpack[i] = -1;
                ++dst_hero->artifacts.backpack_count;
                --src_hero->artifacts.backpack_count;
                break;
            }
        }
    }

    g_disable_trade_refresh = FALSE;
}

void swap_army(struct HDE_HERO *h1, struct HDE_HERO *h2)
{
    struct HDE_HERO tmp = { 0 };
    size_t n = sizeof(tmp.creature_quantities) + sizeof(tmp.creature_types);

    // creature_quantities member follows creature_types in the structure
    memcpy(&tmp.creature_types, h1->creature_types, n);

    memcpy(h1->creature_types, h2->creature_types, n);
    memcpy(h2->creature_types, &tmp.creature_types, n);
}

void swap_backpacks(struct HDE_HERO *h1, struct HDE_HERO *h2)
{
    struct HDE_HERO tmp = { 0 };
    size_t n = sizeof(tmp.artifacts.backpack) + sizeof(tmp.artifacts.backpack_count);

    memcpy(&tmp.artifacts.backpack, &h1->artifacts.backpack, n);

    memcpy(h1->artifacts.backpack, h2->artifacts.backpack, n);
    memcpy(h2->artifacts.backpack, tmp.artifacts.backpack, n);
}

// FORCEINLINE is essential here - this function can only be used from swapmgr_update hook,
// with correct stack & register state
FORCEINLINE void swap_worn_artifacts(struct HDE_SWAPMGR *swapmgr, struct HDE_SWAPMGR_ACTION *action, int update_arg)
{
    g_disable_trade_refresh = TRUE;

    action->action_id1 = 0x200;
    action->action_id2 = 0x0C;

    for (int i = 0; i < MOVABLE_WORN_MAX; ++i)
    {
        action->slot_id = LEFT_HERO_WORN_SLOT_0 + i;

        __asm POPAD
        __asm MOV ECX, swapmgr
        orig_swapmgr_update(update_arg);
        __asm PUSHAD

        action->slot_id = RIGHT_HERO_WORN_SLOT_0 + i;

        __asm POPAD
        __asm MOV ECX, swapmgr
        orig_swapmgr_update(update_arg);
        __asm PUSHAD

        action->slot_id = LEFT_HERO_WORN_SLOT_0 + i;

        __asm POPAD
        __asm MOV ECX, swapmgr
        orig_swapmgr_update(update_arg);
        __asm PUSHAD
    }

    g_disable_trade_refresh = FALSE;
}

void swapmgr_reset_state(struct HDE_SWAPMGR *swapmgr)
{
    swapmgr->selected_owner = -1;
    swapmgr->inspected_owner = -1;
    swapmgr->selected_slot = -1;
    swapmgr->inspected_slot = -1;
    swapmgr->state = -1;
    *var_artifact_dragging = -1;
}


static void _spellbook_hack_add(struct HDE_SWAPMGR *swapmgr)
{
    // Terrible hack here...
    // Need an artifact in slot 0 for backpack redraw to work,
    // temporarily put a spellbook (0x00000000) there if nothing is there
    if (-1 == swapmgr->hero_left->artifacts.backpack[0])
    {
        swapmgr->hero_left->artifacts.backpack[0] = 0x00000000;
        ++swapmgr->hero_left->artifacts.backpack_count;
    }
    if (-1 == swapmgr->hero_right->artifacts.backpack[0])
    {
        swapmgr->hero_right->artifacts.backpack[0] = 0x00000000;
        ++swapmgr->hero_right->artifacts.backpack_count;
    }
}

static void _spellbook_hack_remove(struct HDE_SWAPMGR *swapmgr)
{
    // Terrible hack here...
    // Remove any spellbooks from backpack that are still there as the 
    // result of the refresh hack
    for (int i = 0; i < BACKPACK_MAX; ++i)
    {
        if (0x00000000 == swapmgr->hero_right->artifacts.backpack[i])
        {
            swapmgr->hero_right->artifacts.backpack[i] = 0xFFFFFFFF;
        }
        if (0x00000000 == swapmgr->hero_left->artifacts.backpack[0])
        {
            swapmgr->hero_left->artifacts.backpack[i] = 0xFFFFFFFF;
        }
    }
}

int __stdcall hooked_swapmgr_update(int a1)
{
    struct HDE_SWAPMGR *swapmgr;
    struct HDE_SWAPMGR_ACTION *action;
    static BOOL press_handled;
    BOOL do_refresh = FALSE;

    __asm PUSHAD

    __asm MOV swapmgr, ECX
    __asm MOV action, EDX

    if (GetForegroundWindow() == FindWindowA("SDL_app", WINDOW_TITLE)
        && -1 == *var_artifact_dragging) // Can't only do this when not dragging artifact, otherwise dragged artifact will disappear
    {
#if 1
        if (GetAsyncKeyState(VK_LBUTTON))
        {
            //__asm int 3
        }
#endif
        if (GetAsyncKeyState(0x31) & 0x8000) // 1 key
        {
            if (FALSE == press_handled)
            {
                swapmgr_reset_state(swapmgr);
                move_army(swapmgr->hero_right, swapmgr->hero_left);
                move_artifacts(swapmgr, action, a1, 1);

                press_handled = TRUE;
                do_refresh = TRUE;
            }
        }
        else if (GetAsyncKeyState(0x32) & 0x8000) // 2 key
        {
            if (FALSE == press_handled)
            {
                swapmgr_reset_state(swapmgr);
                move_army(swapmgr->hero_left, swapmgr->hero_right);
                move_artifacts(swapmgr, action, a1, 0);

                press_handled = TRUE;
                do_refresh = TRUE;
            }
        }
        else if (GetAsyncKeyState('Q') & 0x8000)
        {
            if (FALSE == press_handled)
            {
                swapmgr_reset_state(swapmgr);
                swap_army(swapmgr->hero_left, swapmgr->hero_right);
                swap_worn_artifacts(swapmgr, action, a1);
                swap_backpacks(swapmgr->hero_left, swapmgr->hero_right);

                press_handled = TRUE;
                do_refresh = TRUE;
            }
        }
        else
        {
            press_handled = FALSE;
        }

        if (do_refresh)
        {
            f_disable_backpack_select = TRUE;
            action->action_id1 = 0x200;
            action->action_id2 = 0x0C;
            action->slot_id = 0x57;

            // Terrible hack here...
            // Need an artifact in slot 0 for backpack redraw to work,
            // temporarily put a spellbook (0x00000000) there if nothing is there
            _spellbook_hack_add(swapmgr);

            // Now fall out to to this hook's final line where a normal update is called, causing refresh
        }
    }
    
    __asm POPAD
    __asm MOV ECX, swapmgr
    orig_swapmgr_update(a1);

    __asm PUSHAD
    _spellbook_hack_remove(swapmgr);
    __asm POPAD
}

void __stdcall hooked_swapmgr_backpack_action(int hero_idx, int slot_idx, int a3)
{
    __asm PUSHAD

    if (FALSE != f_disable_backpack_select)
    {
        orig_swapmgr_backpack_action(1, slot_idx, a3);
    }

    __asm POPAD
    orig_swapmgr_backpack_action(hero_idx, slot_idx, a3);

    f_disable_backpack_select = FALSE;
}

void __stdcall hooked_swapmgr_backpack_select(int a1)
{
    struct HDE_HERO *hero;
#if 1
    __asm PUSHAD
    __asm MOV hero, ECX

    if (FALSE != f_disable_backpack_select)
    {
        if (0 == hero->artifacts.backpack[0])
        {
            hero->artifacts.backpack[0] = -1;
            --hero->artifacts.backpack_count;
        }
        *var_artifact_dragging = -1;
        __asm POPAD
        return;
    }

    __asm POPAD
#endif
    orig_swapmgr_backpack_select(a1);
}

#if 0
void __declspec(naked) hooked_swapmgr_artifact_stats_update(void)
{
    if (g_disable_trade_refresh)
    {
        // Change return address to return directly to epilogue of calling function,
        // skipping mouse icon change code
        __asm MOV EAX, [ESP]
        __asm CMP AX, 0x33A0 // Hacky return address check. If it does not end with 33A0, then
        // we were called from somewhere else than the ESP we can add 0x167 to.
        __asm JNE skip
        __asm ADD EAX, 0x167
        __asm MOV [ESP], EAX
    }
skip:
    __asm JMP orig_swapmgr_artifact_stats_update
}
#endif

static int __declspec(noinline) _find_unused_slot(int taken_slots[8 + 1], int wanted_slot)
{
    for (int i = wanted_slot; i > 0; --i) {
        if (taken_slots[i] == 0)
            return i;
    }

    return -1;
}

void __declspec(naked) hooked_swapmgr_display(void)
{
    static struct HDE_HERO *hero;
    static uint32_t unused_slot;
    static int i;
    static int taken_slots[8+1];
    static int cur_slot;
    
    __asm MOV hero, ecx

    __asm PUSHAD

again:
    cur_slot = 0;
    memset(taken_slots, 0, sizeof(taken_slots));
    // go through valid skills, put them in taken_slots
    for (i = 0; i < sizeof(hero->secondary_skill_levels); ++i) {
        if (hero->secondary_skill_levels[i] != 0 && hero->secondary_skill_slot[i] != 0)
            taken_slots[hero->secondary_skill_slot[i]] = 1;
    }

    if (unused_slot <= hero->secondary_skill_count) {
        // go through skills finding invalid skill, putting it in unused slot
        for (i = 0; i < sizeof(hero->secondary_skill_levels); ++i) {
            if (hero->secondary_skill_levels[i] != 0 && hero->secondary_skill_slot[i] == 0) {
                hero->secondary_skill_slot[i] = _find_unused_slot(taken_slots, hero->secondary_skill_count);
                goto again;
            }
        }
    }

    __asm POPAD

    __asm JMP orig_swapmgr_display
}

void hero_trade_init(void)
{
    ///////////////////////////////////////////////////////////////////////////////////////////////
    // swapmgr_update: 88 5C 24 10 0F 95 44 24 14
    ///////////////////////////////////////////////////////////////////////////////////////////////
    unsigned char mem_swapmgr_update[] = {
        0x88, 0x5C, 0x24, 0x10,         // MOV [ESP + 0x10], BL
        0x0F, 0x95, 0x44, 0x24, 0x14    // SETNZ BYTE PTR [ESP + 0x14]
    };
    int off_swapmgr_update = -0x28;
    ///////////////////////////////////////////////////////////////////////////////////////////////
    // swapmgr_backpack_action: 89 44 24 14 8B 8C C7 B9 01 00 00 8B 94 C7 BD 01 00 00
    ///////////////////////////////////////////////////////////////////////////////////////////////
    unsigned char mem_swapmgr_backpack_action[] = {
        0x89, 0x44, 0x24, 0x14,                     // MOV [ESP+0x14], EAX
        0x8B, 0x8C, 0xC7, 0xB9, 0x01, 0x00, 0x00,   // MOV ECX, [EDI+EAX*8+0x1B9]
        0x8B, 0x94, 0xC7, 0xBD, 0x01, 0x00, 0x00    // MOV EDX, [EDI+EAX*8+0x1BD]
    };
    int off_swapmgr_backpack_action = -0x4A;
    ///////////////////////////////////////////////////////////////////////////////////////////////
    // swapmgr_backpack_select: 0F BF FE 8B D9
    ///////////////////////////////////////////////////////////////////////////////////////////////
    unsigned char mem_swapmgr_backpack_select[] = {
        0x0F, 0xBF, 0xFE,   // MOVSX EDI, SI
        0x8B, 0xD9          // MOV EBX, ECX
    };
    int off_swapmgr_backpack_select = -0x09;
    ///////////////////////////////////////////////////////////////////////////////////////////////
    // swapmgr_artifact_stats_update: 8B 94 C3 29 01 00 00 8D 34 C3
    ///////////////////////////////////////////////////////////////////////////////////////////////
    unsigned char mem_swapmgr_artifact_stats_update[] = {
        0x8B, 0x94, 0xC3, 0x29, 0x01, 0x00, 0x00, // MOV EDX, DWORD PTR DS : [EAX * 8 + EBX + 129]
        0x8D, 0x34, 0xC3                          // LEA ESI, [EAX * 8 + EBX]
    };
    int off_swapmgr_artifact_stats_update = -0x0D;
    ///////////////////////////////////////////////////////////////////////////////////////////////
    // swapmgr_display: 8B 94 C3 29 01 00 00 8D 34 C3
    ///////////////////////////////////////////////////////////////////////////////////////////////
    unsigned char mem_swapmgr_display[] = {
        0xBF, 0x01, 0x00, 0x00, 0x00,   // MOV EDI, 1
        0x74, 0x11,                     // JE SHORT 00C028FD
        0x80, 0x79, 0x10, 0x00,         // CMP BYTE PTR DS : [ECX + 10], 0
        0x74, 0x0B,                     // JE SHORT 00C028FD

    };
    int off_swapmgr_display = -0x0A; // -0x25 // 0x25 is real offset but we hook inside function
    
    HOOK_NEEDLE_FAIL_MSG(NULL, swapmgr_update);
    HOOK_NEEDLE_FAIL_MSG(NULL, swapmgr_backpack_action);
    HOOK_NEEDLE_FAIL_MSG(NULL, swapmgr_backpack_select);
    HOOK_NEEDLE_FAIL_MSG(NULL, swapmgr_display);
    //HOOK_NEEDLE_FAIL_MSG(NULL, swapmgr_artifact_stats_update);

    // var_artifact_dragging: This var is compared against -1 in backpack_action. It is the artifact currently being dragged.
    // Obtain from this instruction: CMP DWORD PTR DS : [XXXXXXXX], -1
    var_artifact_dragging = *(uint32_t **)(hook_find_by_needle(NULL, mem_swapmgr_backpack_action, sizeof(mem_swapmgr_backpack_action)) - 0x0F);
}
