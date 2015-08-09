/* Created by John Åkerblom 10/26/2014 */

#ifndef __PROGRESS_DIALOG_H_DEF__
#define __PROGRESS_DIALOG_H_DEF__

#include <windows.h>

HWND progress_dialog_init(LPCWSTR lpTitle, HICON hIcon);

void dialog_update_progress(HWND hwnd, DWORD dwPercent, LPCWSTR lpTitle, 
    LPCWSTR lpProgressStatic, LPCWSTR lpExtraStatic);

#endif

