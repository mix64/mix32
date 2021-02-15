#pragma once

#include <types.h>

extern char kernel_end[];
extern char rodata_end[];

#define MEM_SIZE 0x20000000

#define PGSIZE 0x1000
#define PGNENT 1024
#define COREMAP_SIZE (MEM_SIZE / PGSIZE)

#define MEM_K_START 0x100000
#define MEM_U_START 0x40000000
#define MEM_U_END 0xFFC00000
#define MEM_U_STACK (MEM_U_END - PGSIZE)

#define PGROUNDUP(sz) (((sz) + PGSIZE - 1) & ~(PGSIZE - 1)) /* round up to page size */
#define PGROUNDDOWN(a) (((a)) & ~(PGSIZE - 1))              /* round down to page size */
#define PGX(pa) ((u32)pa >> 12)
#define XPG(index) ((u32)(index << 12))
#define PDX(pa) ((u32)pa >> 22)
#define XPD(index) ((u32)(index << 22))
#define P2V(sz) (sz + MEM_U_START)

#define PG_V 0x001 // valid
#define PG_R 0x002
#define PG_W 0x004
#define PG_X 0x008
#define PG_U 0x010 // user
#define PG_G 0x020 // global

struct page {
    u32 flags;
} coremap[COREMAP_SIZE];

// Manage all memory from kernel, memory size must be less than kernel space.
// kvm must be aligned in 4KB, it allocated by pmalloc()
u32 *kvm[PDX(MEM_SIZE)];

// pmm.c
u32 pmalloc(u32 flags);
void pmfree(u32);

// vmm.c
struct inode;
u32 *setup_kvm();
int load_uvm(u32 *pgdir, u32 va, struct inode *ip, u32 offset, u32 size);
void free_uvm(u32 *pgdir, u32 oldsz);
int alloc_uvm(u32 *pgdir, u32 oldsz, u32 newsz);
int dealloc_uvm(u32 *pgdir, u32 oldsz, u32 newsz);
void copy_out(u32 *pgdir, u32 va, void *src, u32 len);
