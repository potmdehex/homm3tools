// Created by John �kerblom 2015-04-20, from hooked.c from 2015-01-29

#include "stack_split.h"
#include "common.h"

#include <hook_utils.h>

#include "../hde/structures/hero.h"

typedef void (__stdcall *hero_select_stack_t)(void);
typedef void (__stdcall *town_select_stack_t)(void);
typedef int (__stdcall *swap_select_stack_t)(int a1);
typedef void (__stdcall *move_stack_t)(int src_slot, uint32_t *dest_creature_types, int dest_slot); // Used in hero/town screen when moving stack
typedef void (__stdcall *swap_move_stack_t)(void); // Used in swap screen when moving stack

hero_select_stack_t orig_hero_select_stack = (hero_select_stack_t)NULL;
town_select_stack_t orig_town_select_stack = (town_select_stack_t)NULL;
swap_select_stack_t orig_swap_select_stack = (swap_select_stack_t)NULL;
move_stack_t orig_move_stack = (move_stack_t)NULL;
swap_move_stack_t orig_swap_move_stack = (swap_move_stack_t)NULL;

static BOOL f_disable_move_stack;
static BOOL f_town_do_special_move;

const unsigned int MAX_SLOT_COUNT = 7;

void __stdcall hooked_move_stack(int src_slot, uint32_t *dest_creature_types, int dest_slot)
{
    //uint32_t id;
    struct HDE_HERO *src_hero;
    struct HDE_HERO *dest_hero;

    if (FALSE != f_disable_move_stack)
    {
        f_disable_move_stack = FALSE;
        return;
    }

    __asm PUSHAD

    //__asm MOV id, [ECX-0x8D]
    //if (0x000C000F == id)
    if (FALSE != f_town_do_special_move)
    {
        __asm SUB ECX, 0x8D // 0x8D == sizeof HDE_HERO->id + HDE_HERO->unknown1
        __asm MOV src_hero, ECX
        dest_hero = (struct HDE_HERO *)((BYTE *)dest_creature_types - 0x8D);

        common_move_last_stack(src_hero, dest_hero, src_slot, dest_slot);

        f_town_do_special_move = FALSE;
        return;
    }

    __asm POPAD

    orig_move_stack(src_slot, dest_creature_types, dest_slot);
}

int split_stack(int selected_slot, uint32_t *types, uint32_t *quantities, int repeat, int max_split_count)
{
    uint32_t type = types[selected_slot];
    unsigned int split_count = 0;

    for (int j = 0; j <= repeat + 1; ++j)
    {
        int free_slot_idx = -1;

        for (unsigned int i = 0; i < MAX_SLOT_COUNT; ++i)
        {
            // If stack is not the selected stack and has 0 quantity / no type,
            // mark this slot as free slot to split into
            if (i != selected_slot && (0 == quantities[i] || 0xFFFFFFFF == types[i]))
            {
                free_slot_idx = i;
                break;
            }
        }

        // If there is no free slot OR quantity of selected slot is below 2 OR split_count matches limit,
        // return
        if (-1 == free_slot_idx ||
            quantities[selected_slot] < 2 ||
            split_count == max_split_count)
        {
            return 0;
        }

        types[free_slot_idx] = type;
        quantities[free_slot_idx] = 1;
        quantities[selected_slot] -= 1;

        split_count++;
    }

    return 0;
}

int divide_stack(int selected_slot, uint32_t *types, uint32_t *quantities)
{
    uint32_t type = types[selected_slot];
    int free_slot_idx = -1;

    for (unsigned int i = 0; i < MAX_SLOT_COUNT; ++i)
    {
        // If stack is not the selected stack and has 0 quantity / no type,
        // mark this slot as free slot to divide into
        if (i != selected_slot && (0 == quantities[i] || 0xFFFFFFFF == types[i]))
        {
            free_slot_idx = i;
            break;
        }
    }

    if (free_slot_idx != -1 && quantities[selected_slot] > 1)
    {
        int total_creatures = quantities[selected_slot];
        int first_stack = total_creatures / 2;
        int second_stack = total_creatures - first_stack;

        types[free_slot_idx] = type;
        quantities[selected_slot] = first_stack;
        quantities[free_slot_idx] = second_stack;
    }

    return 0;
}

