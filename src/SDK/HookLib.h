#pragma once
#include <cstdint>

namespace HookLib
{
    uintptr_t FindSignature(const char* moduleName, const char* pattern);
    bool Hook(const char* moduleName, const char* pattern, void* detour, void** original);
}
