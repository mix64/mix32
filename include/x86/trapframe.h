#pragma once

#include <types.h>

// Layout of the trap frame built on the stack by the
// hardware and by trapasm.S, and passed to trap().
// sizeof TrapFrame = 76
struct trapframe {
    // registers as pushed by pushal
    u32 edi;
    u32 esi;
    u32 ebp;
    u32 oesp; // useless & ignored
    u32 ebx;
    u32 edx;
    u32 ecx;
    u32 eax;

    // rest of trap frame
    u16 gs;
    u16 padding1;
    u16 fs;
    u16 padding2;
    u16 es;
    u16 padding3;
    u16 ds;
    u16 padding4;
    u32 trapno;

    // below here defined by x86 hardware
    u32 err;
    u32 eip;
    u16 cs;
    u16 padding5;
    u32 eflags;

    // below here only when crossing rings, such as from user to kernel
    u32 esp;
    u16 ss;
    u16 padding6;
};
