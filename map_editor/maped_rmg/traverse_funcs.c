#include <Windows.h>

#include <stdio.h>

#pragma warning(disable:4996)

// TODO merge all these, they are separate only because of speedcoding incorporated

int traverse_templates(const char *path, const char *search, HWND combobox)
{
    HANDLE hFind = NULL;
    WIN32_FIND_DATAW wfd = { 0 };
    WCHAR searchpath[MAX_PATH] = { 0 };
    WCHAR target[MAX_PATH] = { 0 };
    int ret = 0;

    if (NULL != strstr(path, "\\\\?\\"))
    {
        path += sizeof("\\\\?\\") - 1;
    }

    _snwprintf(searchpath, sizeof(searchpath) / sizeof(searchpath[0]) - 1, L"%S\\%S", path, search);

    //OutputDebugStringW(L"search:");
    //OutputDebugStringW(searchpath);

    if (INVALID_HANDLE_VALUE == (hFind = FindFirstFileW(searchpath, &wfd)))
    {
        char error[256];
        OutputDebugStringA("QQ FindFirstFileW fail:");
        sprintf(error, "%d", GetLastError());
        OutputDebugStringA(error);
        return 1;
    }

    do
    {
        if (0 == wcscmp(wfd.cFileName, L".")
            || 0 == wcscmp(wfd.cFileName, L"..")
            || !(FILE_ATTRIBUTE_DIRECTORY & wfd.dwFileAttributes))
        {
            //OutputDebugStringA("skip");
            continue;
        }

        _snwprintf(target, sizeof(target) / sizeof(target[0]) - 1, L"%S\\%s\\rmg.txt", path, wfd.cFileName);
        if (INVALID_FILE_ATTRIBUTES == GetFileAttributesW(target))
        {
            continue;
        }
        //OutputDebugStringW(L"Template");
        //OutputDebugStringW(target);
        int idx = SendMessageW(combobox, CB_ADDSTRING, 0, (LPARAM)wfd.cFileName);

    } while (FALSE != FindNextFileW(hFind, &wfd));

    FindClose(hFind);

    return 0;
}

int traverse_deleteold(const char *path, const char *search)
{
    HANDLE hFind = NULL;
    WIN32_FIND_DATAW wfd = { 0 };
    WCHAR searchpath[MAX_PATH] = { 0 };
    WCHAR target[MAX_PATH] = { 0 };
    int ret = 0;

    if (NULL != strstr(path, "\\\\?\\"))
    {
        path += sizeof("\\\\?\\") - 1;
    }

    _snwprintf(searchpath, sizeof(searchpath) / sizeof(searchpath[0]) - 1, L"%S\\%S", path, search);

    //OutputDebugStringW(L"search:");
    //OutputDebugStringW(searchpath);

    if (INVALID_HANDLE_VALUE == (hFind = FindFirstFileW(searchpath, &wfd)))
    {
        char error[256];
        OutputDebugStringA("QQ FindFirstFileW fail:");
        sprintf(error, "%d", GetLastError());
        OutputDebugStringA(error);
        return 1;
    }

    do
    {
        OutputDebugStringW(L"delete");

        _snwprintf(target, sizeof(target) / sizeof(target[0]) - 1, L"%S\\%s", path, wfd.cFileName);
        OutputDebugStringW(target);
        DeleteFileW(target);
    } while (FALSE != FindNextFileW(hFind, &wfd));

    FindClose(hFind);

    return 0;
}

int traverse_copy_map(const char *path, const char *search, const char *src_map)
{
    HANDLE hFind = NULL;
    WIN32_FIND_DATAW wfd = { 0 };
    WCHAR searchpath[MAX_PATH] = { 0 };
    WCHAR target[MAX_PATH] = { 0 };
    WCHAR src_map_w[MAX_PATH];
    int ret = 0;

    _snwprintf(src_map_w, sizeof(src_map_w) / sizeof(src_map_w[0]) - 1, L"%S", src_map);

    if (NULL != strstr(path, "\\\\?\\"))
    {
        path += sizeof("\\\\?\\") - 1;
    }

    _snwprintf(searchpath, sizeof(searchpath) / sizeof(searchpath[0]) - 1, L"%S\\%S", path, search);

    //OutputDebugStringW(L"search:");
    //OutputDebugStringW(searchpath);

    if (INVALID_HANDLE_VALUE == (hFind = FindFirstFileW(searchpath, &wfd)))
    {
        char error[256];
        OutputDebugStringA("FindFirstFileW fail:");
        sprintf(error, "%d", GetLastError());
        OutputDebugStringA(error);
        return 1;
    }

    do
    {
        //OutputDebugStringW(wfd.cFileName);
        if (0 == wcscmp(wfd.cFileName, L"..")
            || 2 != wcslen(wfd.cFileName)
            || !(FILE_ATTRIBUTE_DIRECTORY & wfd.dwFileAttributes))
        {
            OutputDebugStringA("skip");
            continue;
        }

        WCHAR *locale = wfd.cFileName;

        _snwprintf(target, sizeof(target) / sizeof(target[0]) - 1, L"%S\\%s\\rm.h3m", path, locale);
        OutputDebugStringW(L"target");
        OutputDebugStringW(target);
        while (INVALID_FILE_ATTRIBUTES != GetFileAttributesW(target))
        {
            WCHAR *c = wcsrchr(target, '.');
            wsprintf(c, L"_.h3m");
            OutputDebugStringW(L"newtarget:");
            OutputDebugStringW(target);
        }
        OutputDebugStringW(L"move:");
        OutputDebugStringW(src_map_w);
        OutputDebugStringW(target);
        char oldpath[MAX_PATH] = { 0 };
        _snprintf(oldpath, sizeof(oldpath)-1, "%s\\%S", path, locale);
        traverse_deleteold(oldpath, "rm*h3m");
        traverse_deleteold(path, "*.tmp");
        CopyFileW(src_map_w, target, FALSE);
        //_patch(path, wfd.cFileName);
        //_writeback(path, wfd.cFileName);
        //_h3m_info(path, wfd.cFileName);
    } while (FALSE != FindNextFileW(hFind, &wfd));

    FindClose(hFind);

    return 0;
}