int join_stack(int selected_slot, uint32_t *types, uint32_t *quantities)
{
    uint32_t type = types[selected_slot];

    for (int i = 0; i < 7; ++i)
    {
        // If stack is not the selected stack but is same type as selected,
        // join it into the selected stack
        if (i != selected_slot && types[i] == type)
        {
            quantities[selected_slot] += quantities[i];
            types[i] = 0xFFFFFFFF;
            quantities[i] = 0;
        }
    }

    return 0;
}

int delete_stack(int selected_slot, uint32_t *types, uint32_t *quantities)
{
    boolean is_last_stack = TRUE;

    for (unsigned int i = 0; i < MAX_SLOT_COUNT; ++i)
    {
        if (i != selected_slot && quantities[i] > 0)
        {
            is_last_stack = FALSE;
            break;
        }
    }

    if (!is_last_stack)
    {
        types[selected_slot] = 0xFFFFFFFF;
        quantities[selected_slot] = 0;
        return 0;
    }

    return 1;
}

boolean is_control_pressed()
{
    return (GetAsyncKeyState(VK_CONTROL) & 0x8000) != 0;
}

boolean is_shift_pressed()
{
    return (GetAsyncKeyState(VK_SHIFT) & 0x8000) != 0;
}

boolean is_alt_pressed()
{
    return (GetAsyncKeyState(VK_MENU) & 0x8000) != 0;
}

boolean is_delete_pressed()
{
    return (GetAsyncKeyState(VK_DELETE) & 0x8000) != 0;
}

// Important that this is stdcall since it is called from inline asm
int __stdcall logic_select_stack(struct HDE_HERO *hero, unsigned int selected_slot, int repeat)
{
    // Basic bounds check for pointer
    if ((uint32_t)hero < 1000 || (uint32_t)hero > 0x7FFFFFFF)
    {
        return 1;
    }

    // The second argument, selected_slot, can actually be pointer to second hero if clicking on a slot of other hero.
    // In that case we should do nothing here. Would be cleaner to put proper name and type for 2nd arg but this works.
    if (selected_slot > MAX_SLOT_COUNT)
    {
        // Other hero was clicked while stack was selected, let game handle it normally
        return 1;
    }

    // & 0x8000 -> being pressed, & 1 -> pressed since last GetAsyncKeyState call
    if (is_control_pressed())
    {
        if (is_shift_pressed())
        {
            return split_stack(selected_slot, hero->creature_types, hero->creature_quantities, repeat, MAX_SLOT_COUNT);
        }
        return split_stack(selected_slot, hero->creature_types, hero->creature_quantities, repeat, 1);
    }
    else if (is_shift_pressed())
    {
        return divide_stack(selected_slot, hero->creature_types, hero->creature_quantities);
    }
    else if (is_alt_pressed())
    {
        return join_stack(selected_slot, hero->creature_types, hero->creature_quantities);
    }
    else if (is_delete_pressed())
    {
        return delete_stack(selected_slot, hero->creature_types, hero->creature_quantities);
    }

    return 1;
}

// Variables for hooked_hero_select_stack (don't use stack because of offsets)
int hero_select_stack_skip = 0;
void *retn_hero_select_stack = NULL;
void __declspec(naked) hooked_hero_select_stack(void)
{
    __asm PUSHAD // preserve for next call (this function is called twice for each select)

    // Call select stack logic function which will do hotkey actions
    __asm PUSH 6 // repeat count
    __asm PUSH EDX // slot idx
    __asm PUSH EAX // hero struct
    __asm CALL logic_select_stack

    // If return not 0, no hotkey was pressed, then skip selection prevention
    __asm CMP EAX, 0
    __asm JNE skip_selection_prevention

    // Selection prevention: Prevent stack from being selected by
    // setting retn address to that of stack move action
    __asm POPAD
    __asm MOV ECX, retn_hero_select_stack // usual return for stack move causes creature to not be selected
    __asm MOV [ESP], ECX
    __asm PUSHAD

    skip_selection_prevention:
    __asm POPAD // restore for this call
    orig_hero_select_stack();
    __asm RETN
}

