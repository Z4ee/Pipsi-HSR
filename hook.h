#pragma once
#include <windows.h>

struct HOOK
{
    PVOID* ppTrampoline;
};

BOOL IsHookActive(const HOOK* pHook);

BOOL CreateHook(HOOK* pHook, PVOID pAddress, PVOID pDetour, BOOL bUseResolveJumpTarget);
BOOL RemoveHook(HOOK* pHook, PVOID pDetour);

#define CALL_ORIGINAL(Hook, Detour, ...) ((decltype(Detour)*)(*(Hook).ppTrampoline))(__VA_ARGS__)
