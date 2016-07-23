// Created by John Åkerblom 2016-05-15

#include "common.h"

#include <hook_utils.h>

#include "../hde/structures/hero.h"

//battlefield action: 50 8D 84 24 C8 00 00 00 64 A3 00 00 00 00 8B F9 89 7C 24 18 8B 45 08

#define SPELL_ID_DISPEL     0x4E
#define SPELL_ID_WEAKNESS   0x2D

typedef int (__stdcall *cast_spell_t)(int spell_id, int a2, int a3, int a4, int a5, int a6);

static cast_spell_t orig_cast_spell = (cast_spell_t)NULL;

static int __stdcall hooked_cast_spell(int spell_id, int a2, int a3, int a4, int a5, int a6)
{
    uint32_t _retn;
    int ret;
    //__asm MOV EAX, [ESP]
    __asm MOV EAX, [ESP + 0x18] // because of prologue
    __asm MOV _retn, EAX

    __asm PUSHAD

    ret = orig_cast_spell(spell_id, a2, a3, a4, a5, a6);
    
    // check if this is a spell caused by a creature, not a hero
    if (*(uint32_t *)_retn == 0xFFFF06C7) // compare instruction at return address
    {
        // spell caused by creature

        // check for dispel cast, it means a dragon fly is attacking
        if (spell_id == SPELL_ID_DISPEL)
        {
            // apply Weakness as well, like in RoE 1.4 and SoD
            __asm POPAD
            orig_cast_spell(SPELL_ID_WEAKNESS, a2, a3, a4, a5, a6);
            __asm PUSHAD
        }
    }

    __asm POPAD

    return ret;
}

struct creature
{
    uint8_t unknown1[0x408];
    uint32_t *affecting_spells[3]; // pointer to array (double dereference)
    uint8_t unknown2[8];
    uint32_t affecting_spells_count;
};

void __stdcall creature_melee_attack(struct creature *target, int a2) // a2 == angle?
{
    struct creature *attacker = NULL;
    __asm MOV attacker, ECX



}

void sod_init(void)
{
    unsigned char mem_cast_spell[] = {
        0x89, 0x5C, 0x24, 0x54, 0x8B, 0x45, 0x0C, 0x8A, 
        0x4D, 0x10, 0x8B, 0xB3, 0x2C, 0x26, 0x01, 0x00
    };
    int off_cast_spell = -0x42;
    ///////////////////////////////////////////////////////////////////////////////////////////////

    HOOK_NEEDLE_FAIL_MSG(NULL, cast_spell);
}