#include "rmgdialog_fix.h"
#include "globals.h"

#include "traverse_funcs.h"

#include <stdio.h>
#include <CommCtrl.h>
#include <Windows.h>

#pragma warning(disable:4996)

#define GUI_ID_FIRST 40000
#define GUI_ID_LAST  50000

static HWND f_hwnd_templates;
static HWND f_hwnd_rmg_dialog;
static LONG f_orig_rmg_dialog_proc = 0;
static int f_id_ok;

static char *f_templates_path;
static WCHAR *f_last_template;

static BOOL _get_reg_value_a(const char *value_name, const char **pointer)
{
    char value[MAX_PATH] = { 0 };
    DWORD value_length = sizeof(value)-1;
    DWORD type = REG_SZ;
    HKEY key = NULL;
    if (RegOpenKeyA(HKEY_CURRENT_USER, "Software\\h3minternals\\", &key) != ERROR_SUCCESS)
    {
        return FALSE;
    }
    RegQueryValueExA(key, value_name, NULL, &type, (LPBYTE)value, &value_length);
    RegCloseKey(key);

    *pointer = strdup(value);
    
    return TRUE;
}

static BOOL _get_reg_value_w(const WCHAR *value_name, const WCHAR **pointer)
{
    WCHAR value[MAX_PATH] = { 0 };
    DWORD value_length = sizeof(value)-1;
    DWORD type = REG_SZ;
    HKEY key = NULL;
    if (RegOpenKeyA(HKEY_CURRENT_USER, "Software\\h3minternals\\", &key) != ERROR_SUCCESS)
    {
        return FALSE;
    }
    RegQueryValueExW(key, value_name, NULL, &type, (LPBYTE)value, &value_length);
    RegCloseKey(key);

    *pointer = wcsdup(value);

    return TRUE;
}

static BOOL _set_last_template(WCHAR *last_template)
{
    HKEY key = NULL;
    if (RegCreateKeyA(HKEY_CURRENT_USER, "Software\\h3minternals\\", &key) != ERROR_SUCCESS)
    {
        return FALSE;
    }

    RegSetValueExW(key, L"last_template", 0, REG_SZ, (BYTE *)last_template, wcslen(last_template));

    RegCloseKey(key);

    return TRUE;
}

BOOL f_ok_was_clicked = FALSE;

LRESULT CALLBACK new_rmg_dialog_WndProc(HWND hwnd, UINT Message, WPARAM wParam, LPARAM lParam)
{
    switch (Message)
    {
    case WM_COMMAND:
        if (BN_CLICKED == HIWORD(wParam) && f_id_ok == LOWORD(wParam))
        {
            f_ok_was_clicked = TRUE;
            if (NULL != f_templates_path)
            {
                char template_src[MAX_PATH] = {0};
                WCHAR chosen_template[MAX_PATH] = {0};
                int idx = SendMessage(f_hwnd_templates, CB_GETCURSEL, 0, 0);
                SendMessage(f_hwnd_templates, CB_GETLBTEXT, idx, (LPARAM)chosen_template);
                OutputDebugStringW(chosen_template);
                _set_last_template(chosen_template);

                if (0 != wcslen(chosen_template))
                {
                    char *orig_maped_path = NULL;
                    char template_dest[MAX_PATH] = { 0 };

                    _snprintf(template_src, sizeof(template_src)-1, "%s\\%S\\rmg.txt", f_templates_path, chosen_template);
                    _get_reg_value_a("orig_maped_path", &orig_maped_path);
                    _snprintf(template_dest, sizeof(template_dest)-1, "%s\\data\\rmg.txt", orig_maped_path);
                    OutputDebugStringA("Copy");
                    OutputDebugStringA(template_src);
                    OutputDebugStringA(template_dest);
                    DeleteFileA(template_dest);
                    CopyFileA(template_src, template_dest, FALSE);
                    if (NULL != orig_maped_path)
                    {
                        free(orig_maped_path);
                    }
                }
                free(f_templates_path);
            }
        }
        break;
    case WM_DESTROY:
        OutputDebugStringA("qq destroy");
        if (FALSE == f_ok_was_clicked)
        {
            OutputDebugStringA("cancel");
            ExitProcess(1);
        }
        g_selectable_towns = (BST_CHECKED == IsDlgButtonChecked(hwnd, ID_SELECTABLE_TOWNS));
        g_selectable_towns ? OutputDebugStringA("selectable") : OutputDebugStringA("not selectable");
        break;
    case WM_CLOSE:
        OutputDebugStringA("qq close");
        break;
    default:
        break;
    }

    return CallWindowProc((WNDPROC)f_orig_rmg_dialog_proc, hwnd, Message, wParam, lParam);
}

