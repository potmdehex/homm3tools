// Created 2014-11-24 by John Åkerblom

#ifndef __GLOBALS_H_DEF__
#define __GLOBALS_H_DEF__

#include <Windows.h>
#include <stdint.h>

extern char *g_map_filename;
extern WCHAR *g_map_filename_w;
extern int g_do_replace;
extern int g_do_fail;
extern HWND g_hwnd_main;
extern HWND g_hwnd_rmg_dialog;

extern int g_selectable_towns;

extern uint32_t g_map_format;
extern uint32_t g_new_format;
extern BOOL g_convert_on_reload;
extern HWND g_hwnd_terrain_types;
#endif
