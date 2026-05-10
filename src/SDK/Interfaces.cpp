#include "Interfaces.h"
#include "HookLib.h"
#include "../Obfuscation.h"
#include <iostream>
#include <windows.h>

static uintptr_t g_GlobalVars = 0;
static uintptr_t g_CSGOInput = 0;

static uintptr_t ResolveRIPOffset(uintptr_t addr, int offsetPos, int instrLen)
{
    int32_t rel = *(int32_t*)(addr + offsetPos);
    return addr + instrLen + rel;
}

static void PrintInterface(const char* name, uintptr_t addr)
{
    if (!addr)
    {
        std::cout << XorStr("[ERROR] ") << name << XorStr(" not found!") << std::endl;
        return;
    }
    uintptr_t base = (uintptr_t)GetModuleHandleA(XorStr("client.dll"));
    std::cout << XorStr("[Interfaces] ") << name << XorStr(" found at RVA: 0x")
        << std::hex << (uint64_t)(addr - base) << std::dec << std::endl;
}

void Interfaces::Initialize()
{
    uintptr_t sig;

    sig = HookLib::FindSignature(XorStr("client.dll"), XorStr("48 89 15 ? ? ? ? 48 89 42"));
    if (sig)
    {
        g_GlobalVars = ResolveRIPOffset(sig, 3, 7);
        PrintInterface(XorStr("GlobalVars"), g_GlobalVars);
    }
    else
    {
        std::cout << XorStr("[ERROR] GlobalVars not found!") << std::endl;
    }

    sig = HookLib::FindSignature(XorStr("client.dll"), XorStr("48 8B 0D ? ? ? ? 4C 8B C6 8B 10 E8"));
    if (sig)
    {
        g_CSGOInput = ResolveRIPOffset(sig, 3, 7);
        PrintInterface(XorStr("CCSGOInput"), g_CSGOInput);
    }
    else
    {
        std::cout << XorStr("[ERROR] CCSGOInput not found!") << std::endl;
    }
}

uintptr_t Interfaces::GetGlobalVars()
{
    return g_GlobalVars;
}

uintptr_t Interfaces::GetCSGOInput()
{
    return g_CSGOInput;
}
