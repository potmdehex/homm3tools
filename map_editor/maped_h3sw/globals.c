// Created 2014-11-24 by John Åkerblom

#include "globals.h"
#include <Windows.h>

char *g_map_filename = NULL;
WCHAR *g_map_filename_w = NULL;
int g_do_replace = 0;
int g_do_fail = 0;
int g_selectable_towns = 0;

HWND g_hwnd_main = NULL;
HWND g_hwnd_rmg_dialog = NULL;

uint32_t g_map_format = 0;
uint32_t g_new_format;
BOOL g_convert_on_reload;
