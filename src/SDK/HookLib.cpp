#include "HookLib.h"
#include "../../deps/MinHook.h"
#include <windows.h>
#include <vector>
#include <string>

static uintptr_t FindPatternInModule(uintptr_t start, size_t size, const std::vector<std::pair<uint8_t, bool>>& pattern)
{
    for (size_t i = 0; i < size - pattern.size(); i++)
    {
        bool found = true;
        for (size_t j = 0; j < pattern.size(); j++)
        {
            if (pattern[j].second && *(uint8_t*)(start + i + j) != pattern[j].first)
            {
                found = false;
                break;
            }
        }
        if (found)
            return start + i;
    }
    return 0;
}

static std::vector<std::pair<uint8_t, bool>> ParsePattern(const char* pattern)
{
    std::vector<std::pair<uint8_t, bool>> result;
    const char* p = pattern;

    while (*p)
    {
        while (*p == ' ') p++;
        if (!*p) break;

        if (*p == '?')
        {
            result.push_back({ 0, false });
            p++;
            if (*p == '?') p++;
        }
        else
        {
            uint8_t byte = (uint8_t)strtoul(p, nullptr, 16);
            result.push_back({ byte, true });
            p += 2;
        }
    }
    return result;
}

uintptr_t HookLib::FindSignature(const char* moduleName, const char* pattern)
{
    HMODULE hMod = GetModuleHandleA(moduleName);
    if (!hMod) return 0;

    PIMAGE_DOS_HEADER dos = (PIMAGE_DOS_HEADER)hMod;
    PIMAGE_NT_HEADERS nt = (PIMAGE_NT_HEADERS)((uintptr_t)hMod + dos->e_lfanew);
    size_t imageSize = nt->OptionalHeader.SizeOfImage;

    auto parsed = ParsePattern(pattern);
    if (parsed.empty()) return 0;

    return FindPatternInModule((uintptr_t)hMod, imageSize, parsed);
}

bool HookLib::Hook(const char* moduleName, const char* pattern, void* detour, void** original)
{
    uintptr_t addr = FindSignature(moduleName, pattern);
    if (!addr) return false;

    if (MH_CreateHook((void*)addr, detour, original) != MH_OK)
        return false;

    if (MH_EnableHook((void*)addr) != MH_OK)
        return false;

    return true;
}
