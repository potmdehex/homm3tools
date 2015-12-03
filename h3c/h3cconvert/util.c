// Created by John Åkerblom 2015-07-19

#include "util.h"

#include <windows.h>

static UINT CALLBACK _OFNHookProc(HWND hDlg, UINT uiMsg, WPARAM wParam, LPARAM lParam)
{
    DWORD dwNeededSize = 0;
    LPOPENFILENAMEW lpOfn = NULL;

    switch (uiMsg)
    {
    case WM_INITDIALOG:
        SetPropW(GetParent(hDlg), L"OFN", (void *)lParam);
        break;
    case WM_NOTIFY:
        if (CDN_SELCHANGE == ((OFNOTIFYW *)lParam)->hdr.code)
        {
            dwNeededSize = CommDlg_OpenSave_GetSpecW(GetParent(hDlg), NULL, 0) * sizeof(WCHAR);
            dwNeededSize += MAX_PATH * sizeof(WCHAR);

            lpOfn = (LPOPENFILENAMEW)GetPropW(GetParent(hDlg), L"OFN");
            if (lpOfn->nMaxFile < dwNeededSize)
            {
                lpOfn->lpstrFile = (LPWSTR)HeapReAlloc(GetProcessHeap(), 0, lpOfn->lpstrFile, dwNeededSize);
                if (NULL == lpOfn->lpstrFile)
                    return 1;
                lpOfn->lpstrFile[0] = L'\0';
                lpOfn->nMaxFile = dwNeededSize;
            }
        }
        break;
    case WM_DESTROY:
        RemovePropW(GetParent(hDlg), L"OFN");
        break;
    }

    return 0;
}

// Free returned string with HeapFree(GetProcessHeap(), ...);
LPWSTR UtilBrowseForMultipleFiles(IN LPCWSTR lpTitle, IN LPCWSTR lpFilter,
    OUT DWORD *pnFiles)
{
    OPENFILENAMEW ofn = { 0 };
    LPCWSTR lpFilename = NULL;

    ofn.lStructSize = sizeof (ofn);
    ofn.lpstrFile = HeapAlloc(GetProcessHeap(), HEAP_ZERO_MEMORY, MAX_PATH * sizeof(WCHAR));
    ofn.lpstrTitle = lpTitle;
    ofn.lpstrFilter = lpFilter;
    ofn.Flags = OFN_FILEMUSTEXIST | OFN_ALLOWMULTISELECT | OFN_EXPLORER | OFN_ENABLEHOOK;
    ofn.lpfnHook = _OFNHookProc;
    ofn.nMaxFile = 1; // GetOpenFileNameW fails if this is 0. Actual value is set in hook.
    
    if (FALSE == GetOpenFileNameW(&ofn))
    {
        HeapFree(GetProcessHeap(), 0, ofn.lpstrFile);
        ofn.lpstrFile = NULL;
        return FALSE;
    }

    // Adjust output string format for single selection case
    // 
    // GetOpenFileName produces different format when one file is selected as opposed
    // to when several are. Here format for single selection is changed to be like 
    // multiselection. This means appending an extra NULL terminator and cutting off
    // filename from path
    if (NULL != ofn.lpstrFile && ofn.nFileOffset > 0
        && L'\0' != ofn.lpstrFile[ofn.nFileOffset - 1]) // if not multiselection
    {
        // We might be unlucky and not have room for the extra 2 bytes, if so realloc
        if (ofn.nMaxFile < wcslen(ofn.lpstrFile) + 1 + 1)
        {
            ofn.lpstrFile = (LPWSTR)HeapReAlloc(GetProcessHeap(), 0, ofn.lpstrFile, ofn.nMaxFile * 2);
        }
        // Add extra NULL terminator at end, and replace the last backslash in path
        // with a NULL terminator
        ofn.lpstrFile[wcslen(ofn.lpstrFile) + 1] = L'\0';
        ofn.lpstrFile[ofn.nFileOffset - 1] = L'\0';
    }

    // Count number of files
    if (NULL != pnFiles)
    {
        lpFilename = &ofn.lpstrFile[ofn.nFileOffset];
        for (; L'\0' != lpFilename[0]; lpFilename += wcslen(lpFilename) + 1)
        {
            ++(*pnFiles);
        }
    }

    return ofn.lpstrFile;
}

