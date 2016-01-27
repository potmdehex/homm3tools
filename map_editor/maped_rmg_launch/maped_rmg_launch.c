// Created by John Åkerblom 2013 (generic CreateProcess + DLL inject)

#include <windows.h>

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

#define DLL_NAME "maped_rmg.dll"
#define COMMANDLINE "h3maped.exe"

// Note: this function has a CRT dep because of strlen, but can
// still be compiled with VS without CRT due to optimization
BOOL inject_dll(HANDLE proc, const char *dll_path, DWORD timeout)
{
    void *remote_path = NULL;
    HANDLE remote_lib = NULL;
    HANDLE thread = NULL;
    HMODULE kernel32 = GetModuleHandleA("Kernel32");

    if (NULL == (remote_path = VirtualAllocEx(proc, NULL, strlen(dll_path) + 1,
        MEM_COMMIT, PAGE_READWRITE))) {
        return FALSE;
    }

    WriteProcessMemory(proc, remote_path, (void*)dll_path, strlen(dll_path),
        NULL);

    if (NULL == (thread = CreateRemoteThread(proc, NULL, 0,
        (LPTHREAD_START_ROUTINE)GetProcAddress(kernel32, "LoadLibraryA"),
        remote_path, 0, NULL))) {
        return FALSE;
    }

    if (0 != timeout) {
        if (WAIT_TIMEOUT == WaitForSingleObject(thread, timeout)) {
            CloseHandle(thread);
            return FALSE;
        }
    }

    // Get handle, success if not NULL
    GetExitCodeThread(thread, (LPDWORD)&remote_lib);
    CloseHandle(thread);

    return remote_lib ? TRUE : FALSE;
}

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nShowCmd)
{
    STARTUPINFOA si;
    PROCESS_INFORMATION pi;

    memset(&si, 0, sizeof(si));
    memset(&pi, 0, sizeof(pi));
    si.cb = sizeof(si);
    si.lpReserved = "C:\\test2";

    if (FALSE == CreateProcessA(NULL, COMMANDLINE, NULL, NULL, FALSE,
        CREATE_SUSPENDED, NULL, NULL, &si, &pi)) {
        DWORD e = GetLastError();
        MessageBoxA(NULL, "CreateProcessA failed", COMMANDLINE, MB_ICONERROR);
        return 1;
    }

    if (FALSE == inject_dll(pi.hProcess, DLL_NAME, 0)) {
        MessageBoxA(NULL, "Failed to inject " DLL_NAME, "DLL Injection Failed", MB_ICONERROR);
        return 1;
    }

    ResumeThread(pi.hThread);

    CloseHandle(pi.hProcess);
    CloseHandle(pi.hThread);

    return 0;
}
