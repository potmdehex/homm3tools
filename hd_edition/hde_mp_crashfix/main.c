// Created by John Åkerblom 2015

#include <windows.h>
#include <psapi.h>

#include "debugpriv.h"

#define WND_TITLE "Heroes of Might & Magic III - HD Edition"
#define WND_CLASS "SDL_app"
static DWORD g_pid;

#pragma optimize("",off)
int __memcmp(const unsigned char *p1, const unsigned char *p2, size_t size)
{
    for (unsigned int i = 0; i < size; ++i)
    {
        if (p1[i] != p2[i])
        {
            return 1;
        }
    }
    return 0;
}
#pragma optimize("",on)

static BOOL CALLBACK _EnumWindowsProcGet_g_pid(HWND hwnd, LPARAM lParam)
{
    DWORD dwProcessId = 0;

    GetWindowThreadProcessId(hwnd, &dwProcessId);
    if (hwnd == (HWND)lParam)
    {
        g_pid = dwProcessId;
        return FALSE;
    }

    return TRUE;
}

static HANDLE _OpenGame(VOID)
{
    HWND hwnd = FindWindowA(WND_CLASS, WND_TITLE);
    HANDLE hProc = NULL;

    if (NULL == hwnd)
    {
        MessageBoxA(NULL, "No window with name \"" WND_TITLE "\" found", "No running HD Edition instance found", MB_ICONERROR);
        return NULL;
    }

    EnumWindows(_EnumWindowsProcGet_g_pid, (LPARAM)hwnd);

    SetDebugPrivilege(TRUE);

    if (NULL == (hProc = OpenProcess(PROCESS_ALL_ACCESS, FALSE, g_pid)))
    {
        MessageBoxA(NULL, "Failed to open HD Edition process, try running as admin", "Error", MB_ICONERROR);
        return NULL;
    }

    return hProc;
}

static BOOL _GetBaseAndSize(HANDLE hProc, BYTE **base, DWORD *size)
{
    HMODULE hBaseMod = NULL;
    DWORD cbNeeded = 0;
    MODULEINFO mi = { 0 };

    // First module returned is always base module, so instead of passing array just pass pointer to 
    // this single handle
    EnumProcessModules(hProc, &hBaseMod, sizeof(hBaseMod), &cbNeeded);

    if (NULL == hBaseMod)
    {
        return FALSE;
    }

    GetModuleInformation(hProc, hBaseMod, &mi, sizeof(mi));

    *base = mi.lpBaseOfDll;
    *size = mi.SizeOfImage;

    return TRUE;
}

static BOOL _ApplyPatch(HANDLE hProc, BYTE *mod_base, DWORD mod_size, BYTE *needle, DWORD needle_size, int needle_rel_offset, BYTE *patch, DWORD patch_size)
{
    BYTE *buf = HeapAlloc(GetProcessHeap(), HEAP_ZERO_MEMORY, needle_size);
    BYTE *p = mod_base;
    DWORD dwRead = 0;
    DWORD dwWritten = 0;
    DWORD dwOldProtect = 0;

    for (; p < (p + mod_size); ++p)
    {
        ReadProcessMemory(hProc, p, buf, sizeof(buf), &dwRead);

        if (0 == __memcmp(buf, needle, sizeof(needle)))
        {
            p += needle_rel_offset;
            VirtualProtectEx(hProc, p, patch_size, PAGE_EXECUTE_READWRITE, &dwOldProtect);
            WriteProcessMemory(hProc, p, patch, patch_size, &dwWritten);
            VirtualProtectEx(hProc, p, patch_size, dwOldProtect, &dwOldProtect);

            MessageBoxA(NULL, "MP crash fix patch applied!", "Success", MB_ICONINFORMATION);

            HeapFree(GetProcessHeap(), 0, buf);
            return TRUE;
        }
    }

    HeapFree(GetProcessHeap(), 0, buf);

    return FALSE;
}

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
    HANDLE hProc = NULL;
    BYTE *base = NULL;
    DWORD size = 0;
    // Needle: 0FBF4818894C2410
    // 0FBF4818    MOVSX ECX, WORD PTR DS : [EAX + 18]
    // 894C2410    MOV DWORD PTR SS : [ESP + 10], ECX
    BYTE needle[] = { 0x0F, 0xBF, 0x48, 0x18, 0x89, 0x4C, 0x24, 0x10 };
    // Patch： C21800
    // C21800      RETN 18
    BYTE patch[] = { 0xC2, 0x18, 0x00 }; 

    hProc = _OpenGame();
    _GetBaseAndSize(hProc, &base, &size);
    _ApplyPatch(hProc, base, size, needle, sizeof(needle), -0x23, patch, sizeof(patch));
    
    CloseHandle(hProc);

    return 0;
}

