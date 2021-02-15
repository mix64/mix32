#include <cpu.h>
#include <console.h>
#include <mem.h>
#include <proc.h>
#include <string.h>
#include <intr.h>
#include <file.h>
#include <arch.h>
#include <macro.h>
#include <inode.h>

struct proc *palloc()
{
    struct proc *p;
    disable_intr();
    // find process entry
    for (p = proc; p < &proc[NPROC]; p++) {
        if (p->stat == UNUSED) {
            memset(p, 0, sizeof(*p));
            break;
        }
    }
    if (p == &proc[NPROC]) {
        // process table overflow
        panic("palloc");
    }
    p->stat = SET;
    p->pid  = cpu.nextpid++;
    enable_intr();
    // setup kstack
    p->kstack = pmalloc(PG_R | PG_W);
    memset((void*)p->kstack, 0, PGSIZE);
    // set trapframe & context
    arch_setup_proc(p);
    p->ofile[STDIN] = fget(STDIN);
    p->ofile[STDOUT] = fget(STDOUT);
    p->ustack = MEM_U_STACK; // allocate by setup_uvm()
    return p;
}

void pfree(struct proc *p)
{
    disable_intr();
    // close file
    for (int fd = 0; fd < NELEM(p->ofile); fd++) {
        if (p->ofile[fd]) {
            ffree(p->ofile[fd]);
            p->ofile[fd] = 0;
        }
    }
    if (p->cwd) {
        irelse(p->cwd);
    }
    if (p->pgdir) {
        free_uvm(p->pgdir, p->size);
    }
    if (p->kstack) {
        pmfree(p->kstack);
    }
    u32 ustack = arch_v2p(p->pgdir, MEM_U_STACK);
    if (ustack) {
        pmfree(ustack);
    }
    // in palloc(), struct proc is set to 0.
    p->stat = UNUSED;
    enable_intr();
}

void sleep(void *wchan)
{
    disable_intr();
    cpu.procp->wchan = wchan;
    cpu.procp->stat  = SLEEP;
    enable_intr();
    while (cpu.procp->wchan) {
        sched();
    }
}

void wakeup(void *wchan)
{
    struct proc *p;
    disable_intr();
    for (p = proc; p < &proc[NPROC]; p++) {
        if (p->wchan == wchan) {
            p->stat  = RUN;
            p->wchan = 0;
        }
    }
    enable_intr();
}

void sched()
{}