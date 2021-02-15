#pragma once

#include <types.h>

struct cpu {
    struct proc *procp;
    u32 nextpid;
    u16 intr_depth;
    u16 intr_enable;
    u32 sched;
    u32 tick;
} cpu;
