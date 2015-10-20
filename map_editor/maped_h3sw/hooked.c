#include "hooked.h"
#include "globals.h"
#include "gui.h"
#include "messages.h"
#include "nt_undoc.h"
#include "rmgdialog_fix.h"
#include "rmg_postfix.h"

#include "traverse_funcs.h"

#include <h3mlib.h>

#include <hook_utils.h>

#include <windows.h>
#include <CommCtrl.h>
#include <string.h>
#include <stdio.h>

#pragma warning(disable:4996)

typedef int (WINAPI *lstrcmpiA_t)(
    _In_  LPCSTR lpString1,
    _In_  LPCSTR lpString2
    );

typedef BOOL (WINAPI *GetSaveFileNameA_t)(
    _Inout_  LPOPENFILENAMEA lpofn
    );

typedef int (WINAPI *MessageBoxA_t)(
    _In_opt_  HWND hWnd,
    _In_opt_  LPCSTR lpText,
    _In_opt_  LPCSTR lpCaption,
    _In_      UINT uType
    );

typedef HWND (WINAPI *CreateDialogIndirectParamA_t)(
    _In_opt_  HINSTANCE hInstance,
    _In_      LPCDLGTEMPLATE lpTemplate,
    _In_opt_  HWND hWndParent,
    _In_opt_  DLGPROC lpDialogFunc,
    _In_      LPARAM lParamInit
    );

typedef LRESULT (WINAPI *CallWindowProcA_t)(
    WNDPROC pWndProc,       
    HWND hwnd,              
    UINT message,           
    WPARAM wParam,          
    LPARAM lParam           
    );

lstrcmpiA_t                  orig_lstrcmpiA = (lstrcmpiA_t)NULL;
NtCreateFile_t               orig_NtCreateFile = (NtCreateFile_t)NULL;
NtOpenFile_t                 orig_NtOpenFile = (NtOpenFile_t)NULL;
NtClose_t				     orig_NtClose = (NtClose_t)NULL;
GetSaveFileNameA_t           orig_GetSaveFileNameA = (GetSaveFileNameA_t)NULL;
MessageBoxA_t                orig_MessageBoxA = (MessageBoxA_t)NULL;
CreateDialogIndirectParamA_t orig_CreateDialogIndirectParamA = (CreateDialogIndirectParamA_t)NULL;
CallWindowProcA_t            orig_CallWindowProcA = (CallWindowProcA_t)NULL;

static HWND f_map_spec_hwnd;
BOOL f_enable_GetSaveFilenameA_hook;
static HWND f_orig_roe;
static HWND f_orig_ab;
static HWND f_orig_wog;
static HWND f_orig_ok;
static BOOL f_enable_sod_postfix;

//
// Hooked functions
//

VOID OnMapSpecificationsOK(HWND hwnd)
{
    uint32_t new_fmt = g_new_format;
    uint32_t src_fmt = g_map_format;
    f_enable_GetSaveFilenameA_hook = TRUE;
    
    f_enable_sod_postfix = FALSE;
    g_convert_on_reload = FALSE;

    // Let maped handle if version is same or higher (unless SOD)
    if ((new_fmt == src_fmt || new_fmt > src_fmt) 
        && new_fmt != H3M_FORMAT_SOD 
        && !(src_fmt == H3M_FORMAT_SOD && new_fmt == H3M_FORMAT_WOG)) {
        SendMessage(g_hwnd_main, WM_SAVERELOADMAP, 0, 0);
        return;
    }

    g_convert_on_reload = TRUE;
    SendMessage(g_hwnd_main, WM_SAVERELOADMAP, 0, 0);
}

