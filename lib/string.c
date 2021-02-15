#include <string.h>
#include <types.h>
#include <x86/asm.h>

void memset(void *addr, int data, u32 count)
{
    while(count--) {
        *(char*)addr++ = (char)data;
    }
}

void memmove(void *src, void *dst, u32 count)
{
    for (int i = 0; i < count; i++) {
        *(char*)dst++ = *(char*)src++;
    }
}

int strcmp(const char *p, const char *q, u32 limit)
{
    while (*p && limit > 0) {
        if (*p != *q)
            return 0;
        p++, q++, limit--;
    }
    return 1;
}

u32 strlen(const char *s)
{
    int cnt = 0;
    while (s[cnt++]);
    return cnt;
}