#include <mem.h>
#include <macro.h>
#include <arch.h>
#include <proc.h>
#include <init.h>
#include <console.h>
#include <file.h>
#include <inode.h>
#include <devsw.h>
#include <fs.h>
#include <string.h>

void pminit()
{
    u32 pa;
    // setup kernel space (RD-only)
    for (pa = 0; pa < (u32)rodata_end; pa += PGSIZE) {
        coremap[PGX(pa)].flags = (PG_V | PG_G | PG_X | PG_R);
    }
    // setup kernel space (RW)
    for (; pa < (u32)kernel_end; pa += PGSIZE) {
        coremap[PGX(pa)].flags = (PG_V | PG_G | PG_W | PG_R);
    }
}

void vminit()
{
    for (int i = 0; i < NELEM(kvm); i++) {
        kvm[i] = (u32*)pmalloc(PG_R | PG_W | PG_X | PG_G);
        for (int j = 0; j < NELEM(kvm); j++) {
            u32 pa = (i << 22) + (j << 12);
            arch_set_pte(&kvm[i][j], pa);
        }
    }
}

void procinit()
{
    struct proc *p = palloc();
    p->pgdir = setup_kvm();
    // initcode space
    u32 pa = pmalloc(PG_U | PG_X | PG_R);
    if (pa == 0) {
        panic("init_uvm");
    }
    u32 *pte = arch_get_pte(p->pgdir, MEM_U_START, 1);
    arch_set_pte(pte, pa);
    return arch_copy_initcode(pa);
}

void device_init()
{
    console_init();
    arch_timer_init();
}

void trap_init()
{
    arch_trap_init();
}

void fsinit()
{
    struct inode *ip;
    // setup stdin&stdout
    ip = ialloc();
    ip->devno = DEV_CONSOLE;
    ip->type = INODE_T_DEV;
    file[STDIN].refcnt = 1;
    file[STDIN].inode = ip;
    file[STDOUT].refcnt = 1;
    file[STDOUT].inode = ip;
    memmove((void *)SUPER_ADDR, &sb, sizeof(sb));
}