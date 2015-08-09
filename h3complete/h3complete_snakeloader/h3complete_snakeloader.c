// Created by John Åkerblom 2015-08-06

#include <windows.h>
#include <shlwapi.h>

#include "inject_utils.c" // deliberate direct include because not linking
#include "maped_snake.h"

#ifdef _MSC_VER
#ifndef memset
    #pragma optimize("", off)
        FORCEINLINE void *__memset(void *s, int c, unsigned int n)
        {
            unsigned int i = 0;

            for (i = 0; i < n; ++i) {
                ((unsigned char *)s)[i] = c;
            }

            return s;
        }
        #define memset __memset
    #pragma optimize("", on)
#endif
#else
    #include <string.h>
#endif

static VOID SetDirectory(VOID)
{
    WCHAR szwPath[MAX_PATH];

    GetModuleFileNameW(GetModuleHandle(NULL), szwPath, sizeof(szwPath) / sizeof(szwPath[0]));

    PathRemoveFileSpecW(szwPath);
    SetCurrentDirectoryW(szwPath);
}

static VOID WriteSnake(VOID)
{
    HANDLE hFile = CreateFileA("maped_snake.dll", GENERIC_WRITE, 0, NULL, CREATE_ALWAYS, 0, NULL);
    DWORD dwWritten = 0;

    if (NULL == hFile)
    {
        MessageBoxA(NULL, "Failed to create maped_snake.dll", "RIP h3msnake", MB_ICONERROR);
        return;
    }

    WriteFile(hFile, maped_snake_dll, sizeof(maped_snake_dll), &dwWritten, NULL);

    CloseHandle(hFile);
}

static VOID LoadSnake(VOID)
{
    STARTUPINFOW si;
    PROCESS_INFORMATION pi;

    memset(&si, 0, sizeof(si));
    memset(&pi, 0, sizeof(pi));

    si.cb = sizeof(si);
    si.dwFlags = STARTF_USESHOWWINDOW;
    si.wShowWindow = SW_SHOWMAXIMIZED;

    CreateProcessW(L"h3maped.exe", NULL, NULL, NULL, FALSE, CREATE_SUSPENDED, NULL, NULL, &si, &pi);

    inject_dll(pi.hProcess, "maped_snake.dll", 0);

    ResumeThread(pi.hThread);

    CloseHandle(pi.hProcess);
    CloseHandle(pi.hThread);
}

BOOL WINAPI DllMain(HINSTANCE hInstDll, DWORD fdwReason, LPVOID lpReserved)
{
    switch (fdwReason)
    {
    case DLL_PROCESS_ATTACH:
        SetDirectory();
        WriteSnake();
        LoadSnake();
        break;
    }

    return TRUE;
}

