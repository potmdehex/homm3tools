#include <stdint.h>

#include <hook_utils.h>

#include "hooks.h"
#include "h3api.h"
#include "h3struct.h"
#include "trigger.h"

typedef int (__stdcall *object_visit_t)(struct H3STRUCT_HERO *hero, int a2, int a3, int a4);
typedef int (__stdcall *object_remove_t)(void *a1, uint32_t coords, int a3);
typedef int (__stdcall *seers_hut_visit_t)(void);
typedef int (__stdcall *quest_is_monster_defeated_t)(void);
typedef int (__stdcall *match_main_func_t)(void);

object_visit_t orig_object_visit = (object_visit_t)NULL;
void *preserved_object_visit = NULL;
object_remove_t orig_object_remove = (object_remove_t)NULL;
void *preserved_object_remove = NULL;
seers_hut_visit_t orig_seers_hut_visit = (seers_hut_visit_t)NULL;
void *preserved_seers_hut_visit = NULL;
quest_is_monster_defeated_t orig_quest_is_monster_defeated = (quest_is_monster_defeated_t)NULL;
void *preserved_quest_is_monster_defeated = NULL;
match_main_func_t orig_match_main_func = (match_main_func_t)NULL;
void *preserved_match_main_func = NULL;

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

	coords = H3API_XYZ(hero->x, hero->y, hero->z);
	pass = 0;
	ret = 0;
	res = 0;
	
	res = trigger_dispatch(EVENT_OBJECT_VISIT, coords, (void *)hero, &pass, &ret);
	if (res == -1 || pass != 0) {
        __asm POPAD
		__asm JMP orig_object_visit
	}

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
	//	return orig_seers_hut_visit();
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

// TODO replace with less racy way of unloading
DWORD WINAPI _ExitThreadProc(LPVOID *lpParameter)
{
    hooks_uninit();
    Sleep(100);
    FreeLibraryAndExitThread(GetModuleHandleA("h3m_code.dll"), 0);
}

void __declspec(naked) hooked_match_main_func(void)
{
    __asm CALL orig_match_main_func

    __asm PUSHAD
    CloseHandle(CreateThread(NULL, 0, _ExitThreadProc, NULL, 0, NULL));
    __asm POPAD

    __asm RETN
}

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
///////////////////////////////////////////////////////////////////////////////////////////////
// match_main_func : 55 8B EC 83 EC 48 53 8B D9 33 C0 56 8B 0B
///////////////////////////////////////////////////////////////////////////////////////////////
unsigned char mem_match_main_func[] = {
    0x55, 0x8B, 0xEC, 0x83, 0xEC, 0x48, 0x53, 0x8B, 0xD9, 0x33, 0xC0, 0x56, 0x8B, 0x0B
};
int off_match_main_func = 0;

void hooks_init(void)
{
    HOOK_NEEDLE_FAIL_MSG_PRESERVE(NULL, object_visit);
    HOOK_NEEDLE_FAIL_MSG_PRESERVE(NULL, object_remove);
    HOOK_NEEDLE_FAIL_MSG_PRESERVE(NULL, match_main_func);
    //HOOK_NEEDLE_FAIL_MSG_PRESERVE(NULL, seers_hut_visit);
    //HOOK_NEEDLE_FAIL_MSG_PRESERVE(NULL, quest_is_monster_defeated);
}

void hooks_uninit(void)
{
    UNHOOK_PRESERVED(object_visit);
    UNHOOK_PRESERVED(object_remove);
    UNHOOK_PRESERVED(match_main_func);
    //UNHOOK_PRESERVED(seers_hut_visit);
    //UNHOOK_PRESERVED(quest_is_monster_defeated);
}
