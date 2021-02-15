/* Routines to let C code use special x86 instructions. */

#pragma once

#include <types.h>

static inline void cli() { asm volatile("cli"); }

static inline void hlt() { asm volatile("hlt"); }

static inline u8 inb(u16 port)
{
    u8 data;
    asm volatile("in %1,%0"
                 : "=a"(data)
                 : "d"(port));
    return data;
}

static inline void insl(u16 port, void *addr, u32 cnt)
{
    asm volatile("cld; rep insl"
                 : "=D"(addr), "=c"(cnt)
                 : "d"(port), "0"(addr), "1"(cnt)
                 : "memory", "cc");
}

static inline u32 lcr2()
{
    u32 val;
    asm volatile("movl %%cr2,%0"
                 : "=r"(val));
    return val;
}

static inline u32 leflags(void)
{
    u32 eflags;
    asm volatile("pushfl; popl %0"
                 : "=r"(eflags));
    return eflags;
}

static inline void movsb(const void *src, void *dst, u32 cnt)
{
    asm volatile("cld; rep movsb"
                 : "=D"(dst), "=S"(src), "=c"(cnt)
                 : "0"(dst), "1"(src), "2"(cnt)
                 : "memory", "cc");
}

static inline void outb(u16 port, u8 data)
{
    asm volatile("outb %0,%1"
                 :
                 : "a"(data), "d"(port));
}

static inline void outw(u16 port, u16 data)
{
    asm volatile("outw %0,%1"
                 :
                 : "a"(data), "d"(port));
}

static inline void outsl(u16 port, const void *addr, u32 cnt)
{
    asm volatile("cld; rep outsl"
                 : "=S"(addr), "=c"(cnt)
                 : "d"(port), "0"(addr), "1"(cnt)
                 : "cc");
}

static inline void scr0(u32 flags)
{
    u32 val;
    asm volatile("movl %%cr0,%0"
                 : "=r"(val));
    asm volatile("mov %0, %%cr0" ::"r"(val | flags));
}

static inline void scr3(void *addr)
{
    asm volatile("mov %0, %%cr3" ::"r"(addr));
}

static inline void sti() { asm volatile("sti"); }

static inline void stosb(void *addr, int data, u32 cnt)
{
    asm volatile("cld; rep stosb"
                 : "=D"(addr), "=c"(cnt)
                 : "0"(addr), "1"(cnt), "a"(data)
                 : "memory", "cc");
}

// bochs only
static inline void breakpoint()
{
    outw(0x8a00, 0x8a00);
    outw(0x8a00, 0x8ae0);
}