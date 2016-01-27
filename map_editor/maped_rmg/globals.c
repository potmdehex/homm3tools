// Created 2014-11-24 by John Åkerblom

#include "globals.h"
#include <Windows.h>

char *g_map_filename = NULL;
int g_do_replace = 0;
int g_selectable_towns = 0;

HWND g_hwnd_main = NULL;
HWND g_hwnd_rmg_dialog = NULL;

int g_selected_towns[256];