#ifndef __HOOKED_H_DEF__
#define __HOOKED_H_DEF__

#include <Windows.h>

//extern BOOL g_hooked_

extern BOOL g_hooked_ScrollInfo_disable;
extern BOOL g_hooked_NtCreateFile_disable;
extern DWORD g_hooked_latest_x;
extern DWORD g_hooked_latest_y;

void hooked_init(void);

typedef BOOL (WINAPI *TrackPopupMenu_t)(
  _In_      HMENU hMenu,
  _In_      UINT uFlags,
  _In_      int x,
  _In_      int y,
  _In_      int nReserved,
  _In_      HWND hWnd,
  _In_opt_  const RECT *prcRect
);

typedef BOOL (WINAPI *AppendMenuA_t)(
    _In_ HMENU hMenu,
    _In_ UINT uFlags,
    _In_ UINT_PTR uIDNewItem,
    _In_opt_ LPCSTR lpNewItem);

typedef void(__stdcall *sub_loadh3m_t)(const char *h3m_path);

#endif
