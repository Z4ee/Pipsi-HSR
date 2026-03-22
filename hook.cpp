#include "pch.h"

#include "hook.h"

#include "detours/detours.h"

static PVOID ResolveJumpTarget(PVOID pStartAddress)
{
	PBYTE pCurrentAddress = (PBYTE)pStartAddress;

	while (TRUE)
	{
		if (pCurrentAddress[0] == 0x90)
		{
			pCurrentAddress += 1;

			continue;
		}
		else if (pCurrentAddress[0] == 0xE9)
		{
			pCurrentAddress = pCurrentAddress + *(int*)(pCurrentAddress + 0x1) + 0x5;

			continue;
		}
		else if (pCurrentAddress[0] == 0xEB)
		{
			pCurrentAddress = pCurrentAddress + *(char*)(pCurrentAddress + 0x1) + 0x2;

			continue;
		}
		else if (pCurrentAddress[0] == 0xFF && pCurrentAddress[1] == 0x25)
		{
			pCurrentAddress = *(PBYTE*)(pCurrentAddress + *(int*)(pCurrentAddress + 0x2) + 0x6);

			continue;
		}
		else if (pCurrentAddress[0] == 0x48 && pCurrentAddress[1] == 0xFF && pCurrentAddress[2] == 0x25)
		{
			pCurrentAddress = *(PBYTE*)(pCurrentAddress + *(int*)(pCurrentAddress + 0x3) + 0x7);

			continue;
		}
		else if ((pCurrentAddress[0] == 0x48 && pCurrentAddress[1] == 0x8B && pCurrentAddress[2] == 0x05)
			&& ((pCurrentAddress[7] == 0x48 && pCurrentAddress[8] == 0xFF && pCurrentAddress[9] == 0xE0)
				|| (pCurrentAddress[7] == 0xFF && pCurrentAddress[8] == 0xE0)))
		{
			pCurrentAddress = *(PBYTE*)(pCurrentAddress + *(int*)(pCurrentAddress + 0x3) + 0x7);

			continue;
		}

		break;
	}

	return pCurrentAddress;
}

BOOL IsHookActive(const HOOK* pHook)
{
	return pHook->ppTrampoline ? TRUE : FALSE;
}

BOOL CreateHook(HOOK* pHook, PVOID pAddress, PVOID pDetour, BOOL bUseResolveJumpTarget)
{
	if (!pAddress || !pDetour)
	{
		return FALSE;
	}

	pHook->ppTrampoline = new PVOID;
	*pHook->ppTrampoline = bUseResolveJumpTarget ? ResolveJumpTarget(pAddress) : pAddress;

	DetourTransactionBegin();
	DetourUpdateThread(GetCurrentThread());
	DetourAttach(pHook->ppTrampoline, pDetour);

	if (DetourTransactionCommit() != NO_ERROR)
	{
		delete pHook->ppTrampoline;
		pHook->ppTrampoline = NULL;

		return FALSE;
	}

	return TRUE;
}

BOOL RemoveHook(HOOK* pHook, PVOID pDetour)
{
	DetourTransactionBegin();
	DetourUpdateThread(GetCurrentThread());
	DetourDetach(pHook->ppTrampoline, pDetour);

	if (DetourTransactionCommit() != NO_ERROR)
	{
		return FALSE;
	}

	delete pHook->ppTrampoline;
	pHook->ppTrampoline = NULL;

	return TRUE;
}
