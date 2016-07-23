#include "hooked.h"
#include "patches.h"

#include <Windows.h>
#include <commctrl.h>

BOOL WINAPI DllMain(HINSTANCE hInstDll, DWORD fdwReason, LPVOID lpReserved)
{
    switch (fdwReason)
    {
    case DLL_PROCESS_ATTACH:
        //patches_apply();
        hooked_init();
        //CloseHandle(CreateThread(NULL, 0, _thread, NULL, 0, NULL));
        break;
    }
    return TRUE;
}
