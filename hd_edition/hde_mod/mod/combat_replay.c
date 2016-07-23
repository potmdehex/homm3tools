// Created by John Åkerblom 2015-04-21, from hooked.c from 2015-01-29

#include "common.h"
#include "combat_replay.h"

#include "../hde/structures/hero.h"

#include <hook_utils.h>

#include <stdint.h>
#include <stdlib.h>

// HoMM 3 HD 2.0.exe
typedef void(__stdcall *enter_battle_t)(int a1, struct HDE_HERO *hero_attacking, struct HDE_ARMY *army_attacking,
    int a4, int a5, struct HDE_HERO *hero_defending, struct HDE_ARMY *army_defending,
    int a8, int a9, int a10);
typedef void(__stdcall *enter_mgr_t)(void *mgr);
typedef void(__stdcall *inputmgr_func_t)(int a1);
typedef void(__stdcall *cast_spell_t)(int a1);
typedef void(__stdcall *show_artifact_dialog_t)(int a1, int a2);

enter_battle_t orig_enter_battle = (enter_battle_t)NULL;
enter_mgr_t orig_enter_mgr = (enter_mgr_t)NULL;
inputmgr_func_t orig_inputmgr_func = (inputmgr_func_t)NULL;
cast_spell_t orig_cast_spell = (cast_spell_t)NULL;
show_artifact_dialog_t orig_show_artifact_dialog = (show_artifact_dialog_t)NULL;

#pragma pack(push, 1)

struct HDE_ARMY
{
    uint32_t types[7];
    uint32_t quantities[7];
};
#pragma pack(pop)

uint32_t *var_quick_combat;
static uint32_t *var_spell_casted;

static struct HDE_ARMY *army_attacking;
static struct HDE_HERO *hero_attacking;
static struct HDE_ARMY army_attacking_saved;
static struct HDE_HERO hero_attacking_saved;
static struct HDE_ARMY *army_defending;
static struct HDE_HERO *hero_defending;
static struct HDE_ARMY army_defending_saved;
static struct HDE_HERO hero_defending_saved;
static BOOL f_enable_enter_mgr_hook;
static BOOL f_do_battle_replay;
static BOOL f_in_battle; // Needed to diffrentiate between combat and non combat spell casts

void __declspec(naked) hooked_enter_battle(void)
/*int a1,
struct HDE_HERO *hero_attacking,
struct HDE_ARMY *army_attacking,
int a4,
int a5,
struct HDE_HERO *hero_defending,
struct HDE_ARMY *army_defending,
int a8,
int a9,
int a10)*/
{
    //static uint32_t saved_ecx;
    static uint32_t *saved_retn;
    static uint32_t saved_eax;

    // Only do anything here if quick combat is enabled
    if (0 == *var_quick_combat)
    {
        __asm JMP orig_enter_battle
    }

    __asm MOV EAX, [ESP + 8]
    __asm MOV hero_attacking, EAX

    __asm MOV EAX, [ESP + 0xC]
    __asm MOV army_attacking, EAX

    __asm MOV EAX, [ESP + 0x18]
    __asm MOV hero_defending, EAX

    __asm MOV EAX, [ESP + 0x1C]
    __asm MOV army_defending, EAX

    __asm PUSHAD

    if (NULL != hero_attacking)
        memcpy(&hero_attacking_saved, hero_attacking, sizeof(struct HDE_HERO));
    else
        memcpy(&army_attacking_saved, army_attacking, sizeof(struct HDE_ARMY));

    if (NULL != hero_defending)
        memcpy(&hero_defending_saved, hero_defending, sizeof(struct HDE_HERO));
    else
        memcpy(&army_defending_saved, army_defending, sizeof(struct HDE_ARMY));

    __asm POPAD
    f_enable_enter_mgr_hook = TRUE;
    f_do_battle_replay = FALSE;
    f_in_battle = TRUE;

    // The enter_battle function uses adressing relative to ESP, so we cannot
    // modify the stack here, even a call to orig_enter_battle is out as that
    // puts an extra return address on the stack. Instead save the original
    // return adress, temporarily replace it with a new one 

    __asm MOV EAX, [ESP]
    __asm MOV saved_retn, EAX
    __asm ADD ESP, 4
    __asm CALL get_eip
get_eip :
    __asm POP EAX
    __asm ADD EAX, 0xB // 0xB = size of this instruction (4) + PUSH EAX (1), JMP orig_enter_battle (6)
    __asm PUSH EAX
    __asm JMP orig_enter_battle

    // We will return here after the function jumped to with JMP orig_enter_battle returns
    f_enable_enter_mgr_hook = FALSE;
    f_do_battle_replay = FALSE;
    f_in_battle = FALSE;

    // Restore previously saved return address and return
    __asm MOV saved_eax, EAX
    __asm MOV EAX, saved_retn
    __asm PUSH EAX
    __asm MOV EAX, saved_eax
    __asm RETN
}

