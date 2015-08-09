// Created by John Åkerblom 2015-04-20, from hooked.c from 2015-01-29

#include "revisit.h"
#include "common.h"

#include "../hde/structures/hero.h"

#include <hook_utils.h>
#include <h3mlib.h>

#include <stdint.h>
#include <stdio.h>
#include <Windows.h>

#ifdef _MSC_VER
    #pragma warning(disable:4996)
#endif

// HoMM 3 HD 2.0.exe
typedef int (__stdcall *sub_801a0_t)(struct HDE_HERO *hero, int a2, int a3, int a4); // checks x, y, returns passable/impassable
typedef int (__stdcall *sub_7f030_t)(int a1, int a2, int a3, int a4); // visits object, returns movement points
typedef int (__stdcall *sub_a6a0_t)(int a1, int a2, int a3, int a4); // keypress func
typedef void (__stdcall *sub_b9840_t)(int a1, int a2, int a3, int a4, int a5, int a6); // fog write func
typedef void (__cdecl   *sub_baec0_t)(void); // edge crash func
typedef void (__stdcall *visit_object_t)(int a1, int a2);
typedef void (__stdcall *pre_visit_object_t)(void); // actually takes more args
typedef void (__stdcall *visit_warp_t)(int a1, int a2, int a3, int a4, int a5, int a6);

sub_801a0_t orig_sub_801a0 = (sub_801a0_t)NULL;
sub_7f030_t orig_sub_7f030 = (sub_7f030_t)NULL;
sub_a6a0_t orig_sub_a6a0 = (sub_a6a0_t)NULL;
sub_b9840_t orig_sub_b9840 = (sub_b9840_t)NULL;
sub_baec0_t orig_sub_baec0 = (sub_baec0_t)NULL;
pre_visit_object_t orig_pre_visit_object = (pre_visit_object_t)NULL;
visit_object_t orig_visit_object = (visit_object_t)NULL;
visit_warp_t orig_visit_warp = (visit_warp_t)NULL;

static struct HDE_HERO f_revisiting_hero_saved = { 0 };
static struct HDE_HERO *f_revisiting_hero = NULL;
static int f_anti_collision; // Used to prevent collision with monsters (starting battle) when revisiting

static BOOL f_warp_flag = FALSE; // Set when revisiting underground/monolith
static BOOL f_visit_flag = FALSE; // Set when an object is actually visited when doing revisit
static BOOL f_no_object_flag = FALSE; // Set when an object is NOT visited when doing revisit
static BOOL f_do_revisit = FALSE;

BOOL g_disable_revisit_refresh = FALSE;

//set y+1:
//0xfe01a0 (base + 0x801A0):
int __stdcall hooked_sub_801a0(struct HDE_HERO *hero, int a2, int a3, int a4)//, char a5)
{
    __asm PUSHAD
    if (FALSE != f_do_revisit)
    {
        if (0 == f_anti_collision)
        {
            f_revisiting_hero = hero;
            memcpy(&f_revisiting_hero_saved, hero, sizeof(f_revisiting_hero_saved));

            // Temporarily add some movement points, so that revisit can be performed
            // even if hero has none. (0x01A4 value taken from hero with movement points in-game)
            hero->movement_points = 0x01A4;
        }

        if (1 != f_anti_collision)
        {
            hero->y += 1;
        }

        ++f_anti_collision;
        __asm POPAD

        orig_sub_801a0(hero, a2, a3, a4);

        return 1; // Return that terrain is passable, regardless of impassable objects/terrain
    }
    __asm POPAD
    orig_sub_801a0(hero, a2, a3, a4);//, a5);
}

