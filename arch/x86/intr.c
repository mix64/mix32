#include <arch.h>
#include <x86/asm.h>
#include <x86/regs.h>
#include <cpu.h>
#include <console.h>

void arch_disable_intr()
{
    int eflags = leflags();
    cli();
    if (cpu.intr_depth == 0) {
        cpu.intr_enable = (eflags & EFLAGS_IF);
    }
    cpu.intr_depth++;
}

void arch_enable_intr()
{
    cpu.intr_depth--;
    if (leflags() & EFLAGS_IF) {
        panic("arch_enable_intr: 1");
    }
    if (cpu.intr_depth < 0) {
        panic("arch_enable_intr: 2");
    }
    if (cpu.intr_depth == 0 && cpu.intr_enable) {
        cpu.intr_enable = 0;
        sti();
    }
}
