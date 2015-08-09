#include "hooked.h"
#include "patches.h"
#include "gui.h"

#include "h3msnake.h"

#include <Windows.h>
#include <commctrl.h>

#include "globals.h"

#define BEING_OR_BEEN_PRESSED 0x0001 

static DWORD f_keycode;
static HHOOK f_hhk;

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
    case VK_F5:
        // TODO actually remove timer here, otherwise speed gets bugged if not
        // dead yet.
        PostThreadMessage(g_main_tid, WM_RESTART, 0, 0);
        return 1;
    case VK_LEFT:
    case VK_RIGHT:
    case VK_UP:
    case VK_DOWN:
        InterlockedExchange(&f_keycode, kbd->vkCode);
        return 1;
    default:
        break;
    }

next:
    return CallNextHookEx(f_hhk, nCode, wParam, lParam);
}


VOID CALLBACK _TimerProc(
    HWND hwnd,        // handle to window for timer messages 
    UINT message,     // WM_TIMER message 
    UINT idTimer,     // timer identifier
    DWORD dwTime)     // current system time 
{
    DWORD keycode = InterlockedExchange(&f_keycode, f_keycode);
    enum H3MSNAKE_INPUT input
        = ((GetAsyncKeyState(VK_ESCAPE) & BEING_OR_BEEN_PRESSED) ? H3MSNAKE_INPUT_START
        : ((VK_UP == keycode) ? H3MSNAKE_INPUT_UP
        : ((VK_DOWN == keycode) ? H3MSNAKE_INPUT_DOWN
        : ((VK_LEFT == keycode) ? H3MSNAKE_INPUT_LEFT
        : ((VK_RIGHT == keycode) ? H3MSNAKE_INPUT_RIGHT
        : H3MSNAKE_INPUT_NONE)))));

    
    if (H3MSNAKE_STATE_DEAD == h3msnake_update_state(input))
    {
        OutputDebugStringA("Dead");
        KillTimer(NULL, idTimer);
    }

    SendMessage(g_hwnd_main, 0x1337, 0, 0);
}

DWORD WINAPI _thread(LPVOID lpParameter)
{
    MSG msg = { 0 };

    gui_init();

    f_hhk = SetWindowsHookEx(WH_KEYBOARD_LL, (HOOKPROC)LowLevelKeyboardProc, GetModuleHandle(NULL), 0);

    g_main_tid = GetCurrentThreadId();

restart:
    h3msnake_init("h3msnake2_level.h3m");
    SendMessage(g_hwnd_main, 0x1337, 0, 0);

    f_keycode = 0;
    SetTimer(NULL, 0, 250, _TimerProc);

    while (0 != GetMessage(&msg, NULL, 0, 0))
    {
        if (msg.message == WM_RESTART)
        {
            h3msnake_exit();
            goto restart;
        }
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
        patches_apply();
        hooked_init();
        CloseHandle(CreateThread(NULL, 0, _thread, NULL, 0, NULL));
        break;
    }
    return TRUE;
}
