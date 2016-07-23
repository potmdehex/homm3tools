#include "mod/common.h"

#include "mod/combat_replay.h"
#include "mod/cursor_always.h"
#include "mod/fps_increase.h"
#include "mod/hero_trade.h"
#include "mod/revisit.h"
#include "mod/rmg.h"
#include "mod/sod.h"
#include "mod/sound_nodelay.h"
#include "mod/stack_split.h"
#include "mod/xxl_maps.h"

#include <hook_utils.h>

#include <settings_utils.h>

#include <windows.h>

static const char *SETTING_NAMES[] = {
    "enable_cursor",
    "enable_fps",
    "enable_hotkeys",
    "enable_revisitable",
    "enable_rmg",
    "enable_sound",
    "enable_xxl"
};

enum SETTINGS
{
    SETTING_CURSOR,
    SETTING_FPS,
    SETTING_HOTKEYS,
    SETTING_REVISITABLE,
    SETTING_RMG,
    SETTING_SOUND,
    SETTING_XXL
};

#define SETTINGS_COUNT sizeof(SETTING_NAMES) / sizeof(const char *)
#define SETTINGS_PATH "Software\\h3minternals"

static BOOL _init_features(void)
{
    int values[SETTINGS_COUNT] = { 0 };
    int types[SETTINGS_COUNT] = { 0 }; // 0 == SETTINGS_INT

    // Set defaults
    memset(values, 1, sizeof(values)); // fine to set each byte to 1
    values[SETTING_RMG] = 0;
    values[SETTING_XXL] = 0;

    settings_get_n(SETTINGS_PATH, SETTINGS_COUNT, SETTING_NAMES, types,
        (void **)values, NULL);

    common_init();
    //sod_init();

    if (values[SETTING_CURSOR])
        cursor_always_init();
    if (values[SETTING_FPS])
        fps_increase_init();
    if (values[SETTING_HOTKEYS]) {
        combat_replay_init();
        hero_trade_init();
        stack_split_init();
    }
    if (values[SETTING_REVISITABLE])
        revisit_init();
    if (values[SETTING_RMG])
        rmg_init();
    if (values[SETTING_SOUND])
        sound_nodelay_init();
    if (values[SETTING_XXL])
        xxl_maps_init();

    return TRUE;
}

// This function is called at right after HOMM3 2.0.exe has decrypted itself
void hooked_SDL_SetMainReady(void)
{
    __asm PUSHAD
    _init_features();
    __asm POPAD
    // Seems fine to skip actually calling the original function here
}

BOOL WINAPI DllMain(HINSTANCE hinstDLL, DWORD fdwReason, LPVOID lpvReserved)
{
    BOOL bRet = TRUE;
    void *orig_SDL_SetMainReady = NULL;

	switch (fdwReason)
	{
    case DLL_PROCESS_ATTACH:
        // If we were launched from the extended launcher, don't init features
        // directly here as the executable has not yet decrypted many of the
        // functions to hook. Instead make an intermediatery hook that will
        // apply the rest of the hooks once the exe is decrypted.
        if (NULL != wcsstr(GetCommandLineW(), L"--delayhook")) {
            hook_trampoline_dis_x86_by_name("SDL2", 
                "SDL_SetMainReady", 
                hooked_SDL_SetMainReady, 
                (void **)&orig_SDL_SetMainReady);
        } else {
            bRet = _init_features();
        }
		break;
	case DLL_PROCESS_DETACH:
		break;
	case DLL_THREAD_ATTACH:
		break;
	case DLL_THREAD_DETACH:
		break;
	}

	return bRet;
}
