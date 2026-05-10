#include "Offsets.h"
#include "SDK/HookLib.h"
#include "Obfuscation.h"
#include <iostream>
#include <windows.h>

static uintptr_t ResolveRIP(uintptr_t addr, int offsetPos, int instrLen)
{
    return addr + instrLen + *(int32_t*)(addr + offsetPos);
}

static bool ResolveOffset(const char* name, const char* pattern, int offsetPos, int instrLen, std::ptrdiff_t& out)
{
    uintptr_t sig = HookLib::FindSignature(XorStr("client.dll"), pattern);
    if (!sig) return false;

    uintptr_t base = (uintptr_t)GetModuleHandleA(XorStr("client.dll"));
    uintptr_t absolute = ResolveRIP(sig, offsetPos, instrLen);
    out = (std::ptrdiff_t)(absolute - base);

    std::cout << XorStr("[Offsets] ") << name << XorStr(" RVA: 0x")
        << std::hex << (uint64_t)(absolute - base) << std::dec << std::endl;
    return true;
}

void dreamy_offsets::Initialize()
{
    using namespace offsets::client_dll;

    ResolveOffset(XorStr("dwEntityList"), XorStr("48 89 0D ? ? ? ? E9 ? ? ? ? CC"), 3, 7, dwEntityList);
    ResolveOffset(XorStr("dwViewMatrix"), XorStr("48 8D 0D ? ? ? ? 48 C1 E0 06"), 3, 7, dwViewMatrix);
    ResolveOffset(XorStr("dwLocalPlayerController"), XorStr("48 8B 05 ? ? ? ? 41 89 BE"), 3, 7, dwLocalPlayerController);
}
