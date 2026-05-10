#pragma once

template <size_t N>
struct XorStr_t
{
    char data[N]{};

    constexpr XorStr_t(const char(&str)[N])
    {
        for (size_t i = 0; i < N; i++)
            data[i] = str[i] ^ (i % 3 == 0 ? 0xEF : i % 3 == 1 ? 0x1A : 0xED);
    }

    const char* decrypt()
    {
        volatile char vol = data[0];
        (void)vol;
        for (size_t i = 0; i < N; i++)
            data[i] = data[i] ^ (i % 3 == 0 ? 0xEF : i % 3 == 1 ? 0x1A : 0xED);
        return data;
    }
};

#define XorStr(str) XorStr_t(str).decrypt()
