#include "hooked.h"
#include "globals.h"
#include "gui.h"
#include "nt_undoc.h"

#include <h3mlib.h>

#include <hook_utils.h>

#include <windows.h>
#include <CommCtrl.h>
#include <string.h>
#include <stdio.h>

#pragma warning(disable:4996)

typedef void(__stdcall *sub_0046FAC8_t)(int a1, DWORD *xy);

typedef BOOL(WINAPI *EnableMenuItem_t)(
    _In_  HMENU hMenu,
    _In_  UINT uIDEnableItem,
    _In_  UINT uEnable
    );

typedef BOOL(WINAPI *SetMenuItemInfoA_t)(
    _In_  HMENU hMenu,
    _In_  UINT uItem,
    _In_  BOOL fByPosition,
    _In_  LPMENUITEMINFOA lpmii
    );

typedef int (WINAPI *lstrcmpiA_t)(
    _In_  LPCSTR lpString1,
    _In_  LPCSTR lpString2
    );

typedef int (WINAPI *MessageBoxA_t)(
    _In_opt_  HWND hWnd,
    _In_opt_  LPCSTR lpText,
    _In_opt_  LPCSTR lpCaption,
    _In_      UINT uType
    );

typedef HWND(WINAPI *CreateDialogIndirectParamA_t)(
    _In_opt_  HINSTANCE hInstance,
    _In_      LPCDLGTEMPLATE lpTemplate,
    _In_opt_  HWND hWndParent,
    _In_opt_  DLGPROC lpDialogFunc,
    _In_      LPARAM lParamInit
    );

typedef BOOL(WINAPI *SetScrollInfo_t)(
    _In_  HWND hwnd,
    _In_  int fnBar,
    _In_  LPCSCROLLINFO lpsi,
    _In_  BOOL fRedraw
    );

typedef LRESULT(WINAPI *SendMessageA_t)(
    _In_  HWND hWnd,
    _In_  UINT Msg,
    _In_  WPARAM wParam,
    _In_  LPARAM lParam
    );

//
// Globals
//
BOOL g_hooked_ScrollInfo_disable = TRUE;
BOOL g_hooked_NtCreateFile_disable = FALSE;
DWORD g_hooked_latest_x = 0;
DWORD g_hooked_latest_y = 0;

static HANDLE	f_handle_current_h3m;
static BOOL		f_haswritten;
static wchar_t  *f_oldmap;
static wchar_t  *f_newmap;

NtQueryDirectoryFile_t	orig_NtQueryDirectoryFile = (NtQueryDirectoryFile_t)NULL;
NtCreateFile_t			orig_NtCreateFile = (NtCreateFile_t)NULL;
NtOpenFile_t			orig_NtOpenFile = (NtOpenFile_t)NULL;
NtSetInformationFile_t	orig_NtSetInformationFile = (NtSetInformationFile_t)NULL;
NtWriteFile_t			orig_NtWriteFile = (NtWriteFile_t)NULL;
NtClose_t				orig_NtClose = (NtClose_t)NULL;

TrackPopupMenu_t        orig_TrackPopupMenu = (TrackPopupMenu_t)NULL;
AppendMenuA_t           orig_AppendMenuA = (AppendMenuA_t)NULL;
EnableMenuItem_t        orig_EnableMenuItem = (EnableMenuItem_t)NULL;
SetMenuItemInfoA_t      orig_SetMenuItemInfoA = (SetMenuItemInfoA_t)NULL;
MessageBoxA_t           orig_MessageBoxA = (MessageBoxA_t)NULL;
SetScrollInfo_t         orig_SetScrollInfo = (SetScrollInfo_t)NULL;
CreateDialogIndirectParamA_t orig_CreateDialogIndirectParamA = (CreateDialogIndirectParamA_t)NULL;
SendMessageA_t          orig_SendMessageA = (SendMessageA_t)NULL;
lstrcmpiA_t             orig_lstrcmpiA = (lstrcmpiA_t)NULL;

