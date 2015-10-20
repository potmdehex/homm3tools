// Created by John Åkerblom 2014-11-22

#include "gui.h"
#include "messages.h"
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
//#define H3MAPED_CLASS_PREFIX "Afx:400000:8:"
#define H3MAPED_CLASS_PREFIX "Afx:400000:8:"

// Tell h3maped that a recently opened document has been clicked
// and should thus be reopened. g_do_replace then tells NtCreateFile 
// hook to instead reload current map.
#define FORCE_MAP_RELOAD() \
    g_do_replace = 1; \
    CallWindowProc((WNDPROC)f_orig_main_proc, g_hwnd_main, WM_COMMAND, ID_H3MAPED_RECENT_DOCUMENT, 0);

// Tell h3maped that save has been pressed
#define FORCE_MAP_SAVE() \
    disable_NtCreateFile_hook = TRUE;  \
    CallWindowProc((WNDPROC)f_orig_main_proc, g_hwnd_main, WM_COMMAND, MAKELONG(ID_H3MAPED_SAVE, 1), 0);
    disable_NtCreateFile_hook = FALSE;

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

DWORD WINAPI _DelayedReload(LPVOID lp)
{
    // Oh boy. Sleep and double reload required to work. Worst hack 20xx.
    Sleep(100);

    disable_NtCreateFile_hook = TRUE;

    h3mlib_ctx_t h3m = NULL;
    uint32_t src_fmt;

    SetThreadPriority(GetCurrentThread(), THREAD_PRIORITY_TIME_CRITICAL);

    h3m_read_convert_u(&h3m, g_map_filename_w, g_new_format, &src_fmt, NULL, NULL, NULL, NULL);
    if (h3m != NULL) {
        h3m_write_u(h3m, g_map_filename_w);
        //h3m_write(h3m, "output.h3m");
        h3m_exit(&h3m);
    }
    else {
        OutputDebugStringA("There was an error opening the .h3m, trying to avoid crash");
    }

    g_convert_on_reload = FALSE;

    disable_NtCreateFile_hook = FALSE;

    SendMessage(g_hwnd_main, WM_RELOADMAP, 0, 0);
    ExitThread(0);
}

LRESULT CALLBACK new_main_WndProc(HWND hwnd, UINT Message, WPARAM wParam, LPARAM lParam)
{
    BOOL bChecked = FALSE;

    //OutputDebugStringA("in main wndproc");

    switch (Message)
    {
    case WM_RELOADMAP:
        OutputDebugStringA("RELOAD");
        FORCE_MAP_RELOAD();
        break;
    case WM_SAVERELOADMAP:
        OutputDebugStringA("SAVE & RELOAD");
        CloseHandle(CreateThread(NULL, 0, _DelayedReload, NULL, 0, NULL));
        break;
    case WM_SAVEMAP:
        OutputDebugStringA("SAVE");
        FORCE_MAP_SAVE();
        break;
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
