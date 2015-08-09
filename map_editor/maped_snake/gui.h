// Created by John Åkerblom 2014-11-22

#ifndef __GUI_H_DEF__
#define __GUI_H_DEF__

#include <windows.h>

extern HWND g_hwnd_main;

extern HWND g_hwnd_render;

#define ID_H3MSNAKE   GUI_ID_FIRST 
#define WM_RESTART WM_USER + 1337

#define GUI_ID_FIRST 40000
#define GUI_ID_LAST  50000

void gui_init(void);

#endif
