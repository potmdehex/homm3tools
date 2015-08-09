// Created by John Åkerblom 2015-07-11
#include <windows.h>
#include <commctrl.h>

#include "util.h"

// Displayed strings
#define MSGTITLE L"HDE Mod Installer"
#define PREINSTALLMSG L"Do you wish to install HDE Mod to the fllowing path? (pressing No will allow you to browse for another path):\n\""
#define POSTINSTALLMSG L"HDE Mod installation complete."
#define BROWSEMSG L"Please choose HD Edition directory (containing HOMM3 2.0.exe)."

#define INSTALLERRORMSG L"There was an error copying files to the HD Edition folder. You can still install by copying manually."
#define INSTALLERRORTITLE MSGTITLE

// Case-insensitive Steam executable name
#define STEAMEXE L"Steam.exe"
// Subpath to HDE in Steam path
#define HDESUBPATH L"steamapps\\common\\Heroes of Might & Magic III - HD Edition"
// "Common" HDE paths. User can still browse manually if different
#define HDEGUESS32 L"C:\\Program Files\\Steam\\steamapps\\common\\Heroes of Might & Magic III - HD Edition\\" HDESUBPATH
#define HDEGUESS64 L"C:\\Program Files (x86)\\Steam\\steamapps\\common\\Heroes of Might & Magic III - HD Edition\\" HDESUBPATH
// Files/folders to install
LPCWSTR INSTALLFILES[] = {
    L"gzip_utils.dll",
    L"h3mpatches",
    L"hde_mod.dll",
    L"maped_rmg.dll",
    L"plugins",
    L"templates"
};

static BOOL _AutodetectHDEPath(OUT LPWSTR lpHDEPath, IN int iMaxLength)
{
    // Try to find HDE path by looking in subfolder of running Steam.exe's path
    if (FALSE != UtilPathFromRunningExeW(STEAMEXE, lpHDEPath, iMaxLength))
    {
        UtilSafeStrCat(lpHDEPath, HDESUBPATH, iMaxLength);

        if (INVALID_FILE_ATTRIBUTES != GetFileAttributesW(lpHDEPath))
        {
            return TRUE;
        }
    }

    // Look for "common" 32/64 bit paths for HDE
    if (INVALID_FILE_ATTRIBUTES != GetFileAttributesW(HDEGUESS64))
    {
        lstrcpynW(lpHDEPath, HDEGUESS64, iMaxLength);
        return TRUE;
    }
    if (INVALID_FILE_ATTRIBUTES != GetFileAttributesW(HDEGUESS32))
    {
        lstrcpynW(lpHDEPath, HDEGUESS32, iMaxLength);
        return TRUE;
    }

    return TRUE;
}

static BOOL _BrowseHDEPath(OUT LPWSTR lpHDEPath, int iMaxLength)
{
    return UtilPathFromBrowseDialogW(BROWSEMSG,
        lpHDEPath, iMaxLength);;
}

static BOOL _Install(IN LPCWSTR lpInstallPath)
{
    return UtilCopyFilesW(INSTALLFILES, 
        sizeof(INSTALLFILES) / sizeof(LPCWSTR), lpInstallPath);
}

int WINAPI wWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, 
    LPWSTR lpCmdLine, int nShowCmd)
{
    WCHAR szwInstallPath[MAX_PATH];
    WCHAR szwInstallMsg[MAX_PATH * 4];
    int iRet = 0;
    BOOL bInstalled = FALSE;

    // Try to autodetect HDE path, asking user to browse as fallback
    if (FALSE == _AutodetectHDEPath(szwInstallPath, 
        sizeof(szwInstallPath) / sizeof(szwInstallPath[0])))
    {
        if (FALSE == _BrowseHDEPath(szwInstallPath,
            sizeof(szwInstallPath) / sizeof(szwInstallPath[0])))
        {
            // Quit if user cancelled browse
            ExitProcess(1);
        }
    }
    
    // Ask if user wants to install to <path> until 'Yes' or 'Cancel' is
    // pressed. 'No' opens browse dialog to select a new path, then asks again.
    do
    {
        UtilStackSafeStrCpy(szwInstallMsg, PREINSTALLMSG);
        UtilStackSafeStrCat(szwInstallMsg, szwInstallPath);
        UtilStackSafeStrCat(szwInstallMsg, L"\"");

        iRet = MessageBoxW(NULL, szwInstallMsg, MSGTITLE,
            MB_ICONINFORMATION | MB_YESNOCANCEL);
        
        switch (iRet)
        {
        // Install if user clicked Yes
        case IDYES:
            if (FALSE == _Install(szwInstallPath))
            {
                MessageBoxW(NULL, INSTALLERRORMSG, INSTALLERRORTITLE,
                    MB_ICONERROR);
            }
            bInstalled = TRUE;
            break;
        // Browse if user clicked No
        case IDNO:
            // Re-open browse dialog until cancel or a real path is selected
            do
            {
                if (FALSE == _BrowseHDEPath(szwInstallPath,
                    sizeof(szwInstallPath) / sizeof(szwInstallPath[0])))
                {
                    // Quit if user cancelled browse
                    ExitProcess(1);
                }
            } while (0 == lstrlenW(szwInstallPath));
            break;
        // Quit if user clicked Cancel
        case IDCANCEL:
        default:
            ExitProcess(1);
        }
    } while (FALSE == bInstalled);

    MessageBoxW(NULL, POSTINSTALLMSG, MSGTITLE, MB_ICONINFORMATION | MB_OK);

    ExitProcess(0);
}

