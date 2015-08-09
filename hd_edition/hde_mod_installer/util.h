// Created by John Åkerblom 2015-07-11
#ifndef __UTIL_H_DEF__
#define __UTIL_H_DEF__

#include <windows.h>

BOOL UtilPathFromRunningExeW(IN LPCWSTR lpExeName, OUT LPWSTR lpExePath,
    IN int iMaxLength);

BOOL UtilPathFromBrowseDialogW(IN LPCWSTR lpTitle, OUT LPWSTR lpPath,
    int iMaxLength);

BOOL UtilCopyDirectoryW(IN LPCWSTR lpSource, IN LPCWSTR lpDestination);

BOOL UtilCopyFilesW(IN LPCWSTR *lpExistingFiles, IN DWORD nCount,
    IN LPCWSTR lpDestination);

VOID UtilSafeStrCat(OUT LPWSTR lpStr, IN LPCWSTR lpCat, IN int iMaxLength);

#define UtilStackSafeStrCat(s, c) UtilSafeStrCat(s, c, sizeof(s))
#define UtilStackSafeStrCpy(s, c) lstrcpynW(s, c, sizeof(s))

#endif