static BOOL WINAPI hooked_CallWindowProcA(
    WNDPROC pWndProc,
    HWND hwnd,
    UINT message,
    WPARAM wParam,
    LPARAM lParam
    )
{
    WPARAM new_wParam;

    if (WM_COMMAND == message)
    {
        switch (wParam)
        {
        case IDOK:
            OnMapSpecificationsOK(hwnd);
            break;
        case 0x1337:
            g_new_format = H3M_FORMAT_ROE;
            new_wParam = GetDlgCtrlID(f_orig_ab);
            CheckRadioButton(hwnd, GetDlgCtrlID(f_orig_roe), GetDlgCtrlID(f_orig_wog), GetDlgCtrlID(f_orig_roe));
            break;
        case 0x1337 + 1:
            g_new_format = H3M_FORMAT_AB;
            new_wParam = GetDlgCtrlID(f_orig_ab);
            CheckRadioButton(hwnd, GetDlgCtrlID(f_orig_roe), GetDlgCtrlID(f_orig_wog), GetDlgCtrlID(f_orig_ab));
            break;
        case 0x1337 + 2:
            g_new_format = H3M_FORMAT_SOD;
            // Mark as WoG since there is no real SoD button
            new_wParam = GetDlgCtrlID(f_orig_wog);
            CheckRadioButton(hwnd, GetDlgCtrlID(f_orig_roe), GetDlgCtrlID(f_orig_wog), GetDlgCtrlID(f_orig_wog));
            break;
        case 0x1337 + 3:
            g_new_format = H3M_FORMAT_WOG;
            new_wParam = GetDlgCtrlID(f_orig_wog);
            CheckRadioButton(hwnd, GetDlgCtrlID(f_orig_roe), GetDlgCtrlID(f_orig_wog), GetDlgCtrlID(f_orig_wog));
            break;
        }
    }

    return orig_CallWindowProcA(pWndProc, hwnd, message, wParam, lParam);
}

static int WINAPI hooked_MessageBoxA(
    _In_opt_  HWND hWnd,
    _In_opt_  LPCSTR lpText,
    _In_opt_  LPCSTR lpCaption,
    _In_      UINT uType
    )
{
    int ret;
    //TODO filter only certain messages
    return IDYES;
}

VOID _FixVersionOptions(HWND hwnd)
{
    char dbg[256];
    RECT rc;
    POINT orig_ab_pt;
    POINT orig_wog_pt;

    OutputDebugStringA("_FixVersionOptions");

    f_orig_roe = FindWindowExA(hwnd, NULL, NULL, "The &Restoration of Erathia");
    f_orig_ab = FindWindowExA(hwnd, NULL, NULL, "&Armageddon's Blade");
    f_orig_wog = FindWindowExA(hwnd, NULL, NULL, "In the &Wake of Gods");
    //f_orig_ok = FindWindowExA(hwnd, NULL, NULL, "OK");

    ShowWindow(f_orig_roe, SW_HIDE);
    ShowWindow(f_orig_ab, SW_HIDE);
    ShowWindow(f_orig_wog, SW_HIDE);
    
    GetWindowRect(f_orig_ab, &rc);
    orig_ab_pt.x = rc.left;
    orig_ab_pt.y = rc.top;
    ScreenToClient(hwnd, &orig_ab_pt);

    GetWindowRect(f_orig_wog, &rc);
    orig_wog_pt.x = rc.left;
    orig_wog_pt.y = rc.top;
    ScreenToClient(hwnd, &orig_wog_pt);

    // DEBUG 
    /*SetWindowPos(f_orig_roe, NULL, 0, 90, 0, 0, SWP_NOZORDER | SWP_NOSIZE);
    SetWindowPos(f_orig_ab, NULL, 0, 110, 0, 0, SWP_NOZORDER | SWP_NOSIZE);
    SetWindowPos(f_orig_wog, NULL, 0, 130, 0, 0, SWP_NOZORDER | SWP_NOSIZE);*/

    HFONT hFont;
    LOGFONT lf;

    GetObject(GetStockObject(DEFAULT_GUI_FONT), sizeof(LOGFONT), &lf);
    hFont = CreateFont(lf.lfHeight, lf.lfWidth,
        lf.lfEscapement, lf.lfOrientation, lf.lfWeight,
        lf.lfItalic, lf.lfUnderline, lf.lfStrikeOut, lf.lfCharSet,
        lf.lfOutPrecision, lf.lfClipPrecision, lf.lfQuality,
        lf.lfPitchAndFamily, lf.lfFaceName);
    //TODO cleanup/free

    HWND new_roe = CreateWindowA("BUTTON", "A Strategic Quest", WS_VISIBLE | WS_CHILD | BS_AUTORADIOBUTTON, orig_ab_pt.x, orig_wog_pt.y, 150, 21, hwnd, 0x1337, NULL, 0);
    HWND new_ab = CreateWindowA("BUTTON", "The Succession Wars", WS_VISIBLE | WS_CHILD | BS_AUTORADIOBUTTON, orig_ab_pt.x, orig_ab_pt.y, 150, 21, hwnd, 0x1337+1, NULL, 0);
    HWND new_sod = CreateWindowA("BUTTON", "Shadow of Death", WS_VISIBLE | WS_CHILD | BS_AUTORADIOBUTTON, orig_wog_pt.x, orig_wog_pt.y, 150, 21, hwnd, 0x1337+2, NULL, 0);
    HWND new_wog = CreateWindowA("BUTTON", "The Mandate of Heaven", WS_VISIBLE | WS_CHILD | BS_AUTORADIOBUTTON, orig_wog_pt.x, orig_ab_pt.y, 150, 21, hwnd, 0x1337+3, NULL, 0);

    switch (g_map_format) 
    {
    case H3M_FORMAT_ROE:
        CheckRadioButton(hwnd, 0x1337, 0x1337 + 3, 0x1337);
        break;
    case H3M_FORMAT_AB:
        CheckRadioButton(hwnd, 0x1337, 0x1337 + 3, 0x1337 + 1);
        break;
    case H3M_FORMAT_SOD:
        EnableWindow(new_ab, FALSE);
        CheckRadioButton(hwnd, 0x1337, 0x1337 + 3, 0x1337 + 2);
        break;
    case H3M_FORMAT_WOG:
    default:
        EnableWindow(new_ab, FALSE);
        CheckRadioButton(hwnd, 0x1337, 0x1337 + 3, 0x1337 + 3);
        break;
    }

    SendMessage(new_roe, WM_SETFONT, (WPARAM)hFont, TRUE);
    SendMessage(new_ab, WM_SETFONT, (WPARAM)hFont, TRUE);
    SendMessage(new_sod, WM_SETFONT, (WPARAM)hFont, TRUE);
    SendMessage(new_wog, WM_SETFONT, (WPARAM)hFont, TRUE);

    _snprintf(dbg, sizeof(dbg)-1, "%08X %08X %08X", f_orig_ok, f_orig_roe, f_orig_ab);
    OutputDebugStringA(dbg);
}

