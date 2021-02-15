#include <console.h>
#include <mem.h>
#include <string.h>
#include <intr.h>

u32 pmalloc(u32 flags)
{
    disable_intr();
    for (int i = 0; i < COREMAP_SIZE; i++) {
        if (coremap[i].flags == 0) {
            coremap[i].flags = (PG_V | flags);
            enable_intr();
            return XPG(i);
        }
    }
    panic("pmalloc");
    return 0;
}

void pmfree(u32 pa)
{
    if (pa % PGSIZE || pa < (u32)kernel_end || pa >= MEM_SIZE) {
        panic("pmfree");
    }
    // remove data in page for next use
    memset((u32*)pa, 0, PGSIZE);
    disable_intr();
    memset(&coremap[PGX(pa)], 0, sizeof(struct page));
    enable_intr();
}
