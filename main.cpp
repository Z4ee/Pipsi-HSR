#include "pch.h"

#include "main.h"

#include "menu.h"

BOOL WINAPI DllMain(HINSTANCE hinstDLL, DWORD fdwReason, LPVOID lpvReserved)
{
	if (fdwReason == DLL_PROCESS_ATTACH)
	{
		//AllocConsole();

		//freopen_s((FILE**)stdout, "CONOUT$", "w", stdout);
		//freopen_s((FILE**)stderr, "CONOUT$", "w", stderr);

		//printf("hinstDLL: %p\n", hinstDLL);

		if (!Menu::Setup())
		{
			return FALSE;
		}
	}

	return TRUE;
}