void __declspec(naked) hooked_enter_mgr(void)
{
    static struct HDE_MGR *mgr;

    f_do_battle_replay = FALSE;

    __asm CMP f_enable_enter_mgr_hook, 0
    __asm JNZ hook
    __asm JMP orig_enter_mgr
hook :
    __asm MOV ECX, [ESP + 4]
    __asm MOV mgr, ECX

    orig_enter_mgr((void *)mgr);

    __asm PUSHAD

    // TODO remove excl
    if (FALSE == f_do_battle_replay)
    {
        hero_attacking = NULL;
        army_attacking = NULL;
        hero_defending = NULL;
        army_defending = NULL;
        f_enable_enter_mgr_hook = FALSE;

        __asm POPAD
        __asm RETN 4
    }

    // Coordinate check should not be needed here because if coordinates are different when doing this
    // restore the game is in an unknown state, but do it just in-case as otherwise a hero
    // duplication bug will occur when the game's internal state's instances of hero coordinates 
    // are desynced amongst each other.
    // In order to not mess up garrison quick combat replay, the first 20 bytes are not restored here.  

    if (NULL != hero_attacking && HDE_HERO_SAME_COORDINATES(hero_attacking, (&hero_attacking_saved)))
        memcpy((BYTE *)hero_attacking + 20, (BYTE *)&hero_attacking_saved + 20, sizeof(struct HDE_HERO) - 20);
    else if (NULL != army_attacking)
        memcpy(army_attacking, &army_attacking_saved, sizeof(struct HDE_ARMY));

    if (NULL != hero_defending && HDE_HERO_SAME_COORDINATES(hero_defending, (&hero_defending_saved)))
        memcpy((BYTE *)hero_defending + 20, (BYTE *)&hero_defending_saved + 20, sizeof(struct HDE_HERO) - 20);
    else if (NULL != army_defending)
        memcpy(army_defending, &army_defending_saved, sizeof(struct HDE_ARMY));
    
    hero_attacking = NULL;
    army_attacking = NULL;
    hero_defending = NULL;
    army_defending = NULL;

    f_enable_enter_mgr_hook = FALSE;
    f_do_battle_replay = FALSE;

    __asm POPAD
    *var_quick_combat = 0;
    orig_enter_mgr((void *)mgr);
    *var_quick_combat = 1;
    __asm RETN 4
}

#pragma pack(push, 1)
#define HDE_KEYDOWN 1 
#define HDE_KEYUP 2
struct HDE_KEY_INPUT
{
    int state; // 1 = HDE_KEYDOWN, 2 = HDE_KEYUP
    int vk;
};

#pragma pack(pop)

void __declspec(naked) hooked_inputmgr_func(void)
{
    static struct HDE_KEY_INPUT *inp;
    static void *saved_retn;

    __asm MOV ECX, [ESP]
    __asm MOV saved_retn, ECX
    __asm ADD ESP, 4

    __asm CALL orig_inputmgr_func

    __asm PUSHAD

    __asm MOV inp, EAX
    if (FALSE != f_enable_enter_mgr_hook
        // seems game never comes here when not in foreground
        && HDE_KEYUP == inp->state && VK_ESCAPE == inp->vk)
    {
        f_do_battle_replay = TRUE;
        if (NULL != var_spell_casted)
            *var_spell_casted = 0;
        OutputDebugStringA("f_do_battle_replay");
    }

    __asm POPAD

    __asm MOV ECX, saved_retn
    __asm PUSH saved_retn
    __asm RETN
}

static void __declspec(naked) hooked_cast_spell(void)
{
    // If called from outside of battle we should do nothing, we can crash because of invalid EBX
    // (simple 0 check on EBX does not cut it, instead of pointer it can have values like 768)
    __asm CMP f_in_battle, 0
    __asm JE orig
    __asm CMP EBX, 0
    __asm JE orig
    // Preserve var_spell_casted address, EDX register safe to trash
    __asm MOV EDX, DWORD PTR DS : [EBX + 0x1262C]
    __asm LEA EDX, DWORD PTR DS : [EDX * 4 + EBX + 0x54A0]
    __asm MOV var_spell_casted, EDX
orig:
    __asm JMP orig_cast_spell
}

