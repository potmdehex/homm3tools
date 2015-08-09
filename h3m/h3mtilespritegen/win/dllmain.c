/* Created by John Åkerblom 2014-08-25 */
#if defined _WIN32

#include <windows.h>

//debug
#include <stdio.h>

#pragma warning(disable:4996)

BOOL WINAPI DllMain(_In_  HINSTANCE hinstDLL,
	_In_  DWORD fdwReason,
	_In_  LPVOID lpvReserved
	)
{
	switch (fdwReason)
	{
	case DLL_PROCESS_ATTACH:
		//debug
		if (FALSE != AllocConsole())
		{
			freopen("CONIN$", "rb", stdin);
			freopen("CONOUT$", "wb", stdout);
			freopen("CONOUT$", "wb", stderr);
		}
		break;
	case DLL_PROCESS_DETACH:
		break;
	}
	
	return TRUE;
}

#else 
	/* warning: ISO C forbids an empty translation unit [-Wpedantic] */
	typedef void empty; 
#endif


