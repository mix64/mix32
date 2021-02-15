#include <proc.h>
#include <arch.h>
#include <mem.h>
#include <x86/trapframe.h>
#include <x86/regs.h>
#include <x86/segs.h>

extern void trapret();

struct context {
    u32 edi;
    u32 esi;
    u32 ebx;
    u32 ebp;
    u32 eip;
};

void arch_setup_proc(struct proc *p)
{
    u32 sp = p->kstack + PGSIZE;
    sp -= sizeof(struct trapframe);
    p->tf = (struct trapframe *)sp;
    sp -= sizeof(struct context);
    p->context = (struct context *)sp;
    p->context->eip = (u32)trapret;
    p->tf->eflags   = EFLAGS_IF;
    p->tf->cs       = SEG_UCODE | DPL_USER;
    p->tf->ds       = SEG_UDATA | DPL_USER;
    p->tf->es       = p->tf->ds;
    p->tf->ss       = p->tf->ds;
    p->tf->esp      = MEM_U_STACK - PGSIZE;
}
