#include <stddef.h>

void *memset(void *dest, int value, size_t count)
{
    unsigned char *bytes = (unsigned char *)dest;

    for (size_t i = 0; i < count; ++i)
        bytes[i] = (unsigned char)value;

    return dest;
}

void *memcpy(void *restrict dest, const void *restrict src, size_t count)
{
    unsigned char *dst = (unsigned char *)dest;
    const unsigned char *source = (const unsigned char *)src;

    for (size_t i = 0; i < count; ++i)
        dst[i] = source[i];

    return dest;
}

void *memmove(void *dest, const void *src, size_t count)
{
    unsigned char *dst = (unsigned char *)dest;
    const unsigned char *source = (const unsigned char *)src;

    if (dst == source || count == 0)
        return dest;

    if (dst < source) {
        for (size_t i = 0; i < count; ++i)
            dst[i] = source[i];
    } else {
        for (size_t i = count; i > 0; --i)
            dst[i - 1] = source[i - 1];
    }

    return dest;
}

int memcmp(const void *lhs, const void *rhs, size_t count)
{
    const unsigned char *a = (const unsigned char *)lhs;
    const unsigned char *b = (const unsigned char *)rhs;

    for (size_t i = 0; i < count; ++i) {
        if (a[i] != b[i])
            return a[i] < b[i] ? -1 : 1;
    }

    return 0;
}

size_t strlen(const char *str)
{
    size_t len = 0;
    while (str[len] != '\0')
        ++len;
    return len;
}
