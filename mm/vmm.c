#include <fs.h>
#include <console.h>
#include <macro.h>
#include <mem.h>
#include <proc.h>
#include <string.h>
#include <arch.h>
#include <inode.h>

u32 *setup_kvm()
{
    u32 *pgdir = (u32*)pmalloc(PG_W | PG_R);
    for (int i = 0; i < NELEM(kvm); i++) {
        arch_set_pte(&pgdir[i], (u32)kvm[i]);
    }
    return pgdir;
}

// load user program after alloc_uvm()
int load_uvm(u32 *pgdir, u32 va, struct inode *ip, u32 offset, u32 size)
{
    u32 n, *pte;
    u32 pa;
    if (pgdir == 0) {
        panic("setup_uvm: no pgdir");
    }
    if ((u32)va % PGSIZE != 0) {
        panic("setup_uvm: va is not aligned.");
    }
    // setup user stack
    pte = arch_get_pte(pgdir, MEM_U_STACK, 1);
    pa = (u32)pmalloc(PG_U | PG_W | PG_R);
    arch_set_pte(pte, pa);
    // setup user code
    for (int i = 0; i < size; i += PGSIZE) {
        pa = arch_v2p(pgdir, va + i);
        if (pa == 0) {
            panic("setup_uvm: v2p");
        }
        n = (size - i < PGSIZE) ? size - i : PGSIZE;
        if (readi(ip, (void*)pa, offset + i, n) != n) {
            return -1;
        }
    }
    return 0;
}

void free_uvm(u32 *pgdir, u32 oldsz)
{
    if (pgdir == 0) {
        panic("free_uvm: no pgdir");
    }
    dealloc_uvm(pgdir, oldsz, 0);
    pmfree((u32)pgdir);
}

int alloc_uvm(u32 *pgdir, u32 oldsz, u32 newsz)
{
    u32 pa;
    u32 *pte;
    if (P2V(newsz) > MEM_U_END) {
        return 0;
    }
    if (newsz < oldsz) {
        return 0;
    }
    // allocate pages
    // UP(0x1234) to 0x1678, 0x2000 to 0x1678, no alloc
    // UP(0x1234) to 0x5678, 0x2000 to 0x5678, alloc pde[2:5]
    //    UP(0x0) to 0x5678,    0x0 to 0x5678, alloc pde[0:5]
    for (u32 va = P2V(PGROUNDUP(oldsz)); va < P2V(newsz); va += PGSIZE) {
        pte = arch_get_pte(pgdir, va, 1);
        pa = pmalloc(PG_U | PG_X | PG_W | PG_R);
        memset((void *)pa, 0, PGSIZE);
        arch_set_pte(pte, pa);
    }
    return newsz;
}

int dealloc_uvm(u32 *pgdir, u32 oldsz, u32 newsz)
{
    u32 *pte;
    u32 pa;
    if (newsz >= oldsz) {
        return 0;
    }
    // free pte
    for (u32 va = P2V(PGROUNDUP(newsz)); va < P2V(oldsz); va += PGSIZE) {
        pte = arch_get_pte(pgdir, va, 0);
        pa = arch_v2p(pgdir, va);
        if (pa == 0) { 
            // if pa is 0, then pte is also 0
            panic("dealloc_uvm");
        }
        *pte = 0;
        pmfree(pa);
    }
    // free pde
    // DOWN(0x5678) to UP(0x1234) => 0x5000 to 0x2000 => delete pde[2:5]
    // DOWN(0x1234) to UP(0)      => 0x1000 to 0 => delete pde[0:1]
    for (int i = PDX(PGROUNDDOWN(oldsz)); i >= PDX(PGROUNDUP(newsz)); i--) {
        pgdir[PDX(MEM_U_START) + i] = 0;
    }
    return newsz;
}

/*
    Copy data from [src, +len] to another pgdir's [va, +len].
    If zero specified in [src], we fill [va, +len] zeros with memset().
*/
void copy_out(u32 *pgdir, u32 va, void *src, u32 len)
{
    u32 n, offset, pa;
    while (len > 0) {
        pa = arch_v2p(pgdir, va);
        if (pa == 0) {
            panic("copy_uvm");
        }
        // copy self.[0x4020, 0x6000] -> another.[0x8020, 0xA000]
        // src = 0x4020, len = 0x1FE0, va = 0x8020
        // offset = 0x20, n = 0xFE0
        // [memmove]
        // next_src = 0x5000, next_len = 0x1000, next_va = 0x9000
        // next_offset = 0, next_n = 0x1000
        offset = va - PGROUNDDOWN(va);
        n      = PGSIZE - offset;
        if (n > len) {
            n = len;
        }
        if (src) {
            memmove(src, (void*)(pa + offset), n);
            src = (void *)((u32)src + n);
        }
        else {
            // memclr
            memset((void*)(pa + offset), 0, n);
        }
        len -= n;
        va += n;
    }
}