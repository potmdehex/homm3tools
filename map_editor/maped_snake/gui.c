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

HWND g_hwnd_main;
HWND f_hwndMinimap;

HWND g_hwnd_render; 

static LONG f_orig_main_proc = 0;
static LONG f_orig_terrain_dialog_proc = 0;

#define H3MAPED_CLASS_PREFIX "Afx:400000:8:"
#define ID_H3MSNAKE   GUI_ID_FIRST 

// h3maped's own IDs
#define ID_H3MAPED_RECENT_DOCUMENT 0xE111

#define ID_RESTART 0x1337
#define ID_ABOUT 0x1338

static int extra = 0;

// Tell h3maped that a recently opened document has been clicked
// and should thus be reopened. g_do_replace then tells NtCreateFile 
// hook to instead reload current map.
#define FORCE_MAP_RELOAD() \
    g_do_replace = 1; \
    ValidateRect(g_hwnd_main, NULL); \
    CallWindowProc((WNDPROC)f_orig_main_proc, g_hwnd_main, WM_COMMAND, ID_H3MAPED_RECENT_DOCUMENT + extra, 0); \
    ValidateRect(g_hwnd_main, NULL); \
    InvalidateRect(g_hwnd_render, NULL, FALSE); \
    UpdateWindow(g_hwnd_render); \
    extra = !extra;

static BOOL CALLBACK _EnumWindowsProc(HWND hwnd, LPARAM lParam)
{
    DWORD dwProcessId = 0;
    char str[256] = { 0 };

    GetClassNameA(hwnd, str, sizeof(str));
    GetWindowThreadProcessId(hwnd, &dwProcessId);
    if (0 == strncmp(str, H3MAPED_CLASS_PREFIX, sizeof(H3MAPED_CLASS_PREFIX)-1)
            && GetCurrentProcessId() ==  dwProcessId)
    {
        g_hwnd_main = hwnd;
        return FALSE;
    }

    return TRUE;
}

LRESULT CALLBACK new_main_WndProc(HWND hwnd, UINT Message, WPARAM wParam, LPARAM lParam)
{
    BOOL bChecked = FALSE;
    static BOOL disable_draw;

    switch (Message)
    {
    case WM_ERASEBKGND:
        return 1;
    case 0x1337:
        FORCE_MAP_RELOAD();
        break;
    case WM_COMMAND:
        switch (LOWORD(wParam))
        {
        case ID_H3MSNAKE:
            FORCE_MAP_RELOAD();
            break;
        case ID_ABOUT:
            MessageBoxA(NULL, "h3msnake2 by @potmdehex 2015", "http://h3minternals.net", MB_ICONINFORMATION);
            return 0;
        case ID_RESTART:
            PostThreadMessage(g_main_tid, WM_RESTART, 0, 0);
            return 0;
        }

        if (LOWORD(wParam) >= GUI_ID_FIRST && LOWORD(wParam) <= GUI_ID_LAST)
        {
            return 0;
        }
    }

    return CallWindowProc((WNDPROC)f_orig_main_proc, hwnd, Message, wParam, lParam);
}

LRESULT CALLBACK new_terrain_dialog_WndProc(HWND hwnd, UINT Message, WPARAM wParam, LPARAM lParam)
{
    return CallWindowProc((WNDPROC)f_orig_terrain_dialog_proc, hwnd, Message, wParam, lParam);
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
    HWND hwndContainer = NULL;
    HWND hwnd;

    _get_g_hwnd_main();

    if (NULL == g_hwnd_main)
    {
        OutputDebugStringA("main wnd not found");
        ExitProcess(1);
    }

    //ShowWindow(f_hwndMinimap, SW_HIDE);

retry:
    hwnd = g_hwnd_main;
    hwnd = GetWindow(hwnd, GW_CHILD);
    hwnd = GetWindow(hwnd, GW_CHILD);
    hwnd = GetWindow(hwnd, GW_CHILD);
    g_hwnd_render = hwnd;
    if (NULL == g_hwnd_render)
    {
        Sleep(50);
        goto retry;
    }

    ShowWindow(g_hwnd_main, SW_MAXIMIZE);
    SetFocus(g_hwnd_render);
    SetActiveWindow(g_hwnd_render);

    hCurrent = GetMenu(g_hwnd_main); // Get the CURRENT menu of the window
    g_ext_menu = CreatePopupMenu(); // Create a new one
    AppendMenuA(g_ext_menu, MF_STRING | MF_ENABLED, ID_RESTART, "Restart (F5)");
    AppendMenuA(g_ext_menu, MF_STRING | MF_ENABLED, ID_ABOUT, "About");
    AppendMenuA(hCurrent, MF_STRING | MF_POPUP, (UINT)g_ext_menu, "*** h3msnake");

    DrawMenuBar(g_hwnd_main);
    f_orig_main_proc = SetWindowLong(g_hwnd_main, GWL_WNDPROC, (long)new_main_WndProc);
}