VOID _FixNewMapDialog(HWND hwnd)
{
    char dbg[256];
    RECT rc;
    POINT orig_ab_pt;
    POINT orig_wog_pt;

    OutputDebugStringA("_FixVersionOptions");

    HWND orig_roe = FindWindowExA(hwnd, NULL, NULL, "The &Restoration of Erathia");
    HWND orig_ab = FindWindowExA(hwnd, NULL, NULL, "&Armageddon's Blade");
    HWND orig_wog = FindWindowExA(hwnd, NULL, NULL, "In the &Wake of Gods");

    SetWindowTextA(orig_roe, "A Strategic Quest");
    SetWindowTextA(orig_ab, "The Succession Wars");
    SetWindowTextA(orig_wog, "The Mandate of Heaven");
}

HWND WINAPI hooked_CreateDialogIndirectParamA(
    _In_opt_  HINSTANCE hInstance,
    _In_      LPCDLGTEMPLATE lpTemplate,
    _In_opt_  HWND hWndParent,
    _In_opt_  DLGPROC lpDialogFunc,
    _In_      LPARAM lParamInit
    )
{
    HWND hwnd;

    hwnd = orig_CreateDialogIndirectParamA(
        hInstance,
        lpTemplate,
        hWndParent,
        lpDialogFunc,
        lParamInit);

    // Map Specifications
    if (lpTemplate->cx == 260 && lpTemplate->cy == 229) {
        _FixVersionOptions(hwnd);
        f_map_spec_hwnd = hwnd;
    } // New Map Dialog
    else if (lpTemplate->cx == 226 && lpTemplate->cy == 134) {
        _FixNewMapDialog(hwnd);
    }

    return hwnd;
}


BOOL WINAPI hooked_GetSaveFileNameA(
    _Inout_  LPOPENFILENAMEA lpofn
    )
{
    char dest[MAX_PATH] = { 0 };

    if (f_enable_GetSaveFilenameA_hook != FALSE)
    {
        //BOOL ret = orig_GetSaveFileNameA(lpofn);
        OutputDebugStringA(lpofn->lpstrFile);
        OutputDebugStringA("getsavefilename");

        OutputDebugStringA(lpofn->lpstrFile);
        //DeleteFileA(lpofn->lpstrFile);
        if (NULL != g_map_filename) {
            OutputDebugStringA("rmg dll");
            //_snprintf(lpofn->lpstrFile, lpofn->nMaxFile - 1, "%s", g_map_filename);
            lpofn->lpstrFile = strdup(g_map_filename);
            OutputDebugStringA(lpofn->lpstrFile);
        }
        f_enable_GetSaveFilenameA_hook = FALSE;
        return TRUE;
    }

    return orig_GetSaveFileNameA(lpofn);
    //ExitProcess(1337); 
}


