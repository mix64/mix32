#include <mem.h>
#include <x86/segs.h>
#include <x86/asm.h>
#include <x86/regs.h>
#include <proc.h>
#include <intr.h>
#include <string.h>
#include <console.h>
#include <arch.h>

#define PTE_P 0x001
#define PTE_W 0x002
#define PTE_U 0x004

#define PTE_ADDR(pte) ((u32)(pte) & ~0xFFF)

void arch_enable_mmu()
{
    scr0(CR0_PG);
}

void arch_switchuvm(struct proc *p)
{
    if (p == 0 || p->kstack == 0 || p->pgdir == 0) {
        panic("switchuvm");
    }
    disable_intr();
    gdt[SEG_TSS >> 3]   = SEG16(STS_T32A, &ts, sizeof(ts) - 1, 0);
    gdt[SEG_TSS >> 3].s = 0;
    ts.ss0              = SEG_KDATA;
    ts.esp0             = p->kstack + PGSIZE;
    ts.iomb             = 0xFFFF;
    asm volatile("ltr %0" ::"r"((u16)SEG_TSS));
    scr3(p->pgdir);
    enable_intr();
}

u32 *arch_get_pte(u32 *pgdir, u32 va, int is_alloc)
{
    if (pgdir == 0) {
        panic("get_pte");
    }
    u32 *pde = &pgdir[PDX(va)];
    u32 *pte;
    if (*pde & PTE_P) {
        // exist
        pte = (u32*)PTE_ADDR(*pde);
    }
    else if (is_alloc){
        // alloc
        if (va >= MEM_U_START) {
            pte = (u32*)pmalloc(PG_W | PG_R | PG_U);
        }
        else {
            pte = (u32*)pmalloc(PG_W | PG_R);
        }
        memset(pte, 0, PGSIZE);
        arch_set_pte(pde, (u32)pte);
    }
    else {
        // not exist & no alloc
        return 0;
    }
    return &pte[PGX(va)];
}

void arch_set_pte(u32 *pte, u32 pa)
{
    if (pte == 0) {
        panic("set_pte");
    }
    *pte = pa | PTE_P;
    // set page flags
    if (coremap[PGX(pa)].flags & PG_W) {
        *pte |= PTE_W;
    }
    if (coremap[PGX(pa)].flags & PG_U) {
        *pte |= PTE_U;
    }
}

u32 arch_v2p(u32 *pgdir, u32 va)
{
    u32 *pte = arch_get_pte(pgdir, va, 0);
    if (pte && *pte & PTE_P) {
        return PTE_ADDR(*pte);
    }
    else {
        return 0;
    }
}
