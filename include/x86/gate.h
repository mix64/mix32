#pragma once

#include <types.h>

#define NGATES 256
// Gate descriptors for interrupts and traps
struct gatedesc {
    u32 off_15_0 : 16;  // low 16 bits of offset in segment
    u32 cs : 16;        // code segment selector
    u32 args : 5;       // # args, 0 for interrupt/trap gates
    u32 rsv1 : 3;       // reserved(should be zero I guess)
    u32 type : 4;       // type(STS_{IG32,TG32})
    u32 s : 1;          // must be 0 (system)
    u32 dpl : 2;        // descriptor(meaning new) privilege level
    u32 p : 1;          // Present
    u32 off_31_16 : 16; // high bits of offset in segment
} idt[NGATES];

#define STS_IG32 0xE // 32-bit Interrupt Gate
#define STS_TG32 0xF // 32-bit Trap Gate

// Set up a normal interrupt/trap gate descriptor.
// - istrap: 1 for a trap (= exception) gate, 0 for an interrupt gate.
//   interrupt gate clears EFLAGS_IF, trap gate leaves EFLAGS_IF alone
// - sel: Code segment selector for interrupt/trap handler
// - off: Offset in code segment for interrupt/trap handler
// - dpl: Descriptor Privilege Level -
//        the privilege level required for software to invoke
//        this interrupt/trap gate explicitly using an int instruction.
#define SETGATE(gate, istrap, sel, off, d)                 \
    {                                                      \
        (gate).off_15_0  = (u32)(off)&0xffff;              \
        (gate).cs        = (sel);                          \
        (gate).args      = 0;                              \
        (gate).rsv1      = 0;                              \
        (gate).type      = (istrap) ? STS_TG32 : STS_IG32; \
        (gate).s         = 0;                              \
        (gate).dpl       = (d);                            \
        (gate).p         = 1;                              \
        (gate).off_31_16 = (u32)(off) >> 16;               \
    }