//restore movement:
//0x00fdf030 (base + 0x7f030):
int __stdcall hooked_sub_7f030(int a1, int a2, int a3, int a4)//, int a5)
{
    __asm PUSHAD
    f_anti_collision = 0;
    if (FALSE != f_do_revisit && NULL != f_revisiting_hero && FALSE == f_no_object_flag)
    {
        --f_revisiting_hero->y;
    }
    __asm POPAD

    orig_sub_7f030(a1, a2, a3, a4); // this causes object to be visited

    __asm PUSHAD
    if (FALSE != f_do_revisit && NULL != f_revisiting_hero && FALSE == f_no_object_flag)
    {
        ++f_revisiting_hero->y;
    }
    if (NULL != f_revisiting_hero)
    {
        f_revisiting_hero->direction_facing = f_revisiting_hero_saved.direction_facing;
    }
    __asm POPAD
}

int __stdcall hooked_sub_a6a0(int a1, int a2, int a3, int a4)
{
    static int vk; // static to avoid messing stack
    static void *saved_ecx;
    __asm PUSHAD
    __asm mov saved_ecx, ECX
    __asm MOV EAX, [EBP + 0x58]
    __asm mov vk, EAX
    if (VK_SPACE == vk && GetAsyncKeyState(VK_SPACE) & 0x8000)
    {
        __asm MOV EAX, 0x40000052
        __asm MOV [EBP + 0x58], EAX
        __asm MOV [EBP + 0x78], EAX
        f_do_revisit = TRUE;
        f_revisiting_hero = NULL;
        f_anti_collision = 0;
        f_warp_flag = FALSE;
        g_disable_revisit_refresh = TRUE;
    }
    f_visit_flag = FALSE;
    __asm POPAD

    orig_sub_a6a0(a1, a2, a3, a4);

    __asm PUSHAD
    if (VK_SPACE == vk && NULL != f_revisiting_hero)
    {
        if (!f_visit_flag)
        {
            f_no_object_flag = TRUE;
            __asm POPAD
            __asm mov ECX, saved_ecx
            orig_sub_a6a0(a1, a2, a3, a4);
            __asm PUSHAD
            f_no_object_flag = FALSE;
        }
        else
        {
            char dbg[64] = { 0 };
            sprintf(dbg, "%08X at %d,%d,%d", f_revisiting_hero, f_revisiting_hero_saved.x, f_revisiting_hero_saved.y, f_revisiting_hero_saved.z);
            OutputDebugStringA(dbg);

            if (!f_warp_flag) 
            {
                f_revisiting_hero->x = f_revisiting_hero_saved.x;
                f_revisiting_hero->y = f_revisiting_hero_saved.y;
            }
        }
        f_revisiting_hero->direction_facing = f_revisiting_hero_saved.direction_facing;
        f_revisiting_hero = NULL;
    }
    f_do_revisit = FALSE;
    f_warp_flag = FALSE;
    g_disable_revisit_refresh = FALSE;
    __asm POPAD
}

void __stdcall hooked_sub_b9840(int a1, int a2, int a3, int a4, int a5, int a6)
{
    if (FALSE != f_do_revisit && !f_warp_flag)
    {
        return;
    }
     
    orig_sub_b9840(a1, a2, a3, a4, a5, a6);
}

void __cdecl hooked_sub_baec0(void)
{
    __asm PUSHAD
    if (FALSE != f_do_revisit)
    {
        f_revisiting_hero->movement_points = f_revisiting_hero_saved.movement_points;
        __asm POPAD
        return;
    }
    __asm POPAD
    orig_sub_baec0();
}

void __declspec(naked) hooked_pre_visit_object(void)
{
    g_disable_revisit_refresh = FALSE;
    __asm JMP orig_pre_visit_object
}

void __declspec(naked) hooked_visit_object(void)
{
    __asm PUSHAD
    // Restting pos here too smooths out warp animation (otherwise hero is misplaced pre-warp)
    if (NULL != f_revisiting_hero) // underground/monolith
    {
        f_revisiting_hero->x = f_revisiting_hero_saved.x;
        f_revisiting_hero->y = f_revisiting_hero_saved.y;
    }
    f_visit_flag = TRUE;
    g_disable_revisit_refresh = FALSE;
    __asm POPAD
    __asm JMP orig_visit_object
}