void __declspec(naked) hooked_town_select_stack(void)
{
    // Arg 1 (ESP+4) is the type of action. 0 means select action which and
    // -1 means last stack move action, the only two actions we are interested in.
    // For anything except 0 & -1 jmp straight to original function.
    __asm MOV EAX, [ESP+4]
    __asm CMP EAX, 0
    __asm JE select_hook
    __asm CMP EAX, 0xFFFFFFFF
    __asm JE last_move_hook
    __asm JMP orig_town_select_stack
    last_move_hook:
    f_town_do_special_move = TRUE;
    __asm MOV DWORD PTR [ESP+4], 3 // Set action to move action
    __asm JMP orig_town_select_stack
    select_hook:
    //__asm MOV ESI, ECX
    __asm PUSHAD

    // Get pointer to creatures part of hero struct. Previously EDI was used instead of ECX
    // here, but while that works for towns, it does not work for garrisons
    __asm MOV EAX, [ECX+0x124] // this is offset in townManager to selected hero (garrison/visiting)
    __asm MOV EAX, [EAX+0x6C]
    __asm SUB EAX, 0x8D // -= sizeof(hero->unknown1)

    // Get selected slot
    __asm MOV EDX, [ECX+0x128]

    // Call select stack logic function which will do hotkey actions
    __asm PUSH 6 // repeat count
    __asm PUSH EDX // slot idx
    __asm PUSH EAX // hero struct
    __asm CALL logic_select_stack

    // If return not 0, no hotkey was pressed, then skip selection prevention
    __asm CMP EAX, 0
    __asm JNE skip_selection_prevention

    // Selection prevention: Prevent stack from being selected by
    // setting move action and fixing some pointers to not crash with
    // that change
    __asm POPAD
    __asm MOV [ESP+4], 3 // Set action to stack move action
    __asm PUSHAD

    // Fix pointers to not crash. These are dereferenced when pushing
    // arguments to the stack move function
    __asm MOV EAX, [ECX+0x124] // Get hero army pointer
    __asm MOV [ECX+0x12C], EAX // Need hero army pointer here to not crash
    __asm MOV [ECX+0x134], EAX // Need hero army pointer here to not crash

    // Prevent move function from actually doing anything. Then only function
    // updating screen will do something and everyone will be happy.
    f_disable_move_stack = TRUE;
#if 0
    __asm POPAD
    __asm MOV ECX, retn_town_select_stack // usual return for stack move causes creature to not be selected
    __asm MOV [ESP], ECX
    //__asm RETN 0x0C // the function this hook replaces actually is stdcall taking 3 args
    __asm PUSHAD
#endif

    skip_selection_prevention:
    __asm POPAD
    __asm JMP orig_town_select_stack
    //__asm RETN // dont need to cleanup stack here as original function did that
}

void __declspec(naked) hooked_swap_select_stack(void)
{
    __asm PUSHAD

    __asm PUSH 6 // repeat count
    __asm PUSH EBX // slot idx
    __asm PUSH EAX // hero struct
    __asm CALL logic_select_stack

    // If return not 0, not hotkey was pressed, then skip selection prevention
    __asm CMP EAX, 0
    __asm JNE skip_selection_prevention

    // Prevent selection
    __asm POPAD
    __asm MOV EAX, 0 // 0 in eax when returning prevents selection
    __asm RETN 4 // the function this hook replaces actually is stdcall taking 1 arg

    skip_selection_prevention:
    __asm POPAD
    __asm JMP orig_swap_select_stack
}

void __declspec(naked) hooked_swap_move_stack(void)
{
    __asm PUSHAD

    // Get src hero into ESI (same asm as in orig func)
    __asm MOV EAX, [ECX+0x48]
    __asm MOV ESI, [EAX*4+ECX+0x40]

    // Get dest hero into EDI (same asm as in orig func)
    __asm MOV EDX, [ECX+0x4C]
    __asm MOV EDI, [EDX*4+ECX+0x40]

    // Attempt special move. Will not return 0 if conditions are not met.
    __asm PUSH [ESP] // dest slot
    __asm PUSH -1 // src_slot, function will set it
    __asm PUSH EDI // dest hero
    __asm PUSH ESI // src hero
    __asm CALL common_move_last_stack

    // If return not 0, no special move was performed so jmp to orig func
    __asm CMP EAX, 0
    __asm JNE orig

    // Special move leaving 1 creature has been done, return without calling orig func
    __asm POPAD
    __asm RETN

    orig:
    __asm POPAD
    __asm JMP orig_swap_move_stack
}

