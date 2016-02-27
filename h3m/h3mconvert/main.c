// Created by John Åkerblom 2015-07-19

#include "convert.h"
#include "progress_dialog.h"
#include "messages.h"
#include "resource.h"
#include "util.h"
#include "version.h"

#include <h3mlib.h>

#include <commctrl.h>
#include <shlwapi.h>
#include <stdio.h>
#include <windows.h>

#ifdef _MSC_VER
    #pragma warning(disable:4996) // M$ standard lib unsafe
    #define snwprintf _snwprintf
#endif

#define MSG_BROWSE L"Select map(s) to convert to RoE format"
#define MSG_CONVERTED L"Converted %d/%d maps..."
#define MSG_COMPLETE L"Converted maps:\n\nRoE:\t\t%d\nAB:\t\t%d\nSoD:\t\t%d\nWoG:\t\t%d\nHotA:\t\t%d\n" \
    L"Total:\t\t%d\n-------------\nFailed:\t\t%d\n" \
    L"-------------\nOpen destination folder?"
#define TITLE_INITIAL L"Starting conversion..."
#define TITLE_CONVERTING L"Converting: %d%%"
#define TITLE_COMPLETE L"h3mconvert " VERSION_W

struct CONVERT_CB_DATA
{
    DWORD dwMainThreadId;
    LPCWSTR lpSrc;
    LPCWSTR lpDest;
    unsigned int converted_ab;
    unsigned int converted_sod;
    unsigned int converted_wog;
    unsigned int converted_hota;
    unsigned int failed;
    unsigned int skipped;
};

int _convert_cb(const wchar_t *filename, uint32_t version, uint32_t result, void *cb_data)
{
    struct CONVERT_CB_DATA *conv = (struct CONVERT_CB_DATA *)cb_data;
    const wchar_t *version_str = L"Unknown";

    switch (version)
    {
    case H3M_FORMAT_AB:
        version_str = L"AB";
        break;
    case H3M_FORMAT_SOD:
        version_str = L"SoD";
        break;
    case H3M_FORMAT_WOG:
        version_str = L"WoG";
        break;
    case H3M_FORMAT_HOTA:
        version_str = L"HotA";
        break;
    default:
        break;
    }
    
    if (CONVERT_RESULT_SKIP == result)
    {
        ++conv->skipped;
        PostThreadMessage(conv->dwMainThreadId, WM_SKIPPED_MAP, (WPARAM)_wcsdup(filename), (LPARAM)version_str);
    }
    else if (CONVERT_RESULT_FAIL == result)
    {
        ++conv->failed;
        PostThreadMessage(conv->dwMainThreadId, WM_FAILED_MAP, (WPARAM)_wcsdup(filename), (LPARAM)version_str);
    }
    else
    {
        switch (version)
        {
        case H3M_FORMAT_AB:
            ++conv->converted_ab;
            break;
        case H3M_FORMAT_SOD:
            ++conv->converted_sod;
            break;
        case H3M_FORMAT_WOG:
            ++conv->converted_wog;
            break;
        case H3M_FORMAT_HOTA:
            ++conv->converted_hota;
            break;
        default:
            break;
        }

        PostThreadMessage(conv->dwMainThreadId, WM_CONVERTED_MAP, (WPARAM)_wcsdup(filename), (LPARAM)version_str);
    }
    return 0;
}

static DWORD WINAPI _ConvertThreadProc(LPVOID lpParameter)
{
    struct CONVERT_CB_DATA *conv = (struct CONVERT_CB_DATA *)lpParameter;
    LPCWSTR lpSrcPath = conv->lpSrc; // First string in lpSrc is the path
    LPCWSTR lpFilename = conv->lpSrc;
    
    lpFilename += wcslen(lpFilename) + 1;
    for (; L'\0' != lpFilename[0]; lpFilename += wcslen(lpFilename) + 1)
    {
        convert_to_roe(lpSrcPath, lpFilename, conv->lpDest, _convert_cb, (void *)conv);
    }

    PostThreadMessage(conv->dwMainThreadId, WM_CONVERTED_ALL, 0, 0);

    ExitThread(0);
}

// TODO: More details. Perhaps a list view with information about each map.
static VOID _ShowDetails(struct CONVERT_CB_DATA *conv, LPCWSTR lpDest)
{
    WCHAR szwCompleteMsg[MAX_PATH * 4] = { 0 };
    /*WCHAR szwCompleteTitle[MAX_PATH * 4] = { 0 };

    _snwprintf(szwCompleteTitle, (sizeof(szwCompleteTitle) / sizeof(szwCompleteTitle[0])) - 1,
        TITLE_COMPLETE,
        conv->converted_ab + conv->converted_sod + conv->converted_wog);*/
    _snwprintf(szwCompleteMsg, (sizeof(szwCompleteMsg) / sizeof(szwCompleteMsg[0])) - 1,
        MSG_COMPLETE,
        conv->skipped,
        conv->converted_ab,
        conv->converted_sod,
        conv->converted_wog,
        conv->converted_hota,
        conv->skipped + conv->converted_ab + conv->converted_sod + conv->converted_wog,
        conv->failed);

    if (IDYES == MessageBoxW(NULL, szwCompleteMsg, TITLE_COMPLETE, MB_ICONINFORMATION | MB_YESNO))
    {
        ShellExecuteW(NULL, L"open", lpDest, NULL, NULL, SW_SHOWDEFAULT);
    }
}