BOOL disable_NtCreateFile_hook;
static NTSTATUS NTAPI hooked_NtCreateFile(
    _Out_     PHANDLE FileHandle,
    _In_      ACCESS_MASK DesiredAccess,
    _In_      POBJECT_ATTRIBUTES ObjectAttributes,
    _Out_     PIO_STATUS_BLOCK IoStatusBlock,
    _In_opt_  PLARGE_INTEGER AllocationSize,
    _In_      ULONG FileAttributes,
    _In_      ULONG ShareAccess,
    _In_      ULONG CreateDisposition,
    _In_      ULONG CreateOptions,
    _In_      PVOID EaBuffer,
    _In_      ULONG EaLength)
{
    NTSTATUS ntret = 0;
    static BOOL local_disable;

    if (disable_NtCreateFile_hook != FALSE || local_disable != FALSE)
        goto pass;

    if (NULL != ObjectAttributes
        && NULL != ObjectAttributes->ObjectName
        && NULL != ObjectAttributes->ObjectName->Buffer)
    {
        if (NULL != wcsstr(ObjectAttributes->ObjectName->Buffer, L".h3m"))
        {
            ShareAccess |= FILE_SHARE_READ | FILE_SHARE_WRITE | FILE_SHARE_DELETE;

            local_disable = TRUE;

            h3mlib_ctx_t h3m = NULL;
            uint32_t fmt;
            h3m_read_u(&h3m, ObjectAttributes->ObjectName->Buffer + 4);
            if (h3m != NULL) {
                fmt = h3m_get_format(h3m);
                h3m_exit(&h3m);

                g_map_format = fmt;
                char dbg[256];
                sprintf(dbg, "qqqformat: %08X", fmt);
                OutputDebugStringA(dbg);
            }
            else {
                OutputDebugStringA("Could not open");
                char dbg[256];
                sprintf(dbg, "Error: %d, path: %S", GetLastError(), ObjectAttributes->ObjectName->Buffer);
                OutputDebugStringA(dbg);
            }

            char filename[MAX_PATH];
            _snprintf(filename, sizeof(filename)-1, "%S", ObjectAttributes->ObjectName->Buffer + 4);

            if (g_map_filename != NULL) {
                free(g_map_filename);
                g_map_filename = NULL;
            }
            g_map_filename = strdup(filename);

            if (g_map_filename_w != NULL) {
                free(g_map_filename_w);
                g_map_filename_w = NULL;
            }
            g_map_filename_w = wcsdup(ObjectAttributes->ObjectName->Buffer + 4);

            local_disable = FALSE;
        }
    }

pass:
    ntret = orig_NtCreateFile(FileHandle,
        DesiredAccess,
        ObjectAttributes,
        IoStatusBlock,
        AllocationSize,
        FileAttributes,
        ShareAccess,
        CreateDisposition,
        CreateOptions,
        EaBuffer,
        EaLength);

    return ntret;
}

static NTSTATUS NTAPI hooked_NtOpenFile(
    _Out_     PHANDLE FileHandle,
    _In_      ACCESS_MASK DesiredAccess,
    _In_      POBJECT_ATTRIBUTES ObjectAttributes,
    _Out_     PIO_STATUS_BLOCK IoStatusBlock,
    _In_      ULONG ShareAccess,
    _In_      ULONG OpenOptions)
{
    NTSTATUS ntret;

    if (NULL != ObjectAttributes
        && NULL != ObjectAttributes->ObjectName
        && NULL != ObjectAttributes->ObjectName->Buffer)
    {
        if (NULL != wcsstr(ObjectAttributes->ObjectName->Buffer, L".h3m"))
        {
            ShareAccess |= FILE_SHARE_READ | FILE_SHARE_WRITE | FILE_SHARE_DELETE;
        }
        else if (NULL != wcsstr(ObjectAttributes->ObjectName->Buffer, L".tmp"))
        {
            ShareAccess |= FILE_SHARE_READ | FILE_SHARE_WRITE | FILE_SHARE_DELETE;
        }
    }

    ntret = orig_NtOpenFile(FileHandle,
        DesiredAccess,
        ObjectAttributes,
        IoStatusBlock,
        ShareAccess,
        OpenOptions);

    return ntret;
}