void stack_split_init(void)
{
    ///////////////////////////////////////////////////////////////////////////////////////////////
    // hero_select_stack: 8B 01 75 44
    ///////////////////////////////////////////////////////////////////////////////////////////////
    unsigned char mem_hero_select_stack[] = {
        0x8B, 0x01,   // MOV EAX, DWORD PTR DS : [ECX]
        0x75, 0x44 }; // JNE SHORT <+0x44>
    int off_hero_select_stack = -0x21;
    ///////////////////////////////////////////////////////////////////////////////////////////////
    // town_select_stack: 83 F8 09 0F 87 EE 03 00 00
    ///////////////////////////////////////////////////////////////////////////////////////////////
    unsigned char mem_town_select_stack[] = {
        0x83, 0xF8, 0x09,                     // CMP EAX, 9
        0x0F, 0x87, 0xEE, 0x03, 0x00, 0x00 }; // JA <+0x3F4>
    int off_town_select_stack = -0x0F;
    ///////////////////////////////////////////////////////////////////////////////////////////////
    // swap_select_stack: 8A 84 07 88 C8 00 00 3A 84 1F 88 C8 00 00 0F 94 C0
    ///////////////////////////////////////////////////////////////////////////////////////////////
    unsigned char mem_swap_select_stack[] = {
        0x8A, 0x84, 0x07, 0x88, 0xC8, 0x00, 0x00, // MOV AL, [EDI + EAX + 0x0C888]
        0x3A, 0x84, 0x1F, 0x88, 0xC8, 0x00, 0x00, // CMP AL, [EDI + EBX + 0x0C888]
        0x0F, 0x94, 0xC0 };                       // SETZ AL
    int off_swap_select_stack = -0x33;
    ///////////////////////////////////////////////////////////////////////////////////////////////
    // move_stack: 89 04 99 8B 44 B7 1C
    ///////////////////////////////////////////////////////////////////////////////////////////////
    unsigned char mem_move_stack[] = {
        0x89, 0x04, 0x99,      // MOV DWORD PTR DS : [EBX * 4 + ECX], EAX
        0x8B, 0x44, 0xB7, 0x1C // MOV EAX, [EDI + ESI * 4 + 0x1C]
    };
    int off_move_stack = -0x19;
    ///////////////////////////////////////////////////////////////////////////////////////////////
    // swap_move_stack: 8B 41 50 81 C6 8D 00, 00, 00
    ///////////////////////////////////////////////////////////////////////////////////////////////
    unsigned char mem_swap_move_stack[] = {
        0x8B, 0x41, 0x50,                  // MOV EAX, [ECX+0x50]
        0x81, 0xC6, 0x8D, 0x00, 0x00, 0x00 // ADD ESI, 0x8D
    };
    int off_swap_move_stack = -0x26;
    ///////////////////////////////////////////////////////////////////////////////////////////////
    // retn_hero_select_stack: 81 C1 8D 00 00 00 51 52
    // NOTE! This is a retn address, not a function, and should be at this mov (addresses dynamic ofc):
    //    mov     dword ptr ds:12CBDF0h, 0FFFFFFFFh
    //    call    sub_4D8DC0
    //    jmp     loc_4DCEAF
    ///////////////////////////////////////////////////////////////////////////////////////////////
    unsigned char mem_retn_hero_select_stack[] = {
        0x81, 0xC1, 0x8D, 0x00, 0x00, 0x00, // ADD ECX, 0x8D
        0x51,                               // PUSH ECX
        0x52                                // PUSH EDX
    };
    int off_retn_hero_select_stack = 0x0D;
    ///////////////////////////////////////////////////////////////////////////////////////////////

    HOOK_NEEDLE_FAIL_MSG(NULL, hero_select_stack);
    HOOK_NEEDLE_FAIL_MSG(NULL, town_select_stack);
    HOOK_NEEDLE_FAIL_MSG(NULL, swap_select_stack);
    HOOK_NEEDLE_FAIL_MSG(NULL, move_stack);
    HOOK_NEEDLE_FAIL_MSG(NULL, swap_move_stack);

    retn_hero_select_stack = hook_find_by_needle(NULL, mem_retn_hero_select_stack, sizeof(mem_retn_hero_select_stack)) + off_retn_hero_select_stack;
}
