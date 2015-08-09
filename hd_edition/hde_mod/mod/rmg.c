// Created by John Åkerblom 2015-01-28

#include "../nt_undoc.h"

#include <hook_utils.h>
#include <inject_utils.h>

#include <windows.h>
#include <string.h>
#include <stdio.h>
#include <stdint.h>
#include <h3mlib.h>

#ifdef _MSC_VER
    #pragma warning(disable:4996)
#endif

NtCreateFile_t          orig_NtCreateFile = (NtCreateFile_t)NULL;
NtOpenFile_t			orig_NtOpenFile = (NtOpenFile_t)NULL;

#define MAPED_NAME_W L"h3maped.exe"
#define DLL_NAME "maped_rmg.dll"
#define TITLE "Heroes of Might & Magic III - HD Edition"

// http://www.codeproject.com/Tips/76427/How-to-bring-window-to-top-with-SetForegroundWindo
void SetForegroundWindowInternal(HWND hWnd)
{
    SetWindowPos(hWnd,HWND_NOTOPMOST, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE);
    SetWindowPos(hWnd,HWND_TOPMOST, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE);
    SetWindowPos(hWnd,HWND_NOTOPMOST, 0, 0, 0, 0, SWP_SHOWWINDOW | SWP_NOMOVE | SWP_NOSIZE);
    return;
    //relation time of SetForegroundWindow lock
    DWORD lockTimeOut = 0;
    HWND  hCurrWnd = GetForegroundWindow();
    DWORD dwThisTID = GetCurrentThreadId(),
        dwCurrTID = GetWindowThreadProcessId(hCurrWnd, 0);

    //we need to bypass some limitations from Microsoft :)
    if (dwThisTID != dwCurrTID)
    {
        AttachThreadInput(dwThisTID, dwCurrTID, TRUE);

        SystemParametersInfo(SPI_GETFOREGROUNDLOCKTIMEOUT, 0, &lockTimeOut, 0);
        SystemParametersInfo(SPI_SETFOREGROUNDLOCKTIMEOUT, 0, 0, SPIF_SENDWININICHANGE | SPIF_UPDATEINIFILE);

        AllowSetForegroundWindow(ASFW_ANY);
    }

    SetForegroundWindow(hWnd);

    if (dwThisTID != dwCurrTID)
    {
        SystemParametersInfo(SPI_SETFOREGROUNDLOCKTIMEOUT, 0, (PVOID)lockTimeOut, SPIF_SENDWININICHANGE | SPIF_UPDATEINIFILE);
        AttachThreadInput(dwThisTID, dwCurrTID, FALSE);
    }
}

