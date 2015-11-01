#include "hooked.h"
#include "patches.h"
#include "gui.h"

#include <Windows.h>
#include <commctrl.h>

#include "globals.h"

static DWORD f_keycode;
static HHOOK f_hhk;

void _SelectToolbarBtn(int vkCode)
{
    TBBUTTON btn = { 0 };
    int idx = vkCode - 0x30; // 0x30 is the keycode for 0, after this comes 1-9
    char dbg[256];
    sprintf(dbg, "%d", idx);
    OutputDebugStringA(dbg);

    if (idx != 0)
        idx *= 2;
 
    SendMessage(g_hwnd_terrain_types, TB_GETBUTTON, idx, &btn);
    SendMessage(g_hwnd_main, WM_COMMAND, MAKELONG(btn.idCommand, BN_CLICKED), g_hwnd_terrain_types);
}

LRESULT CALLBACK LowLevelKeyboardProc(
    _In_  int nCode,
    _In_  WPARAM wParam,
    _In_  LPARAM lParam
    )
{
    static BOOL lock;
    KBDLLHOOKSTRUCT *kbd = (KBDLLHOOKSTRUCT *)lParam;
    
    if (HC_ACTION != nCode || WM_KEYDOWN != wParam || GetForegroundWindow() != g_hwnd_main)
    {
        goto next;
    }

    switch (kbd->vkCode)
    {
    case 0x30: // Digits 0-9
    case 0x31:
    case 0x32:
    case 0x33:
    case 0x34:
    case 0x35:
    case 0x36:
    case 0x37:
    case 0x38:
    case 0x39:
        _SelectToolbarBtn(kbd->vkCode);
        return 1;
    default:
        break;
    }

next:
    return CallNextHookEx(f_hhk, nCode, wParam, lParam);
}

DWORD WINAPI _thread(LPVOID lpParameter)
{
    MSG msg = { 0 };

    gui_init();
    f_hhk = SetWindowsHookEx(WH_KEYBOARD_LL, (HOOKPROC)LowLevelKeyboardProc, GetModuleHandle(NULL), 0);

    while (0 != GetMessage(&msg, NULL, 0, 0))
    {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }

    ExitThread(0);
}

BOOL WINAPI DllMain(HINSTANCE hInstDll, DWORD fdwReason, LPVOID lpReserved)
{
    switch (fdwReason)
    {
    case DLL_PROCESS_ATTACH:
        //patches_apply();

        g_map_filename = strdup("Untitled.h3m");
        hooked_init();
        CloseHandle(CreateThread(NULL, 0, _thread, NULL, 0, NULL));
        break;
    }
    return TRUE;
}