void __declspec(naked) hooked_visit_warp(void)
{
    f_warp_flag = TRUE;
    f_visit_flag = TRUE;
    g_disable_revisit_refresh = FALSE;
    __asm JMP orig_visit_warp
}

void revisit_init(void)
{
    ///////////////////////////////////////////////////////////////////////////////////////////////
    // sub_801a0: 89 4D FC 03 DF 03 C8
    ///////////////////////////////////////////////////////////////////////////////////////////////
    unsigned char mem_sub_801a0[] = {
        0x89, 0x4D, 0xFC, //    MOV DWORD PTR SS : [EBP - 4], ECX
        0x03, 0xDF, //          ADD EBX, EDI
        0x03, 0xC8  //          ADD ECX, EAX
    };
    int off_sub_801a0 = -0x2B;
    ///////////////////////////////////////////////////////////////////////////////////////////////
    // sub_7f030: C6 86 84 03 00 00 01 66 01 1F 66 01 47 02
    ///////////////////////////////////////////////////////////////////////////////////////////////
    unsigned char mem_sub_7f030[] = {
        0xC6, 0x86, 0x84, 0x03, 0x00, 0x00, 0x01,   // MOV BYTE PTR [ESI + 0x384], 1
        0x66, 0x01, 0x1F,                           // ADD WORD PTR [EDI], BX
        0x66, 0x01, 0x47, 0x02                      // ADD WORD PTR [EDI + 2], AX
    };
    int off_sub_7f030 = -0x30;
    ///////////////////////////////////////////////////////////////////////////////////////////////
    // sub_b9840: 66 0F 6E C7 F3 0F E6 C0 0F B6 C0
    ///////////////////////////////////////////////////////////////////////////////////////////////
    unsigned char mem_sub_b9840[] = {
        0x66, 0x0F, 0x6E, 0xC7, // MOVD XMM0, EDI
        0xF3, 0x0F, 0xE6, 0xC0, // CVTDQ2PD XMM0, XMM0
        0x0F, 0xB6, 0xC0        // MOVZX EAX, AL
    };
    int off_sub_b9840 = -0x25;
    ///////////////////////////////////////////////////////////////////////////////////////////////
    // sub_baec0: 89 55 F8 0F BE 47 0B 8B 77 04
    ///////////////////////////////////////////////////////////////////////////////////////////////
    unsigned char mem_sub_baec0[] = {
        0x89, 0x55, 0xF8,       // MOV [EBP - 8], EDX
        0x0F, 0xBE, 0x47, 0x0B, // MOVSX EAX, BYTE PTR[EDI + 0x0B]
        0x8B, 0x77, 0x04        // MOV ESI, [EDI + 4]
    };
    int off_sub_baec0 = -0xE;
    ///////////////////////////////////////////////////////////////////////////////////////////////
    // sub_a6a0: 8B 47 44 8B 40 58 80 78 73 00
    ///////////////////////////////////////////////////////////////////////////////////////////////
    unsigned char mem_sub_a6a0[] = {
        0x8B, 0x47, 0x44,       // MOV EAX, [EDI + 0x44]
        0x8B, 0x40, 0x58,       // MOV EAX, DWORD PTR DS : [EAX + 58]
        0x80, 0x78, 0x73, 0x00  // CMP BYTE PTR [EAX + 0x73], 0
    };
    int off_sub_a6a0 = -0xE;
    ///////////////////////////////////////////////////////////////////////////////////////////////
    // pre_visit_object: 33 C4 89 84 24 34 03 00 00
    ///////////////////////////////////////////////////////////////////////////////////////////////
    unsigned char mem_pre_visit_object[] = {
        0x33, 0xC4,                              // XOR EAX, ESP
        0x89, 0x84, 0x24, 0x34, 0x03, 0x00, 0x00 // MOV DWORD PTR SS : [ESP + 0x334], EAX
    };
    int off_pre_visit_object = -0x1F;
    ///////////////////////////////////////////////////////////////////////////////////////////////
    // visit_object: C7 47 50 00 00 00 00 FF 36
    // Called after decision has been made that square contains an object, causes it to be visited
    ///////////////////////////////////////////////////////////////////////////////////////////////
    unsigned char mem_visit_object[] = {
        0xC7, 0x47, 0x50, 0x00, 0x00, 0x00, 0x00, // MOV DWORD PTR DS : [EDI + 0x50], 0
        0xFF, 0x36 };                             // PUSH DWORD PTR DS : [ESI]
    int off_visit_object = -0x33;
    ///////////////////////////////////////////////////////////////////////////////////////////////
    // select_hero: 80 BE 00 02 00 00 00 74 77
    ///////////////////////////////////////////////////////////////////////////////////////////////
    unsigned char mem_select_hero[] = {
        0x80, 0xBE, 0x00, 0x02, 0x00, 0x00, 0x00, // CMP BYTE PTR DS : [ESI + 0x200], 0
        0x74, 0x77                                // JE SHORT <+0x79>
    };
    int off_mem_select_hero = -0x1F;
    ///////////////////////////////////////////////////////////////////////////////////////////////
    // visit_warp: 38 4D 14 75 74
    ///////////////////////////////////////////////////////////////////////////////////////////////
    unsigned char mem_visit_warp[] = {
        0x38, 0x4D, 0x14, // CMP BYTE PTR SS : [EBP + 0x14], CL
        0x75, 0x74        // JNE SHORT <+0x74>
    };
    int off_visit_warp = -0x27;
    ///////////////////////////////////////////////////////////////////////////////////////////////
    // get_hero_pos: 88 46 11 0F B7 02
    ///////////////////////////////////////////////////////////////////////////////////////////////
    unsigned char mem_get_hero_pos[] = {
        0x88, 0x46, 0x11, // MOV BYTE PTR DS : [ESI + 0x11], AL
        0x0F, 0xB7, 0x02  // MOVZX EAX, WORD PTR DS : [EDX]
    };
    int off_get_hero_pos = -0x2D;
    ///////////////////////////////////////////////////////////////////////////////////////////////

    HOOK_NEEDLE_FAIL_MSG(NULL, sub_801a0);
    HOOK_NEEDLE_FAIL_MSG(NULL, sub_7f030);
    HOOK_NEEDLE_FAIL_MSG(NULL, sub_a6a0);
    HOOK_NEEDLE_FAIL_MSG(NULL, sub_b9840);
    HOOK_NEEDLE_FAIL_MSG(NULL, sub_baec0);

    HOOK_NEEDLE_FAIL_MSG(NULL, pre_visit_object);
    HOOK_NEEDLE_FAIL_MSG(NULL, visit_object);
    HOOK_NEEDLE_FAIL_MSG(NULL, visit_warp);

    // Inside function that is called when a new hero is selected, binary patch away
    // the code that resets that hero's facing direction. Without this patch, a hero
    // which space revisit has been used on will always faced north after another hero
    // is selected.
    // "MOV BYTE PTR DS : [EDI + 0x43], AL" - > NOP NOP NOP
    BYTE *orig_select_hero = hook_find_by_needle(NULL, mem_select_hero, sizeof(mem_select_hero)) + off_mem_select_hero;
    DWORD op = 0;
    VirtualProtect((LPVOID)(orig_select_hero + 0x71), 3, PAGE_EXECUTE_READWRITE, &op);
    *(orig_select_hero + 0x71) = 0x90;
    *(orig_select_hero + 0x72) = 0x90;
    *(orig_select_hero + 0x73) = 0x90;
    VirtualProtect((LPVOID)(orig_select_hero, + 0x71), 3, op, &op);
}