// TODO cleanup messy code
BOOL _CreateMap(VOID)
{
	STARTUPINFOW si = { 0 };
	PROCESS_INFORMATION pi = { 0 };
	BOOL bRet = FALSE;
    WCHAR cwd[MAX_PATH] = { 0 };
    WCHAR rmgtmp[MAX_PATH] = { 0 };
    char rmg_dll[MAX_PATH] = { 0 };
    WCHAR h3maped_dir[MAX_PATH] = { 0 };
    WCHAR h3maped_exe[MAX_PATH] = { 0 };
    WCHAR gzip_utils_dest[MAX_PATH] = { 0 };
    HKEY key = NULL;
    char value[MAX_PATH] = { 0 };
    DWORD value_length = sizeof(value)-1;
    DWORD type = REG_SZ;

    GetCurrentDirectoryW(sizeof(cwd)-1, cwd);
    _snwprintf(rmgtmp, sizeof(rmgtmp) / sizeof(WCHAR)-1, L"%s\\data\\maps\\rmgtmp", cwd);
    DeleteFileW(rmgtmp);

    if (RegOpenKeyA(HKEY_CURRENT_USER, "Software\\h3minternals\\", &key) != ERROR_SUCCESS)
    {
        return FALSE;
    }


    value_length = sizeof(value);
    memset(value, 0, sizeof(value));
    if (ERROR_SUCCESS != RegQueryValueExA(key, "orig_maped_path", NULL, &type, (LPBYTE)value, &value_length))
    {
        char fail[64];
        sprintf(fail, "fail %d", GetLastError());
        OutputDebugStringA(fail);
    }

    _snwprintf(h3maped_dir, sizeof(h3maped_dir) / sizeof(WCHAR) - 1, L"%S", value);
    _snwprintf(h3maped_exe, sizeof(h3maped_exe) / sizeof(WCHAR)-1, L"%s\\h3maped.exe", h3maped_dir);
    _snwprintf(gzip_utils_dest, sizeof(gzip_utils_dest) / sizeof(WCHAR)-1, L"%s\\gzip_utils.dll", h3maped_dir);

    if (FALSE == CopyFileW(L"gzip_utils.dll", gzip_utils_dest, FALSE))
    {
        MessageBoxA(0, "Failed to copy gzip_utils.dll to map editor path. Please copy this file manually.", 
            "Error", MB_ICONERROR);
    }

    OutputDebugStringW(L"anymap: orig_maped_path");
    OutputDebugStringW(h3maped_exe);

    value_length = sizeof(rmg_dll);
    memset(rmg_dll, 0, sizeof(rmg_dll));
    if (ERROR_SUCCESS != RegQueryValueExA(key, "maped_rmg_path", NULL, &type, (LPBYTE)rmg_dll, &value_length))
    {
        char fail[64];
        sprintf(fail, "fail %d", GetLastError());
        OutputDebugStringA(fail);
    }
    OutputDebugStringA("anymap: maped_rmg_path");
    OutputDebugStringA(rmg_dll);

    RegCloseKey(key);

	si.cb = sizeof(si);
    si.lpReserved = cwd;
	if (FALSE == (bRet = CreateProcessW(NULL,
        h3maped_exe,
		NULL,
		NULL,
		FALSE,
		CREATE_SUSPENDED,
		NULL,
		NULL,
		&si,
		&pi
		)))
	{
		CloseHandle(pi.hThread);
		CloseHandle(pi.hProcess);
		return FALSE;
	}

    inject_dll(pi.hProcess, rmg_dll, 0);

    ResumeThread(pi.hThread);

    {
        for (;;)
        {
            MSG msg;
            if (0 != PeekMessageA(&msg, NULL, 0, 0, PM_REMOVE))
            {
                TranslateMessage(&msg);
                DispatchMessage(&msg);
            }
            if (WAIT_OBJECT_0 == WaitForSingleObject(pi.hThread, 50))
            {
                break;
            }
        }
    }

    SetForegroundWindowInternal(FindWindowA("SDL_app", TITLE));

	CloseHandle(pi.hThread);
	CloseHandle(pi.hProcess);
	return TRUE;
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
    wchar_t * delim = NULL;

    if (NULL != ObjectAttributes
        && NULL != ObjectAttributes->ObjectName
        && NULL != ObjectAttributes->ObjectName->Buffer)
    {
        if (NULL != wcsstr(ObjectAttributes->ObjectName->Buffer, L".h3m"))
        {
            ShareAccess |= FILE_SHARE_READ | FILE_SHARE_WRITE | FILE_SHARE_DELETE;
        }

        if (NULL != (delim = wcsstr(ObjectAttributes->ObjectName->Buffer, L"\\*.*"))
            && NULL != wcsstr(ObjectAttributes->ObjectName->Buffer, L"maps")
            && NULL == wcsstr(ObjectAttributes->ObjectName->Buffer, L"prefs"))
        {
            _CreateMap();
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

    if (NULL != ObjectAttributes
        && NULL != ObjectAttributes->ObjectName
        && NULL != ObjectAttributes->ObjectName->Buffer)
    {
        if (NULL != wcsstr(ObjectAttributes->ObjectName->Buffer, L".h3m"))
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

    return ntret;
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

void rmg_init(void)
{
    _inline_hook_function("ntdll.dll", "NtOpenFile", hooked_NtOpenFile, (void**)&orig_NtOpenFile);
    _inline_hook_function("ntdll.dll", "NtCreateFile", hooked_NtCreateFile, (void**)&orig_NtCreateFile);
}
