#include <intr.h>
#include <arch.h>
#include <cpu.h>
#include <proc.h>

inline void disable_intr()
{
    arch_disable_intr();
}

inline void enable_intr()
{
    arch_enable_intr();
}

void timer_intr()
{
    cpu.tick++;
    cpu.procp->cpu++;
    if (cpu.tick % 1000 == 0) {
        sched();
    }
}
