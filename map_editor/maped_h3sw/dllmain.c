#include "hooked.h"
#include "patches.h"
#include "gui.h"

#include <Windows.h>
#include <commctrl.h>

#include "globals.h"

DWORD WINAPI _thread(LPVOID lpParameter)
{
    MSG msg = { 0 };

    gui_init();

    while (0 != GetMessage(&msg, NULL, 0, 0))
    {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }

    ExitThread(0);
}

BOOL WINAPI DllMain(HINSTANCE hInstDll, DWORD fdwReason, LPVOID lpReserved)
{
    switch (fdwReason)
    {
    case DLL_PROCESS_ATTACH:
        //patches_apply();

        g_map_filename = strdup("Untitled.h3m");
        hooked_init();
        CloseHandle(CreateThread(NULL, 0, _thread, NULL, 0, NULL));
        break;
    }
    return TRUE;
}