int _GetString(HWND hwnd, const wchar_t **arr, size_t n, const wchar_t **str)
{
    //    OutputDebugStringA();

    for (unsigned int i = 0; i < n; ++i)
    {
        if (NULL != FindWindowExW(hwnd, NULL, NULL, arr[i]))
        {
            *str = arr[i];
            return 0;
        }
    }

    return 1;
}

#define ARR_SIZE(A) sizeof(A)/sizeof(A[0])

#define GET_STRING(HW, STR) \
    if (0 != _GetString(HW, arr_##STR, ARR_SIZE(arr_##STR), str_##STR)) { \
        MessageBoxW(NULL, L"Failed to find GUI element (english version in title) for your language, please report bug in h3mtool thread on heroescommunity.com forums, and provide translation for that string", arr_##STR[0], MB_ICONERROR); \
    }

static VOID _GetLocalizedStrings(HWND hwnd,
    const wchar_t **str_roe,
    const wchar_t **str_ab,
    const wchar_t **str_sod,
    const wchar_t **str_generate
    //const wchar_t **str_random,
    //const wchar_t **str_random2
    )
{
    // Selectable town in Czech: "Volitelná města pro hráče"
    // langs: English, French, Russian Complete, Russian SoD, Czech
    const wchar_t *arr_roe[] = { L"The &Restoration of Erathia", L"The &Restoration of Erathia", L"&Возрождение Эрафии", L"Возрождение Эрафии", L"Obnova Erathie - &RoE" };
    const wchar_t *arr_ab[] = { L"&Armageddon's Blade", L"&Armageddon's Blade", L"&Клинок Армагеддона", L"Клинок дрмагеддона", L"Meč Zkázy - &AB" };
    const wchar_t *arr_sod[] = { L"The &Shadow of Death", L"The &Shadow of Death", L"&Дыхание Смерти", L"Дыхание Смерти", L"Stín smrti - &SoD" };

    const wchar_t *arr_generate[] = { L"&Generate random map", L"&Générer carte aléatoire", L"&Генерировать случайную карту", L"Создать произвольную карту", L"&Generovaní náhodné mapy" };
    //const wchar_t *arr_random[] = { L"Random", L"Aléatoire", L"Случайный", L"Произвольный", L"Náhodné" };
    //const wchar_t *arr_random2[] = { L"Random", L"Aléatoire", L"Случайные", L"Произвольный", L"Náhodná" };

    GET_STRING(hwnd, roe);
    //GET_STRING(hwnd, ab);
    GET_STRING(hwnd, sod);

    GET_STRING(hwnd, generate);
    //GET_STRING(hwnd, random);
    //GET_STRING(hwnd, random2);
}

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

// Apart from containing very messy code, this code does not account for 
// windows DPI settings, which means not all of the dialog can be seen with
// non-standard DPI settings. TODO fix this
VOID FixRMGDialog(HWND hwnd)
{
    const wchar_t *str_roe = NULL;
    const wchar_t *str_ab = NULL;
    const wchar_t *str_sod = NULL;
    const wchar_t *str_generate = NULL;
    //const wchar_t *str_random = NULL;
    //const wchar_t *str_random2 = NULL;

    OutputDebugStringW(L"");

#if 1
    _GetLocalizedStrings(hwnd,
        &str_roe,
        &str_ab,
        &str_sod,
        &str_generate
        //&str_random,
        //&str_random2
        );
#endif

    HWND hwnd_roe = FindWindowExW(hwnd, NULL, NULL, str_roe);
    HWND hwnd_sod = FindWindowExW(hwnd, NULL, NULL, str_sod);
    HWND hwnd_generate = FindWindowExW(hwnd, NULL, NULL, str_generate);
//    HWND hwnd_random = FindWindowExW(hwnd, NULL, NULL, str_random);
    RECT rc;
    POINT pt;
    int ok_x = 0;
    int ok_y = 0;
    int roe_x = 0;
    int roe_y = 0;
    BOOL ok_found = FALSE;

    GetWindowRect(hwnd_roe, &rc);
    pt.x = rc.left;
    pt.y = rc.top;
    ScreenToClient(hwnd, &pt);
    roe_x = pt.x;
    roe_y = pt.y;
    //char wow[256] = { 0 };
    
    //_snprintf(wow, sizeof(wow)-1, "%08x, %08X, %08X", str_roe, str_ab, str_sod);
    //OutputDebugStringA(wow);
    CheckRadioButton(hwnd, GetWindowLong(hwnd_roe, GWL_ID), GetWindowLong(hwnd_sod, GWL_ID), GetWindowLong(hwnd_roe, GWL_ID));
    ShowWindow(hwnd_roe, SW_HIDE);
    //EnableWindow(roe, FALSE);
    CheckDlgButton(hwnd, GetWindowLong(hwnd_generate, GWL_ID), BST_CHECKED);
    //SendMessage(generate, BM_SETCHECK, BST_CHECKED, 0);
    EnableWindow(hwnd_generate, FALSE);
    //ShowWindow(hwnd_ab, SW_HIDE);
    //ShowWindow(hwnd_sod, SW_HIDE);
    SetWindowPos(hwnd, NULL, 0, 0, 345, 247, SWP_NOZORDER | SWP_NOMOVE);

    HWND child = NULL;
    while (NULL != (child = FindWindowExA(hwnd, child, NULL, NULL)))
    {
        WCHAR text[256] = { 0 };
        LONG style = GetWindowLong(child, GWL_STYLE);
        GetWindowTextW(child, text, sizeof(text) / sizeof(text[0]) - 1);
        GetWindowRect(child, &rc);
        pt.x = rc.left;
        pt.y = rc.top;
        ScreenToClient(hwnd, &pt);
        if (2 == wcslen(text)) // First pushbutton is OK button, Russian string undetectable
        {
            ok_found = TRUE;
            ok_x = pt.x;
            ok_y = pt.y;
            f_id_ok = GetWindowLong(child, GWL_ID);
            SetWindowPos(child, NULL, 0, 0, 60, 22, SWP_NOZORDER | SWP_NOMOVE);
            continue;
        }
        else if (pt.x == roe_x // Random buttons are at same x as RoE button. Russian random buttons have too many fucking strings (6 found)
            && pt.y > 200) // Don't check AB / SoD radio buttons which also have same x as RoE
        {
            OutputDebugStringW(text);
            CheckRadioButton(hwnd, GetWindowLong(child, GWL_ID), GetWindowLong(child, GWL_ID), GetWindowLong(child, GWL_ID));
        }

        SetWindowPos(child, NULL, pt.x, pt.y - 100, 0, 0, SWP_NOZORDER | SWP_NOSIZE);
        // 125% dpi scaling:
        //SetWindowPos(child, NULL, pt.x, pt.y - 125, 0, 0, SWP_NOZORDER | SWP_NOSIZE);
        ShowWindow(child, SW_SHOW);
    }

    // Extra content here
    //HWND hwnd_OK = FindWindowExA(hwnd, NULL, NULL, "OK");
    GetWindowRect(child, &rc);
    pt.x = rc.left;
    pt.y = rc.top - 21;
    ScreenToClient(hwnd, &pt);
    
    // If Ok button was not found, set default coords
    if (0 == ok_x)
    {
        ok_x = 156;
        ok_y = 23;
    }

    HWND hwnd_selectable_towns = CreateWindowA("Button", "Selectable player towns",
        WS_VISIBLE | WS_CHILD | BS_AUTOCHECKBOX, pt.x, pt.y, 250, 21, hwnd, (HMENU)ID_SELECTABLE_TOWNS, NULL, 0);
    f_hwnd_templates = CreateWindowW(WC_COMBOBOX, TEXT(""),
        WS_VSCROLL | CBS_DROPDOWNLIST | CBS_HASSTRINGS | WS_CHILD | WS_OVERLAPPED | WS_VISIBLE,
        ok_x + 70, ok_y, 150, 330, hwnd, (HMENU)ID_TEMPLATES, NULL, NULL);
    _get_reg_value_a("templates_path", &f_templates_path);
    if (NULL != f_templates_path)
    {
        WCHAR *last_template = NULL;
        traverse_templates(f_templates_path, "*", f_hwnd_templates);
        _get_reg_value_w(L"last_template", &last_template);
        if (NULL != last_template)
        {
            if (CB_ERR == SendMessageW(f_hwnd_templates, CB_SELECTSTRING, -1, (WPARAM)last_template))
            {
                SendMessageW(f_hwnd_templates, CB_SETCURSEL, 0, 0);
            }
        }
        else
        {
            SendMessageW(f_hwnd_templates, CB_SETCURSEL, 0, 0);
        }
    }
    else
    {
        SendMessageW(f_hwnd_templates, CB_ADDSTRING, 0, (LPARAM)L"No templates");
        SendMessageW(f_hwnd_templates, CB_SETCURSEL, 0, 0);
    }

    SetWindowPos(hwnd, NULL, 0, 0, 473, 471 - 100, SWP_NOMOVE | SWP_NOZORDER);
    // 125% dpi scaling
    //SetWindowPos(hwnd, NULL, 0, 0, 639, 596 - 100, SWP_NOMOVE | SWP_NOZORDER);
    
    f_orig_rmg_dialog_proc = SetWindowLong(hwnd, GWL_WNDPROC, (LONG)new_rmg_dialog_WndProc);

    HWND parent = NULL; //FindWindowA("SDL_app", TITLE);
    _CenterWindow(hwnd, parent ? parent : GetDesktopWindow());
    SetForegroundWindow(hwnd);

    g_hwnd_rmg_dialog = hwnd;
}
