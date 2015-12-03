// Created by John Åkerblom 2015-07-19

#ifndef __UTIL_H_DEF__
#define __UTIL_H_DEF__

#include <windows.h>

// Free returned string with HeapFree(GetProcessHeap(), ...);
LPWSTR UtilBrowseForMultipleFiles(IN LPCWSTR lpTitle, IN LPCWSTR lpFilter, 
    OUT DWORD *pnFiles);

#endif

