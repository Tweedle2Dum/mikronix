#include "memory.h"
#include <stdint.h>

#ifdef memcpy
#undef memcpy
#endif
void* memcpy(void* restrict dest, const void* restrict src, size_t n) {
    uint8_t* dst = (uint8_t*)dest;
    const uint8_t* source = (const uint8_t*)src;

    for (size_t i = 0; i < n; ++i)
        dst[i] = source[i];

    return dest;
}

#ifdef memset
#undef memset
#endif
void* memset(void* s, int c, size_t n) {
    uint8_t* bytes = (uint8_t*)s;

    for (size_t i = 0; i < n; ++i)
        bytes[i] = (uint8_t)c;

    return s;
}

#ifdef memmove
#undef memmove
#endif
void* memmove(void* dest, const void* src, size_t n) {
    uint8_t* dst = (uint8_t*)dest;
    const uint8_t* source = (const uint8_t*)src;

    if (dst == source || n == 0)
        return dest;

    if (dst < source) {
        for (size_t i = 0; i < n; ++i)
            dst[i] = source[i];
    } else {
        for (size_t i = n; i > 0; --i)
            dst[i - 1] = source[i - 1];
    }

    return dest;
}

#ifdef memcmp
#undef memcmp
#endif
int memcmp(const void* s1, const void* s2, size_t n) {
    const uint8_t* a = (const uint8_t*)s1;
    const uint8_t* b = (const uint8_t*)s2;

    for (size_t i = 0; i < n; ++i) {
        if (a[i] != b[i])
            return a[i] < b[i] ? -1 : 1;
    }

    return 0;
}
