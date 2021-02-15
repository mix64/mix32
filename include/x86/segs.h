#pragma once

/*
 * assembler macros to create x86 segments
 */

/* The 0xC0 means the limit is in 4096-byte units */
/* and (for executable segments) 32-bit mode. */
#define SEG_ASM(type, base, lim, dpl)                           \
    .word(((lim) >> 12) & 0xffff), ((base)&0xffff);             \
    .byte(((base) >> 16) & 0xff), (0x90 | (dpl << 5) | (type)), \
        (0xC0 | (((lim) >> 28) & 0xf)), (((base) >> 24) & 0xff)

#define SEG(type, base, lim, dpl)                           \
    (struct segdesc)                                        \
    {                                                       \
        ((lim) >> 12) & 0xffff, (u32)(base)&0xffff,         \
            ((u32)(base) >> 16) & 0xff, type, 1, dpl, 1,    \
            (u32)(lim) >> 28, 0, 0, 1, 1, (u32)(base) >> 24 \
    }

#define SEG16(type, base, lim, dpl)                         \
    (struct segdesc)                                        \
    {                                                       \
        (lim) & 0xffff, (u32)(base)&0xffff,                 \
            ((u32)(base) >> 16) & 0xff, type, 1, dpl, 1,    \
            (u32)(lim) >> 16, 0, 0, 1, 0, (u32)(base) >> 24 \
    }

/* segmentation */
#define SEG_KCODE 0x08 /* kernel code */
#define SEG_KDATA 0x10 /* kernel data+stack */
#define SEG_UCODE 0x18 /* user code */
#define SEG_UDATA 0x20 /* user data+stack */
#define SEG_TSS 0x28   /* 32bit TSS */

#define DPL_USER 0x3 /* User DPL */
#define STS_T32A 0x9 // Available 32-bit TSS

#define STA_X 0x8 /* Executable segment */
#define STA_W 0x2 /* Writeable (non-executable segments) */
#define STA_R 0x2 /* Readable (executable segments) */

#ifndef __ASSEMBLER__
#include <types.h>
#define NSEGS 6
struct segdesc {
    u32 lim_15_0 : 16;  // Low bits of segment limit
    u32 base_15_0 : 16; // Low bits of segment base address
    u32 base_23_16 : 8; // Middle bits of segment base address
    u32 type : 4;       // Segment type (see STS_ constants)
    u32 s : 1;          // 0 = system, 1 = application
    u32 dpl : 2;        // Descriptor Privilege Level
    u32 p : 1;          // Present
    u32 lim_19_16 : 4;  // High bits of segment limit
    u32 avl : 1;        // Unused (available for software use)
    u32 rsv1 : 1;       // Reserved
    u32 db : 1;         // 0 = 16-bit segment, 1 = 32-bit segment
    u32 g : 1;          // Granularity: limit scaled by 4K when set
    u32 base_31_24 : 8; // High bits of segment base address
} gdt[NSEGS];

// Task state segment format
struct taskstate {
    u32 link; // Old ts selector
    u32 esp0; // Stack pointers and segment selectors
    u16 ss0;  // after an increase in privilege level
    u16 padding1;
    u32 *esp1;
    u16 ss1;
    u16 padding2;
    u32 *esp2;
    u16 ss2;
    u16 padding3;
    void *cr3; // Page directory base
    u32 *eip;  // Saved state from last task switch
    u32 eflags;
    u32 eax; // More saved state (registers)
    u32 ecx;
    u32 edx;
    u32 ebx;
    u32 *esp;
    u32 *ebp;
    u32 esi;
    u32 edi;
    u16 es; // Even more saved state (segment selectors)
    u16 padding4;
    u16 cs;
    u16 padding5;
    u16 ss;
    u16 padding6;
    u16 ds;
    u16 padding7;
    u16 fs;
    u16 padding8;
    u16 gs;
    u16 padding9;
    u16 ldt;
    u16 padding10;
    u16 t;    // Trap on task switch
    u16 iomb; // I/O map base address
} ts;
#endif