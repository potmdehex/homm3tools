// Created by John Åkerblom 2012-07-??

#include "inject_utils.h"

BOOL inject_dll(HANDLE proc, const char *dll_path, DWORD timeout)
{
    void *remote_path = NULL;
    HANDLE remote_lib = NULL;
    HANDLE thread = NULL;
    HMODULE kernel32 = GetModuleHandleA("Kernel32");

    if (NULL == (remote_path = VirtualAllocEx(proc, NULL, strlen(dll_path) + 1,
        MEM_COMMIT, PAGE_READWRITE)))
    {
        return FALSE;
    }

    WriteProcessMemory(proc, remote_path, (void*)dll_path, strlen(dll_path), 
        NULL);

    if (NULL == (thread = CreateRemoteThread(proc, NULL, 0,
        (LPTHREAD_START_ROUTINE)GetProcAddress(kernel32, "LoadLibraryA"),
        remote_path, 0, NULL)))
    {
        return FALSE;
    }

    if (0 != timeout)
    {
        if (WAIT_TIMEOUT == WaitForSingleObject(thread, timeout))
        {
            CloseHandle(thread);
            return FALSE;
        }
    }

    // Get handle, success if not NULL
    GetExitCodeThread(thread, (LPDWORD)&remote_lib);
    CloseHandle(thread);

    return remote_lib ? TRUE : FALSE;
}