NTSTATUS NTAPI hooked_NtClose(
    _In_  HANDLE Handle
    )
{
    NTSTATUS ret;
    static BOOL local_disable;
    char buf[MAX_PATH] = { 0 };
    char *c = NULL;
    char *mapsroot = NULL;
#if 0
    if (local_disable != FALSE)
        goto pass;
    

    //if (0 != GetFinalPathNameByHandleA(Handle, buf, sizeof(buf), 0))
    {
        //OutputDebugStringA("qq");
        //OutputDebugStringA(buf);
        //if (NULL != (c = strstr(buf, ".h3m")))
        //{
        //    OutputDebugStringA("qz @close for .h3m");
        //    ret = orig_NtClose(Handle);
        //    disable_hook = TRUE;
        //    h3m_decompress(buf, "unc.h3m");
        //    disable_hook = FALSE;

        //    //SendMessage(g_hwnd_main, 0x1338, 0, 0);
        //}
        //else
        //{
        //    ret = orig_NtClose(Handle);
        //}

        if (NULL != (c = strstr(buf, ".h3m")) && g_convert_on_reload != FALSE) {
            ret = orig_NtClose(Handle);
            
            local_disable = TRUE;
            
            local_disable = FALSE;

            return ret;
        }

    }

   
pass:
    ret = orig_NtClose(Handle);

#endif

    return ret;
}

static BOOL _inline_hook_function(const char *dll_name,
    const char *proc_name,
    void *new_proc,
    void **old_proc)
{
    if (NULL == (*old_proc = GetProcAddress(GetModuleHandleA(dll_name),
        proc_name)))
    {
        MessageBoxA(NULL, "GetProcAddress failed", dll_name,
            MB_OK | MB_ICONERROR);
        return FALSE;
    }

    if (FALSE == hook_trampoline_dis_x86(old_proc, new_proc))
    {
        MessageBoxA(NULL, "failed to hook", "gg", MB_OK | MB_ICONERROR);
        return FALSE;
    }

    return TRUE;
}

// NEEDLE: 8B F0 85 F6 74 51 8B 4E 20 8D 46 20
// -0x13
sub_loadh3m_t   orig_sub_loadh3m = (sub_loadh3m_t)0x0045CD6B;

static void __stdcall hooked_sub_loadh3m(const char *h3m_path)
{
    __asm pushad
    //OutputDebugStringA(g_map_filename);
    //OutputDebugStringA(h3m_path);
    __asm popad
    __asm
    {
        CMP g_do_replace, 0
        JE past
        PUSH EAX
        MOV EAX, g_map_filename
        MOV h3m_path, EAX
        POP EAX
        MOV g_do_replace, 0
        past :
    }
    orig_sub_loadh3m(h3m_path);
}

static int WINAPI hooked_lstrcmpiA(
    _In_  LPCSTR lpString1,
    _In_  LPCSTR lpString2
    )
{
    if (NULL != g_map_filename && NULL != strstr(lpString1, g_map_filename))
    {
        return 1;
    }
    else
    {
        return orig_lstrcmpiA(lpString1, lpString2);
    }
}

void hooked_init(void)
{
    _inline_hook_function("user32.dll", "MessageBoxA", hooked_MessageBoxA, (void**)&orig_MessageBoxA);
    _inline_hook_function("user32.dll", "CreateDialogIndirectParamA", hooked_CreateDialogIndirectParamA, (void**)&orig_CreateDialogIndirectParamA);
    _inline_hook_function("comdlg32.dll", "GetSaveFileNameA", hooked_GetSaveFileNameA, (void**)&orig_GetSaveFileNameA);
    _inline_hook_function("ntdll.dll", "NtCreateFile", hooked_NtCreateFile, (void**)&orig_NtCreateFile);
    _inline_hook_function("ntdll.dll", "NtOpenFile", hooked_NtOpenFile, (void**)&orig_NtOpenFile);
    //_inline_hook_function("ntdll.dll", "NtClose", hooked_NtClose, (void**)&orig_NtClose);
    _inline_hook_function("user32.dll", "CallWindowProcA", hooked_CallWindowProcA, (void**)&orig_CallWindowProcA);

    _inline_hook_function("kernel32.dll", "lstrcmpiA", hooked_lstrcmpiA, (void**)&orig_lstrcmpiA);
    hook_trampoline_dis_x86((void **)&orig_sub_loadh3m, hooked_sub_loadh3m);
}
