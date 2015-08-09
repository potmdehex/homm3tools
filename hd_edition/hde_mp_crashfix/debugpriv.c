// Created by John Åkerblom 2015

#include <windows.h>

BOOL SetPrivilege(HANDLE hToken, LPCTSTR lpszPrivilege, BOOL bEnablePrivilege)
{
    TOKEN_PRIVILEGES pTokenPrivileges;
    LUID luid;

    if (0 == LookupPrivilegeValue(NULL, lpszPrivilege, &luid))
    {
        return FALSE;
    }

    pTokenPrivileges.PrivilegeCount = 1;
    pTokenPrivileges.Privileges[0].Luid = luid;

    if (FALSE != bEnablePrivilege)
    {
        pTokenPrivileges.Privileges[0].Attributes = SE_PRIVILEGE_ENABLED;
    }
    else
    {
        pTokenPrivileges.Privileges[0].Attributes = 0;
    }

    if (0 == AdjustTokenPrivileges(hToken, FALSE, &pTokenPrivileges, sizeof(TOKEN_PRIVILEGES), NULL, NULL))
    {
        return FALSE;
    }

    if (ERROR_NOT_ALL_ASSIGNED == GetLastError())
    {
        return FALSE;
    }

    return TRUE;
}

BOOL SetDebugPrivilege(BOOL bEnablePrivilege)
{
    HANDLE	hToken = NULL;
    BOOL	bRet = FALSE;

    if (FALSE == OpenProcessToken(GetCurrentProcess(), TOKEN_ALL_ACCESS, &hToken))
    {
        return FALSE;
    }

    bRet = SetPrivilege(hToken, SE_DEBUG_NAME, bEnablePrivilege);

    CloseHandle(hToken);

    return bRet;
}

