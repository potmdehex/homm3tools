#include <windows.h>
#include <stdio.h>

#define NO_UDIS86
#include <hook_utils.c>
#include <inject_utils.c>

typedef BOOL (WINAPI *TypeCreateProcessA)(
    _In_opt_    LPCSTR                lpApplicationName,
    _Inout_opt_ LPSTR                 lpCommandLine,
    _In_opt_    LPSECURITY_ATTRIBUTES lpProcessAttributes,
    _In_opt_    LPSECURITY_ATTRIBUTES lpThreadAttributes,
    _In_        BOOL                  bInheritHandles,
    _In_        DWORD                 dwCreationFlags,
    _In_opt_    LPVOID                lpEnvironment,
    _In_opt_    LPCSTR                lpCurrentDirectory,
    _In_        LPSTARTUPINFOA        lpStartupInfo,
    _Out_       LPPROCESS_INFORMATION lpProcessInformation
    );

TypeCreateProcessA OrigCreateProcessA = (TypeCreateProcessA)NULL;

BOOL WINAPI HookedCreateProcessA(
    _In_opt_    LPCSTR                lpApplicationName,
    _Inout_opt_ LPSTR                 lpCommandLine,
    _In_opt_    LPSECURITY_ATTRIBUTES lpProcessAttributes,
    _In_opt_    LPSECURITY_ATTRIBUTES lpThreadAttributes,
    _In_        BOOL                  bInheritHandles,
    _In_        DWORD                 dwCreationFlags,
    _In_opt_    LPVOID                lpEnvironment,
    _In_opt_    LPCSTR                lpCurrentDirectory,
    _In_        LPSTARTUPINFOA        lpStartupInfo,
    _Out_       LPPROCESS_INFORMATION lpProcessInformation
    )
{
    BOOL bRet;
    BOOL bSuspended = (dwCreationFlags & CREATE_SUSPENDED);

    WCHAR szwApplicationName[MAX_PATH] = { 0 };
    WCHAR szwCommandLine[MAX_PATH] = { 0 };
    WCHAR szwCurrentDirectory[MAX_PATH] = { 0 };
    STARTUPINFOW siw = { 0 };

    _snwprintf(szwApplicationName,
        sizeof(szwApplicationName) / sizeof(WCHAR),
        L"%S", (NULL != lpApplicationName) ? lpApplicationName : "");
    _snwprintf(szwCommandLine,
        sizeof(szwCommandLine) / sizeof(WCHAR),
        L"%S --delayhook", (NULL != lpCommandLine) ? lpCommandLine : "");
    _snwprintf(szwCurrentDirectory,
        sizeof(szwCurrentDirectory) / sizeof(WCHAR),
        L"%S", (NULL != lpCurrentDirectory) ? lpCurrentDirectory : "");

    siw.cb = sizeof(siw);
    
    dwCreationFlags |= CREATE_SUSPENDED;
    
    bRet = CreateProcessW(
        (NULL != lpApplicationName)? szwApplicationName : NULL,
        (NULL != lpCommandLine)? szwCommandLine : NULL,
        lpProcessAttributes,
        lpThreadAttributes,
        bInheritHandles,
        dwCreationFlags, 
        lpEnvironment,
        (NULL != lpCurrentDirectory)? szwCurrentDirectory : NULL,
        &siw,
        lpProcessInformation);
    
    if (NULL != wcsstr(szwCommandLine, L"HOMM3 2.0"))
    {
        inject_dll(lpProcessInformation->hProcess, "hde_mod.dll", 1000);
    }

    if (!bSuspended)
    {
        ResumeThread(lpProcessInformation->hThread);
    }

    return bRet;
}

void HookCreateProcess()
{
    hook_hotpatch_x86(GetProcAddress(GetModuleHandleA("kernel32"), "CreateProcessA"),
        HookedCreateProcessA,
        (void **)&OrigCreateProcessA);
}