VOID UpdateGui(HWND hwnd, DWORD nConverted, DWORD nTotal, LPCWSTR lpLastConverted)
{
    WCHAR szwTitle[256];
    WCHAR szwProgress[256];
    DWORD dwPercent = 0;

    dwPercent = (unsigned int)(((float)nConverted / (float)nTotal) * (float)100);

    snwprintf(szwTitle, sizeof(szwTitle) / sizeof(WCHAR)-1, TITLE_CONVERTING, dwPercent);
    snwprintf(szwProgress, sizeof(szwTitle) / sizeof(WCHAR)-1, MSG_CONVERTED, nConverted, nTotal);

    dialog_update_progress(hwnd, dwPercent, szwTitle, szwProgress, lpLastConverted);
}

VOID CopySkippedMap(LPCWSTR lpDestPath, LPCWSTR lpSourcePath, LPCWSTR lpFilename)
{
    WCHAR szwExistingFileName[MAX_PATH];
    WCHAR szwNewFileName[MAX_PATH];

    _snwprintf(szwExistingFileName,
        sizeof(szwExistingFileName) / sizeof (WCHAR) - 1, L"%s\\%s",
        lpSourcePath, lpFilename);

    _snwprintf(szwNewFileName,
        sizeof(szwNewFileName) / sizeof (WCHAR) - 1, L"%s\\%s",
        lpDestPath, lpFilename);

    CopyFileW(szwExistingFileName, szwNewFileName, FALSE);
}

int WINAPI wWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPWSTR lpCmdLine, int nShowCmd)
{
    INITCOMMONCONTROLSEX icc = { 0 };
    struct CONVERT_CB_DATA conv = { 0 };
    LPWSTR lpBrowseResult = NULL;
    WCHAR szwExePath[MAX_PATH] = { 0 };
    WCHAR szwDest[MAX_PATH] = { 0 };
    DWORD nTotal = 0;
    DWORD nConverted = 0;
    HWND hwndProgressDlg = NULL;
    HICON hIcon = NULL;
    BOOL bEnd = FALSE;
    MSG msg = { 0 };

    icc.dwSize = sizeof(INITCOMMONCONTROLSEX);
    icc.dwICC  = ICC_LINK_CLASS;
    InitCommonControlsEx(&icc);

    // Select .h3m files to convert
    lpBrowseResult = UtilBrowseForMultipleFiles(MSG_BROWSE, L"H3M (*.h3m)\0*.h3m\0", &nTotal);
    if (NULL == lpBrowseResult)
    {
        return 1;
    }

    // Obtain own path, establish conversion destination subfolder \\converted
    GetModuleFileNameW(GetModuleHandle(NULL), szwExePath, MAX_PATH);
    PathRemoveFileSpecW(szwExePath);
    _snwprintf(szwDest, sizeof(szwDest) / sizeof(szwDest[0]) - 1, L"%s\\converted", szwExePath);
    CreateDirectoryW(szwDest, NULL);

    // Create conversion progress dialog
    hIcon = LoadIcon(GetModuleHandle(NULL), MAKEINTRESOURCE(IDI_ICON1));
    hwndProgressDlg = progress_dialog_init(TITLE_INITIAL, hIcon);
    DestroyIcon(hIcon);

    // Start conversion thread
    conv.dwMainThreadId = GetCurrentThreadId();
    conv.lpSrc = lpBrowseResult;
    conv.lpDest = szwDest;
    CloseHandle(CreateThread(NULL, 0, _ConvertThreadProc, (LPVOID)&conv, 0, NULL));

    while (GetMessage(&msg, NULL, 0, 0) > 0)
    {
        switch (msg.message)
        {
        case WM_SKIPPED_MAP:
            // First string in lpBrowseResult is source path
            CopySkippedMap(szwDest, lpBrowseResult, (LPCWSTR)msg.wParam);
            // Fall-through
        case WM_FAILED_MAP:
        case WM_CONVERTED_MAP:
            ++nConverted;
            UpdateGui(hwndProgressDlg, nConverted, nTotal, (LPCWSTR)msg.wParam);
            if (0 != msg.wParam)
            {
                free((LPWSTR *)msg.wParam);
            }
            break;
        case WM_CONVERTED_ALL:
            bEnd = TRUE;
            break;
        }

        if (TRUE == bEnd)
            break;

        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }

    // Close progress dialog and show conversion results
    DestroyWindow(hwndProgressDlg);
    _ShowDetails(&conv, szwDest);

    // Cleanup
    HeapFree(GetProcessHeap(), 0, lpBrowseResult);

    return 0;
}

