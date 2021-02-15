#pragma once

#include <types.h>

#define NPROC 64

enum procstat { UNUSED,
                SET,
                RUN,
                SLEEP,
                ZOMB,
                STOP };

struct proc {
    enum procstat stat;      /* process state */
    u32 pri;                 /* priority, negative is high */
    u32 pid;                 /* unique process id */
    struct proc *pproc;      /* parent process */
    u32 uid;                 /* user id */
    u32 gid;                 /* group id */
    u32 cpu;                 /* cpu usage for scheduling */
    u32 size;                /* size of process */
    void *wchan;             /* waiting channel */
    u32 *pgdir;              /* page directory */
    u32 kstack;              /* kernel stack for this process (used by tf, context?) */
    u32 ustack;              /* user stack */
    struct trapframe *tf;    /* trap frame */
    struct context *context; /* context swtch */
    struct inode *cwd;       /* current directory */
    struct file *ofile[16];  /* open files */
} proc[NPROC];

struct proc *palloc();
void sleep(void *wchan);
void wakeup(void *wchan);
void sched();