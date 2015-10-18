#include <windows.h>

#include "generic/hooks.h"
#include "poc/poc.h"

BOOL WINAPI DllMain(HINSTANCE hinstDLL, DWORD fdwReason, LPVOID lpReserved)
{
    if (DLL_PROCESS_ATTACH == fdwReason) {
        hooks_init();
        poc_init();
    }

	return TRUE;
}