sub_loadh3m_t   orig_sub_loadh3m = (sub_loadh3m_t)0x0045D166;

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
        past :
    }
    orig_sub_loadh3m(h3m_path);
}

//
// Hooked functions
//
static BOOL WINAPI hooked_EnableMenuItem(
    _In_  HMENU hMenu,
    _In_  UINT uIDEnableItem,
    _In_  UINT uEnable
    )
{
    /*if (uIDEnableItem >= GUI_ID_FIRST && uIDEnableItem <= GUI_ID_LAST)
    {*/
    uEnable &= ~MF_GRAYED;
    uEnable &= ~MF_DISABLED;
    uEnable |= MF_ENABLED;
    //}

    return orig_EnableMenuItem(hMenu, uIDEnableItem, uEnable);
}

BOOL WINAPI hooked_SetMenuItemInfoA(
    _In_  HMENU hMenu,
    _In_  UINT uItem,
    _In_  BOOL fByPosition,
    _In_  LPMENUITEMINFOA lpmii
    )
{
    /*if (uIDEnableItem >= GUI_ID_FIRST && uIDEnableItem <= GUI_ID_LAST)
    {*/
    lpmii->fState &= ~MF_GRAYED;
    lpmii->fState &= ~MF_DISABLED;
    lpmii->fState |= MF_ENABLED;
    //}

    return orig_SetMenuItemInfoA(hMenu, uItem, fByPosition, lpmii);
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

static int WINAPI hooked_MessageBoxA(
    _In_opt_  HWND hWnd,
    _In_opt_  LPCSTR lpText,
    _In_opt_  LPCSTR lpCaption,
    _In_      UINT uType
    )
{
    if (0 != g_do_replace && NULL != strstr(lpText, "Save changes"))
    {
        return IDYES;
    }

    return orig_MessageBoxA(hWnd, lpText, lpCaption, uType);
}

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
    NTSTATUS ntret;
    BOOL save_handle = FALSE;

    if (FALSE == g_hooked_NtCreateFile_disable
        && NULL != ObjectAttributes
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

    if (FALSE != save_handle)
    {
        f_handle_current_h3m = *FileHandle;
    }

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

static BOOL _inline_hook_function(const char *dll_name, const char *proc_name, void *new_proc, void **old_proc)
{
    if (NULL == (*old_proc = GetProcAddress(GetModuleHandleA(dll_name), proc_name)))
    {
        MessageBoxA(NULL, "GetProcAddress failed", dll_name, MB_OK | MB_ICONERROR);
        return FALSE;
    }

    if (FALSE == hook_trampoline_dis_x86(old_proc, new_proc))
    {
        char error[128] = { 0 };
        sprintf(error, "Could not hook proc: '%s' in dll: '%s'", proc_name, dll_name);
        MessageBoxA(NULL, error, "gg", MB_OK | MB_ICONERROR);
        return FALSE;
    }

    return TRUE;
}

void hooked_init(void)
{
    _inline_hook_function("ntdll.dll", "NtCreateFile", hooked_NtCreateFile, (void**)&orig_NtCreateFile);
    _inline_hook_function("ntdll.dll", "NtOpenFile", hooked_NtOpenFile, (void**)&orig_NtOpenFile);
    _inline_hook_function("user32.dll", "EnableMenuItem", hooked_EnableMenuItem, (void**)&orig_EnableMenuItem);
    _inline_hook_function("user32.dll", "SetMenuItemInfoA", hooked_SetMenuItemInfoA, (void**)&orig_SetMenuItemInfoA);
    _inline_hook_function("user32.dll", "MessageBoxA", hooked_MessageBoxA, (void**)&orig_MessageBoxA);
    _inline_hook_function("kernel32.dll", "lstrcmpiA", hooked_lstrcmpiA, (void**)&orig_lstrcmpiA);
    hook_trampoline_dis_x86((void **)&orig_sub_loadh3m, hooked_sub_loadh3m);
}

