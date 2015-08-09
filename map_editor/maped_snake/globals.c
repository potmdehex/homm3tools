// Created 2014-11-24 by John Åkerblom

#include "globals.h"
#include <Windows.h>

HMENU g_ext_menu;

char *g_map_filename;
int g_do_replace;

DWORD g_main_tid = 0;

BOOL g_disable_draw = FALSE;

#pragma warning(disable:4996)
