#pragma once

#include <types.h>

void memset(void *addr, int data, u32 count);
void memmove(void *src, void *dst, u32 count);
int strcmp(const char *p, const char *q, u32 limit);
u32 strlen(const char *s);