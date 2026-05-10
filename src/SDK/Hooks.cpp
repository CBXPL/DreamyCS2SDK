#include "Hooks.h"
#include "HookLib.h"
#include "../../deps/MinHook.h"
#include "../Obfuscation.h"
#include <iostream>
#include <windows.h>


typedef __int64(__fastcall* DrawScope_t)(__int64 int1, __int64 int2);
DrawScope_t oDrawScope = nullptr;

// You can use this hook if you want to implement scope overlay removal :D
// To do this you can basically replace the original function body to just return;
// Example:
//   __int64 __fastcall hkDrawScope(__int64 int1, __int64 int2) { 
// return 0;
// }

__int64 __fastcall hkDrawScope(__int64 int1, __int64 int2)
{
    return oDrawScope(int1, int2);
}


typedef bool(__fastcall* DrawCrosshair_t)(uintptr_t a1);
DrawCrosshair_t oDrawCrosshair = nullptr;

// You can use this hook to create crosshair removal or force crosshair
// To do this you just replace original function to return true or false;
// Example:
//   bool __fastcall hkDrawCrosshair(uintptr_t a1) {
// return true;  // force crosshair always on
// return false; // remove crosshair
// }

bool __fastcall hkDrawCrosshair(uintptr_t a1)
{
    return oDrawCrosshair(a1);
}

void Hooks::Initialize()
{
    std::cout << XorStr("[Hooks] Initializing...") << std::endl;

    bool result;

    result = HookLib::Hook(XorStr("client.dll"), XorStr("48 8B C4 53 57 48 83 EC ? 48 8B FA 44 0F 29 40 B8"),
        hkDrawScope, (void**)&oDrawScope);
    if (result)
    {
        uintptr_t modBase = (uintptr_t)GetModuleHandleA(XorStr("client.dll"));
        std::cout << XorStr("[Hooks] Hooked hkDrawScope at RVA: 0x")
            << std::hex << (uint64_t)((uintptr_t)oDrawScope - modBase) << std::dec << std::endl;
    }
    else
    {
        std::cout << XorStr("[ERROR] Failed to hook hkDrawScope!") << std::endl;
    }

    result = HookLib::Hook(XorStr("client.dll"), XorStr("48 89 5C 24 08 57 48 83 EC ? 48 8B D9 E8 ? ? ? ? 48 85"),
        hkDrawCrosshair, (void**)&oDrawCrosshair);
    if (result)
    {
        uintptr_t modBase = (uintptr_t)GetModuleHandleA(XorStr("client.dll"));
        std::cout << XorStr("[Hooks] Hooked hkDrawCrosshair at RVA: 0x")
            << std::hex << (uint64_t)((uintptr_t)oDrawCrosshair - modBase) << std::dec << std::endl;
    }
    else
    {
        std::cout << XorStr("[ERROR] Failed to hook hkDrawCrosshair!") << std::endl;
    }
}

void Hooks::Shutdown()
{
    MH_DisableHook(MH_ALL_HOOKS);
}