void __declspec(naked) hooked_show_artifact_dialog(void)
{
    __asm PUSHAD
    if (FALSE != f_do_battle_replay)
    {
        __asm POPAD
        __asm RETN 8
    }
    __asm POPAD

    __asm JMP orig_show_artifact_dialog
}

void combat_replay_init(void)
{
    ///////////////////////////////////////////////////////////////////////////////////////////////
    // get_quick_combat: B0 01 C3 33 C0
    ///////////////////////////////////////////////////////////////////////////////////////////////
    unsigned char mem_get_quick_combat[] = {
        0xB0, 0x01, // MOV AL, 1
        0xC3,       // RETN
        0x33, 0xC0  // XOR EAX, EAX
    };
    int off_get_quick_combat = -0x59;
    ///////////////////////////////////////////////////////////////////////////////////////////////
    // enter_battle: 8D 84 24 28 02 00 00 64 A3 00 00 00 00
    ///////////////////////////////////////////////////////////////////////////////////////////////
    unsigned char mem_enter_battle[] = {
        0x8D, 0x84, 0x24, 0x28, 0x02, 0x00, 0x00,   // LEA EAX, [ESP + 0x228]
        0x64, 0xA3, 0x00, 0x00, 0x00, 0x00          // MOV DWORD PTR FS : [0], EAX
    };
    int off_enter_battle = -0x33;
    ///////////////////////////////////////////////////////////////////////////////////////////////
    // enter_mgr: C7 41 34 02 00 00 00 8B 49 44
    ///////////////////////////////////////////////////////////////////////////////////////////////
    unsigned char mem_enter_mgr[] = {
        0xC7, 0x41, 0x34, 0x02, 0x00, 0x00, 0x00, //MOV DWORD PTR DS : [ECX + 34], 2
        0x8B, 0x49, 0x44                          //MOV ECX, DWORD PTR DS : [ECX + 44]
    };
    int off_enter_mgr = -0x1F;
    ///////////////////////////////////////////////////////////////////////////////////////////////
    // inputmgr_func: 8B 87 38 08 00 00 40
    ///////////////////////////////////////////////////////////////////////////////////////////////
    unsigned char mem_inputmgr_func[] = {
        0x8B, 0x87, 0x38, 0x08, 0x00, 0x00, // MOV EAX, DWORD PTR DS : [EDI + 838]
        0x40                                // INC EAX
    };
    int off_inputmgr_func = -0xEE;
    ///////////////////////////////////////////////////////////////////////////////////////////////
    // cast_spell: 51 0F BF 41 18
    ///////////////////////////////////////////////////////////////////////////////////////////////
    unsigned char mem_cast_spell[] = {
        0x51,                   // PUSH ECX
        0x0F, 0xBF, 0x41, 0x18  // MOVSX EAX, WORD PTR DS : [ECX + 0x18]
    };
    int off_cast_spell = -0x3;
    ///////////////////////////////////////////////////////////////////////////////////////////////
    // show_artifact_dialog: C7 45 D4 0F 00 00 00 8B 30
    ///////////////////////////////////////////////////////////////////////////////////////////////
    unsigned char mem_show_artifact_dialog[] = {
        0xC7, 0x45, 0xD4, 0x0F, 0x00, 0x00, 0x00,   // MOV DWORD PTR SS : [EBP - 0x2C], 0x0F
        0x8B, 0x30                                  // MOV ESI, DWORD PTR DS : [EAX]
    }; 
    int off_show_artifact_dialog = -0x2E;
    ///////////////////////////////////////////////////////////////////////////////////////////////
    
    HOOK_NEEDLE_FAIL_MSG(NULL, enter_battle);
    HOOK_NEEDLE_FAIL_MSG(NULL, enter_mgr);
    HOOK_NEEDLE_FAIL_MSG(NULL, inputmgr_func);
    //HOOK_NEEDLE_FAIL_MSG(NULL, cast_spell);
    HOOK_NEEDLE_FAIL_MSG(NULL, show_artifact_dialog);

    // Extract var_quick_combat from its usage in the get_quick_combat routine, from here:
    // CMP DWORD PTR DS : [XXXXXXXX], EAX
    var_quick_combat = *(uint32_t **)(hook_find_by_needle(NULL, mem_get_quick_combat, sizeof(mem_get_quick_combat)) + 7);
}
