#include <stdint.h>

#include <hook_utils.h>

#include "h3api.h"
#include "h3struct.h"
#include "trigger.h"

typedef int (__stdcall *object_visit_t)(struct H3STRUCT_HERO *hero, int a2, int a3, int a4);
typedef int (__stdcall *object_remove_t)(void *a1, uint32_t coords, int a3);
typedef int (__stdcall *seers_hut_visit_t)(void);
typedef int (__stdcall *quest_is_monster_defeated_t)(void);

object_visit_t orig_object_visit = (object_visit_t)NULL;
object_remove_t orig_object_remove = (object_remove_t)NULL;
seers_hut_visit_t orig_seers_hut_visit = (seers_hut_visit_t)NULL;
quest_is_monster_defeated_t orig_quest_is_monster_defeated = (quest_is_monster_defeated_t)NULL;

int __declspec(naked) hooked_object_visit(void) //(struct H3STRUCT_HERO *hero, a2, a3, a4)
{
    __asm PUSHAD
    static struct H3STRUCT_HERO *hero;
    static uint32_t coords;
    static int pass;
    static int ret;
    static int res;

    __asm MOV EAX, [ESP + 4 +0x20] // +0x20 because of __ASM PUSHAD
    __asm MOV hero, EAX

	coords = H3API_FORMAT_COORDS(hero->x, hero->y, hero->z);
	pass = 0;
	ret = 0;
	res = 0;
	
	res = trigger_dispatch(EVENT_OBJECT_VISIT, coords, (void *)hero, &pass, &ret);
	if (res == -1 || pass != 0) {
        __asm POPAD
		__asm JMP orig_object_visit
	}

    __asm int 3
    __asm POPAD
    __asm RETN 0x10
}

int __declspec(naked) hooked_object_remove(void) //(void *a1, uint32_t coords, int a3)
{
    __asm PUSHAD
    static uint32_t coords;
    static int pass;
    static int ret;
    static int res;

    __asm MOV EAX, [ESP + 8 +0x20] // +0x20 because of __ASM PUSHAD
    __asm MOV coords, EAX

	res = trigger_dispatch(EVENT_OBJECT_REMOVE, coords, NULL, &pass, &ret);
	if (res == -1 || pass != 0) {
        __asm POPAD
		__asm JMP orig_object_remove
	}

    __asm int 3
    __asm POPAD
    __asm RETN 0x0C
}

int __stdcall hooked_seers_hut_visit(void)
{
	//int pass = 0;
	//int ret = 0;
	//int res = 0;

	//res = trigger_dispatch(EVENT_OBJECT_VISIT, coords, NULL, &pass, &ret);
	//if (res == -1 || pass != 0) {
	//	return orig_object_remove();
	//}

	//return ret;
    return 0;
}

int __stdcall hooked_quest_is_monster_defeated(void)
{
	//int pass = 0;
	//int ret = 0;
	//int res = 0;

	//res = trigger_dispatch(EVENT_OBJECT_VISIT, coords, NULL, &pass, &ret);
	//if (res == -1 || pass != 0) {
	//	return orig_quest_is_monster_defeated();
	//}

	//return ret;
    return 0;
}

void hooks_init(void)
{
    ///////////////////////////////////////////////////////////////////////////////////////////////
    // object_visit : 64 A1 00 00 00 00 50 64 89 25 00 00 00 00 81 EC C8 03 00 00
    ///////////////////////////////////////////////////////////////////////////////////////////////
    unsigned char mem_object_visit[] = {
        0x64, 0xA1, 0x00, 0x00, 0x00, 0x00, 0x50, 0x64, 0x89, 0x25, 0x00, 0x00, 0x00, 0x00, 0x81, 
        0xEC, 0xC8, 0x03, 0x00, 0x00
    };
    int off_object_visit = -0x0A;
    ///////////////////////////////////////////////////////////////////////////////////////////////
    // object_remove : 89 75 F0 89 7D F4 0F 84 11 02 00 00
    ///////////////////////////////////////////////////////////////////////////////////////////////
    unsigned char mem_object_remove[] = {
        0x89, 0x75, 0xF0, 0x89, 0x7D, 0xF4, 0x0F, 0x84, 0x11, 0x02, 0x00, 0x00
    };
    int off_object_remove = -0x14;
    ///////////////////////////////////////////////////////////////////////////////////////////////
    // seers_hut_visit : 8B 3E 85 FF 0F 84 9D 02 00 00
    ///////////////////////////////////////////////////////////////////////////////////////////////
    unsigned char mem_seers_hut_visit[] = {
        0x8B, 0x3E, 0x85, 0xFF, 0x0F, 0x84, 0x9D, 0x02, 0x00, 0x00
    };
    int off_seers_hut_visit = -0x20;
    ///////////////////////////////////////////////////////////////////////////////////////////////
    // quest_is_monster_defeated : 55 8B EC 8B 45 08 0F BE 40 22
    ///////////////////////////////////////////////////////////////////////////////////////////////
    unsigned char mem_quest_is_monster_defeated[] = {
        0x55, 0x8B, 0xEC, 0x8B, 0x45, 0x08, 0x0F, 0xBE, 0x40, 0x22
    };
    int off_quest_is_monster_defeated = 0;
#if 0
    ///////////////////////////////////////////////////////////////////////////////////////////////
    // quest_is_hero_defeated : 55 8B EC 8B 45 08 8B D1 8A 48 22
    ///////////////////////////////////////////////////////////////////////////////////////////////
    unsigned char mem_quest_is_hero_defeated[] = {
        0x55, 0x8B, 0xEC, 0x8B, 0x45, 0x08, 0x8B, 0xD1, 0x8A, 0x48, 0x22
    };
    int off_quest_is_hero_defeated = 0;
#endif


    HOOK_NEEDLE_FAIL_MSG(NULL, object_visit);
    HOOK_NEEDLE_FAIL_MSG(NULL, object_remove);
    //HOOK_NEEDLE_FAIL_MSG(NULL, seers_hut_visit);
    //HOOK_NEEDLE_FAIL_MSG(NULL, quest_is_monster_defeated);
}
