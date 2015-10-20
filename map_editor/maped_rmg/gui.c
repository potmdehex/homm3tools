// Created by John Åkerblom 2014-11-22

#include "gui.h"
#include "hooked.h"
#include "globals.h"

#include <h3mlib.h>
#include <string.h>
#include <stdio.h>
#include <commctrl.h>
#include <windows.h>

#pragma warning(disable:4996)

static LONG f_orig_main_proc = 0;

// h3maped's own IDs
#define ID_H3MAPED_RECENT_DOCUMENT 0xE111
#define ID_H3MAPED_NEW 0xE100
#define ID_H3MAPED_SAVE 0xE103
#define H3MAPED_CLASS_PREFIX "Afx:400000:8:"

// Tell h3maped that a recently opened document has been clicked
// and should thus be reopened. g_do_replace then tells NtCreateFile 
// hook to instead reload current map.
#define FORCE_MAP_RELOAD() \
    g_do_replace = 1; \
    CallWindowProc((WNDPROC)f_orig_main_proc, g_hwnd_main, WM_COMMAND, ID_H3MAPED_RECENT_DOCUMENT, 0); \
    g_do_replace = 0; 

// Tell h3maped that save has been pressed
#define FORCE_MAP_SAVE() \
    CallWindowProc((WNDPROC)f_orig_main_proc, g_hwnd_main, WM_COMMAND, MAKELONG(ID_H3MAPED_SAVE, 1), 0);

#define FORCE_MAP_NEW() \
    CallWindowProc((WNDPROC)f_orig_main_proc, g_hwnd_main, WM_COMMAND, MAKELONG(ID_H3MAPED_NEW, 1), 0);

static BOOL CALLBACK _EnumWindowsProc(HWND hwnd, LPARAM lParam)
{
    DWORD dwProcessId = 0;
    char str[256] = { 0 };

    GetClassNameA(hwnd, str, sizeof(str));
    GetWindowThreadProcessId(hwnd, &dwProcessId);
    if (0 == strncmp(str, H3MAPED_CLASS_PREFIX, sizeof(H3MAPED_CLASS_PREFIX)-1)
        && GetCurrentProcessId() == dwProcessId)
    {
        g_hwnd_main = hwnd;
        return FALSE;
    }

    return TRUE;
}

LRESULT CALLBACK new_main_WndProc(HWND hwnd, UINT Message, WPARAM wParam, LPARAM lParam)
{
    BOOL bChecked = FALSE;
    ShowWindow(hwnd, SW_HIDE);

    //OutputDebugStringA("in main wndproc");

    switch (Message)
    {
    case 0x1338:
        OutputDebugStringA("msg3");
        SendMessage(FindWindowA("Heroes III", NULL), 0x13371337, 0, 0);
        ExitProcess(0x1338);
        break;
        /*case 0x1338:
        SetTimer(NULL, 0, 2000, _TimerProc);
        OutputDebugStringA("msg2");
        break;*/
    case 0x1337:
        OutputDebugStringA("msg");
        //FORCE_MAP_NEW();
        FORCE_MAP_SAVE();
        break;
    case WM_COMMAND:
        switch (LOWORD(wParam))
        {
#if 0
        case ID_H3MSNAKE:
            OutputDebugStringA("right msg");
            FORCE_MAP_RELOAD();
            break;
#endif
        }
    }

    return CallWindowProc((WNDPROC)f_orig_main_proc, hwnd, Message, wParam, lParam);
}

static void _get_g_hwnd_main(void)
{
    // Retrieve window to g_hwnd_main
    for (;;)
    {
        EnumWindows(_EnumWindowsProc, 0);

        if (NULL != g_hwnd_main)
        {
            break;
        }

        Sleep(50);
    }
}

void gui_init(void)
{
    HMENU hCurrent = NULL;

    _get_g_hwnd_main();

    f_orig_main_proc = SetWindowLong(g_hwnd_main, GWL_WNDPROC, (long)new_main_WndProc);
}
