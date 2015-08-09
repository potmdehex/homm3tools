// Created by John Åkerblom 2012-07-??

#ifndef __INJECT_UTILS_H_DEF__
#define __INJECT_UTILS_H_DEF__

#include <windows.h>

BOOL inject_dll(HANDLE proc, const char *dll_path, DWORD timeout);

#endif