/* Created by John Åkerblom 10/26/2014 */

#include "progress_dialog.h"
#include "resource.h"

#include <stdio.h>
#include <windows.h>
#include <commctrl.h>

#ifdef _MSC_VER
    #pragma warning(disable:4996) // M$ standard lib unsafe
#endif

#define WM_USER_PROGRESS WM_USER + 1337

static BOOL _CenterWindow(HWND hwnd, HWND hwnd_parent)
{
    RECT rect, rectP;
    int width, height;
    int x, y;

    GetWindowRect(hwnd, &rect);
    GetWindowRect(hwnd_parent, &rectP);

    width = rect.right - rect.left;
    height = rect.bottom - rect.top;

    x = ((rectP.right - rectP.left) - width) / 2 + rectP.left;
    y = ((rectP.bottom - rectP.top) - height) / 2 + rectP.top;

    MoveWindow(hwnd, x, y, width, height, FALSE);

    return TRUE;
}

BOOL CALLBACK ProgressDlgProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
    switch (msg)
    {
    case WM_COMMAND:
        switch (LOWORD(wParam))
        {
        case IDCANCEL:
        case IDCLOSE:
            PostQuitMessage(0);
            break;
        }
        break;
    case WM_CLOSE:
        PostQuitMessage(0);
        break;
    default:
        return FALSE;
    }

    return TRUE;
}

HWND progress_dialog_init(LPCWSTR lpTitle, HICON hIcon)
{
    HWND hwnd = CreateDialogParamW(NULL, MAKEINTRESOURCEW(IDD_PROGRESSDIALOG), 
        NULL, ProgressDlgProc, (LPARAM)NULL);
    SendMessage(hwnd, WM_SETICON, ICON_BIG, (LPARAM)hIcon);
    SetWindowTextW(hwnd, lpTitle);
    _CenterWindow(hwnd, GetDesktopWindow());
    ShowWindow(hwnd, SW_SHOW);

    return hwnd;
}

void dialog_update_progress(HWND hwnd, DWORD dwPercent, LPCWSTR lpTitle,
    LPCWSTR lpProgressStatic, LPCWSTR lpExtraStatic)
{
    HWND hwndPB = GetDlgItem(hwnd, IDC_PROGRESSBAR);

    SendMessage(hwndPB, PBM_SETPOS, dwPercent, 0);

    SetWindowTextW(hwnd, lpTitle);
    SetWindowTextW(GetDlgItem(hwnd, IDC_PROGRESSSTATIC), lpProgressStatic);
    SetWindowTextW(GetDlgItem(hwnd, IDC_EXTRASTATIC), lpExtraStatic);
}
