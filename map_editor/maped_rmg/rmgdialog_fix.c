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
static HWND f_hwnd_selectable_towns;
static HWND f_hwnd_town_comboboxes[8];
static LONG f_orig_rmg_dialog_proc = 0;
static HWND f_hwnd_human_players_combobox;
static HWND f_hwnd_computer_players_combobox;
static int f_id_ok;


static char *f_templates_path;
static WCHAR *f_last_template;

static const WCHAR * const town_names[] = { 
    L"Random",
    L"Castle",
    L"Rampart",
    L"Tower",
    L"Inferno",
    L"Necropolis",
    L"Dungeon",
    L"Stronghold",
    L"Fortress"
    // RoE - no Conflux
};

static const WCHAR * const player_names[] = {
    L"Red",
    L"Blue",
    L"Tan",
    L"Green",
    L"Orange",
    L"Purple",
    L"Teal",
    L"Pink"
};

static BOOL _get_reg_value_a(const char *value_name, char **pointer)
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

static BOOL _get_reg_value_w(const WCHAR *value_name, WCHAR **pointer)
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
        if (CBN_SELCHANGE == HIWORD(wParam))
        {
            int humans = SendMessage(f_hwnd_human_players_combobox, CB_GETCURSEL, 0, 0);
            int computers = SendMessage(f_hwnd_computer_players_combobox, CB_GETCURSEL, 0, 0);
            if (0 != humans && 0 != computers) // If neither humans nor computers is random number, disable town selection for unused players
            {
                // Humans combobox starts with Random, 1, 2 ...
                // But Computers starts with Random, 0, 1, ...
                // Adjust for this
                computers -= 1;

                for (int i = 0; i < 8; ++i)
                {
                    BOOL bEnable = i < (humans + computers);
                    EnableWindow(f_hwnd_town_comboboxes[i], bEnable);
                }
            }
            else // At least one combobox is random, enable all town selections
            {
                for (int i = 0; i < 8; ++i)
                {
                    EnableWindow(f_hwnd_town_comboboxes[i], TRUE);
                }
            }
        }
        else if (BN_CLICKED == HIWORD(wParam) && f_id_ok == LOWORD(wParam))
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
				f_templates_path = NULL;
            }
            for (int i = 0; i < 8; ++i)
            {
                g_selected_towns[i] = SendMessageA(f_hwnd_town_comboboxes[i], CB_GETCURSEL, 0, 0) - 1; // - 1 so Random becomes -1, Castle becomes 0 etc
            }
        }
        else if (BN_CLICKED == HIWORD(wParam) && ID_SELECTABLE_TOWNS == LOWORD(wParam))
        {
            static BOOL bEnable;
            for (int i = 0; i < 8; ++i)
            {
                EnableWindow(f_hwnd_town_comboboxes[i], bEnable);
            }
            bEnable = !bEnable;
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

// TODO clean up this ultra messy copy paste/trash code.
VOID FixRMGDialog(HWND hwnd)
{
    const wchar_t *str_roe = NULL;
    const wchar_t *str_ab = NULL;
    const wchar_t *str_sod = NULL;
    const wchar_t *str_generate = NULL;
    //const wchar_t *str_random = NULL;
    //const wchar_t *str_random2 = NULL;

	RECT rc;
	POINT pt;
	int main_w;
	int main_h;
	int roe_x;
	int roe_y;
	int gen_x;
	int gen_y;
	int ok_x;
	int ok_y;
	int mapversion_x;
	int mapversion_y;
	int element_count = 0;
    HWND child = NULL;
	//    HWND hwnd_random = FindWindowExW(hwnd, NULL, NULL, str_random);

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
	HWND hwnd_ab = FindWindowExW(hwnd, NULL, NULL, str_ab);
	HWND hwnd_sod = FindWindowExW(hwnd, NULL, NULL, str_sod);
	HWND hwnd_generate = FindWindowExW(hwnd, NULL, NULL, str_generate);

	// Get size of main dialog
	GetWindowRect(hwnd, &rc);
	pt.x = rc.right - rc.left;
	pt.y = rc.bottom - rc.top;
	main_w = pt.x;
	main_h = pt.y;

	// Get pos of RoE checkbox
	GetWindowRect(hwnd_roe, &rc);
	pt.x = rc.left;
	pt.y = rc.top;
	ScreenToClient(hwnd, &pt);
	roe_x = pt.x;
	roe_y = pt.y;

	// Get pos of Generate checkbox
	GetWindowRect(hwnd_generate, &rc);
	pt.x = rc.left;
	pt.y = rc.top;
	ScreenToClient(hwnd, &pt);
	gen_x = pt.x;
	gen_y = pt.y;

    // Set RoE format and Enable RMG
    CheckRadioButton(hwnd, GetWindowLong(hwnd_roe, GWL_ID), GetWindowLong(hwnd_sod, GWL_ID), GetWindowLong(hwnd_roe, GWL_ID));
    CheckDlgButton(hwnd, GetWindowLong(hwnd_generate, GWL_ID), BST_CHECKED);
    EnableWindow(hwnd_generate, FALSE);

	// Need to manually set RMG options to show and set the default settings (normally done when
	// the Generate checkbox is checked)
	element_count = 0;
	for (child = NULL; NULL != (child = FindWindowExA(hwnd, child, NULL, NULL)); ++element_count)
	{
		WCHAR text[256] = { 0 };
		char classname[256] = { 0 }; 
		LONG style = GetWindowLong(child, GWL_STYLE);
        int w;
        int h;
		GetWindowTextW(child, text, sizeof(text) / sizeof(text[0]) - 1);
		GetWindowTextA(child, classname, sizeof(classname)-1);
		GetWindowRect(child, &rc);
		pt.x = rc.left;
		pt.y = rc.top;
        w = rc.right - rc.left;
        h = rc.bottom - rc.top;
		ScreenToClient(hwnd, &pt);

		OutputDebugStringW(text);
        char s[256] = { 0 };
        sprintf(s, "element :%d", element_count);
        OutputDebugStringA(s);

		if (pt.x == roe_x && pt.y > gen_y) // Detect default radio button for Water content and Monster strength
		{
			CheckRadioButton(hwnd, GetWindowLong(child, GWL_ID), GetWindowLong(child, GWL_ID), GetWindowLong(child, GWL_ID));
		}
		else if (element_count < 4) // First 4 elements are map version settings, hide them by moving them away (because program shows again later)
		{
			if (0 == element_count) // First element is the map version group box
			{
				// Get pos of map version group box
				GetWindowRect(child, &rc);
				pt.x = rc.left;
				pt.y = rc.top;
				ScreenToClient(hwnd, &pt);
				mapversion_x = pt.x;
				mapversion_y = pt.y;
			}
			MoveWindow(child, -50, -50, 0, 0, TRUE);
		}
        else if (element_count == 9) // Two level map checkbox, make it shorter in width so it does not interfere with extra GUI elements
        {
            MoveWindow(child, pt.x, pt.y, 200, h, TRUE);
        }
        else if (element_count == 14)
        {
            //f_id_human_players_combobox = GetDlgCtrlID(child);
            f_hwnd_human_players_combobox = child;
        }
        else if (element_count == 16)
        {
            //f_id_computer_players_combobox = GetDlgCtrlID(child);
            f_hwnd_computer_players_combobox = child;
        }
        else if (element_count >= 32) // Elements 32-34 are OK, Cancel and Help buttons, move them to make space for more GUI elements
        {
            if (32 == element_count) // Save OK button position
            {
                f_id_ok = GetWindowLong(child, GWL_ID);
                ok_x = pt.x;
                ok_y = pt.y;
            }
            
            MoveWindow(child, pt.x + (w * 4 / 3), pt.y, w, h, TRUE);
        }
        

		// Set window to be shown, revealing the RMG options
		ShowWindow(child, SW_SHOW);
	}

    // Extra content here
    f_hwnd_templates = CreateWindowW(WC_COMBOBOX, TEXT(""),
        WS_VSCROLL | CBS_DROPDOWNLIST | CBS_HASSTRINGS | WS_CHILD | WS_OVERLAPPED | WS_VISIBLE,
		gen_x, ok_y, 150, 330, hwnd, (HMENU)ID_TEMPLATES, NULL, NULL);
	f_hwnd_selectable_towns = CreateWindowA("Button", "Selectable Player Towns",
		WS_VISIBLE | WS_CHILD | BS_AUTOCHECKBOX, ok_x, ok_y + (181), 200, 21, hwnd, (HMENU)ID_SELECTABLE_TOWNS, NULL, 0);
    
    HFONT font = (HFONT)SendMessage(hwnd_roe, WM_GETFONT, 0, 0);
    if (NULL == font)
    {
        font = GetStockObject(DEFAULT_GUI_FONT);
    }

    SendMessage(f_hwnd_selectable_towns, WM_SETFONT, (WPARAM)font, TRUE);
    SendMessage(f_hwnd_templates, WM_SETFONT, (WPARAM)font, TRUE);

    for (int i = 0; i < 8; ++i)
    {
        f_hwnd_town_comboboxes[i] = CreateWindowW(WC_COMBOBOX, TEXT(""),
            WS_VSCROLL | CBS_DROPDOWNLIST | CBS_HASSTRINGS | WS_CHILD | WS_OVERLAPPED | WS_VISIBLE,
            ok_x, ok_y + (23 * i), 120, 330, hwnd, (HMENU)NULL, NULL, NULL);

        SendMessage(f_hwnd_town_comboboxes[i], WM_SETFONT, (WPARAM)font, TRUE);
        for (int j = 0; j < sizeof(town_names) / sizeof(town_names[0]); ++j)
        {
            WCHAR str[256]; 
            _snwprintf(str, sizeof(str) / sizeof(str[0]) - 1, L"%s (%s)", town_names[j], player_names[i]);
            SendMessageW(f_hwnd_town_comboboxes[i], CB_ADDSTRING, 0, (LPARAM)str);
        }
        SendMessageW(f_hwnd_town_comboboxes[i], CB_SETCURSEL, 0, 0);
    }


	if (NULL != f_templates_path)
	{
		free(f_templates_path);
		f_templates_path = NULL;
	}
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

			free(last_template);
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

	SetWindowPos(hwnd, NULL, 0, 0, (int)((float)main_w * 1.371f), (int)((float)main_h * 1.907f), SWP_NOMOVE | SWP_NOZORDER);

    f_orig_rmg_dialog_proc = SetWindowLong(hwnd, GWL_WNDPROC, (LONG)new_rmg_dialog_WndProc);

    _CenterWindow(hwnd, GetDesktopWindow());
    SetForegroundWindow(hwnd);

    g_hwnd_rmg_dialog = hwnd;
}

// TODO:
// * if selectable player towns is selected, disable all the town comboboxes
// * if player amount is selected (not random), disable all extraneous players