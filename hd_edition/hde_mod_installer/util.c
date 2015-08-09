// Created by John Åkerblom 2015-07-11
#include <windows.h>
#include <shlobj.h>
#include <shellapi.h>
#include <tlHelp32.h>

#include "util.h"

BOOL UtilPathFromRunningExeW(IN LPCWSTR lpExeName, OUT LPWSTR lpExePath,
    IN int iMaxLength)
{
    HANDLE hProcessSnap;
    HANDLE hModuleSnap;
    PROCESSENTRY32W pe32;
    MODULEENTRY32W me32;
    int iCutoff = 0;

    hProcessSnap = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
    if (INVALID_HANDLE_VALUE == hProcessSnap)
    {
        return FALSE;
    }

    pe32.dwSize = sizeof(PROCESSENTRY32W);

    if (FALSE == Process32FirstW(hProcessSnap, &pe32))
    {
        CloseHandle(hProcessSnap);
        return FALSE;
    }

    me32.dwSize = sizeof(MODULEENTRY32W);

    do
    {
        // Loop if exe name does not match the one searched for
        if (0 != lstrcmpiW(lpExeName, pe32.szExeFile))
        {
            continue;
        }

        // If exe name matched, try to obtain path, breaking out if successful
        hModuleSnap = CreateToolhelp32Snapshot(TH32CS_SNAPMODULE,
            pe32.th32ProcessID);
        if (FALSE != Module32FirstW(hModuleSnap, &me32))
        {
            lstrcpynW(lpExePath, me32.szExePath, iMaxLength);
            // Cut off exe name, leaving just directory ending with slash
            iCutoff = (lstrlenW(lpExePath) - lstrlenW(lpExeName));
            if (iCutoff > 0 && iCutoff < iMaxLength)
                lpExePath[iCutoff] = 0;
            CloseHandle(hModuleSnap);
            break;
        }

        CloseHandle(hModuleSnap);
    } while (Process32NextW(hProcessSnap, &pe32));

    CloseHandle(hProcessSnap);
    return TRUE;
}

BOOL UtilPathFromBrowseDialogW(IN LPCWSTR lpTitle, OUT LPWSTR lpPath, 
    int iMaxLength)
{
    BROWSEINFOW bInfo;
    WCHAR szwName[MAX_PATH];
    WCHAR szwPath[MAX_PATH];
    LPITEMIDLIST lpItem = NULL;

    bInfo.hwndOwner = NULL;
    bInfo.pidlRoot = NULL;
    bInfo.pszDisplayName = szwName;
    bInfo.lpszTitle = lpTitle;
    bInfo.ulFlags = BIF_NEWDIALOGSTYLE | BIF_RETURNFSANCESTORS 
        | BIF_RETURNONLYFSDIRS | BIF_EDITBOX;
    bInfo.lpfn = NULL;
    bInfo.lParam = 0;
    bInfo.iImage = -1;

    if (NULL == (lpItem = SHBrowseForFolderW(&bInfo)))
    {
        return FALSE;
    }

    SHGetPathFromIDListW(lpItem, szwPath);
    CoTaskMemFree(lpItem);

    lstrcpynW(lpPath, szwPath, iMaxLength);

    return TRUE;
}

VOID UtilSafeStrCat(OUT LPWSTR lpStr, IN LPCWSTR lpCat, IN int iMaxLength)
{
    int iNewLength = wcslen(lpStr) + wcslen(lpCat) + 1;
    if (iNewLength <= iMaxLength)
        lstrcatW(lpStr, lpCat);
}

// TODO simplify string handling in this function to make it easier to read
BOOL UtilCopyDirectoryW(IN LPCWSTR lpSource, IN LPCWSTR lpDestination)
{
    HANDLE hFind = NULL;
    WIN32_FIND_DATAW wfd;
    WCHAR szwSearch[MAX_PATH];
    WCHAR szwSubSearch[MAX_PATH];
    WCHAR szwSource[MAX_PATH];
    WCHAR szwTarget[MAX_PATH];

    UtilStackSafeStrCpy(szwTarget, lpDestination);
    UtilStackSafeStrCat(szwTarget, L"\\");
    UtilStackSafeStrCat(szwTarget, lpSource);
    CreateDirectoryW(szwTarget, NULL);

    UtilStackSafeStrCpy(szwSearch, lpSource);
    UtilStackSafeStrCat(szwSearch, L"\\*");

    if (INVALID_HANDLE_VALUE == (hFind = FindFirstFileW(szwSearch, &wfd)))
    {
        return FALSE;
    }

    do
    {
        if (0 == lstrcmpW(wfd.cFileName, L".")
            || 0 == lstrcmpW(wfd.cFileName, L".."))
        {
            continue;
        }

        if (FILE_ATTRIBUTE_DIRECTORY & wfd.dwFileAttributes)
        {
            UtilStackSafeStrCpy(szwSubSearch, szwSearch);
            szwSubSearch[lstrlenW(szwSubSearch) - 1] = 0;

            UtilStackSafeStrCpy(szwTarget, lpDestination);
            UtilStackSafeStrCat(szwTarget, L"\\");
            UtilStackSafeStrCat(szwTarget, szwSubSearch);
            UtilStackSafeStrCat(szwTarget, wfd.cFileName);
            CreateDirectoryW(szwTarget, NULL);

            UtilStackSafeStrCat(szwSubSearch, wfd.cFileName);
            UtilStackSafeStrCat(szwSubSearch, L"\\");
            UtilCopyDirectoryW(szwSubSearch, lpDestination);

            continue;
        }

        UtilStackSafeStrCpy(szwSource, lpSource);
        UtilStackSafeStrCat(szwSource, wfd.cFileName);

        UtilStackSafeStrCpy(szwTarget, lpDestination);
        UtilStackSafeStrCat(szwTarget, L"\\");
        UtilStackSafeStrCat(szwTarget, szwSource);

        if (FALSE == CopyFileW(szwSource, szwTarget, FALSE))
        {
            FindClose(hFind);
            return FALSE;
        }
    } while (FALSE != FindNextFileW(hFind, &wfd));

    FindClose(hFind);

    return TRUE;
}

BOOL UtilCopyFilesW(IN LPCWSTR *lpExistingFiles, IN DWORD nCount,
    IN LPCWSTR lpDestination)
{
    DWORD i = 0;
    DWORD dwAttr = 0;
    WCHAR szwTarget[MAX_PATH];
    
    for (i = 0; i < nCount; ++i)
    {
        dwAttr = GetFileAttributesW(lpExistingFiles[i]);
        if (INVALID_FILE_ATTRIBUTES == dwAttr)
            continue;

        if (FILE_ATTRIBUTE_DIRECTORY & dwAttr)
            UtilCopyDirectoryW(lpExistingFiles[i], lpDestination);
        else
        {
            UtilStackSafeStrCpy(szwTarget, lpDestination);
            UtilStackSafeStrCat(szwTarget, L"\\");
            UtilStackSafeStrCat(szwTarget, lpExistingFiles[i]);
            if (FALSE == CopyFileW(lpExistingFiles[i], szwTarget, FALSE))
            {
                return FALSE;
            }
        }
    }

    return TRUE;
}

