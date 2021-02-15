/*
 * Boot loader.
 *
 * Part of the boot block, along with bootasm.S, which calls bootmain().
 * bootasm.S has put the processor into protected 32-bit mode.
 * bootmain() loads an ELF kernel image from the disk starting at
 * sector 1 and then jumps to the kernel entry routine.
 */

#include <elf.h>
#include <fs.h>

void readseg(void *addr, u8 count, u32 offset);
void memclr(void *addr, u32 count);

void bootmain(void)
{
    struct elfhdr *elf;
    struct proghdr *ph, *eph;
    void (*entry)(void);
    u8 *pa;
    u32 offset, count;
    elf                 = (struct elfhdr *)0x8000; /* scratch space */
    struct superblock *sb = (struct superblock *)SUPER_ADDR; 

    readseg(sb, 1, SUPER_BLOCK);

    /* Read the first 8 sectors (4KB) of the kernel. */
    readseg(elf, 8, sb->kernel);

    /* Is this an ELF executable? */
    if (elf->magic != ELF_MAGIC)
        return; /* let bootasm.S handle error */

    /* Load each program segment (ignores ph flags). */
    ph  = (struct proghdr *)((u8 *)elf + elf->phoff);
    eph = ph + elf->phnum;
    for (; ph < eph; ph++) {
        pa     = (u8 *)ph->paddr - (ph->off % SECTSIZE); /* Round down to sector boundary. */
        count  = (ph->filesz + SECTSIZE - 1) / SECTSIZE;      /* how many read sectors */
        offset = (ph->off / SECTSIZE) + sb->kernel;        /* Translate from bytes to sectors. */
        readseg(pa, count, offset);
        if (ph->memsz > ph->filesz)
            memclr((u8 *)ph->paddr + ph->filesz, ph->memsz - ph->filesz);
    }

    /* Call the entry point from the ELF header. Does not return! */
    entry = (void (*)(void))(elf->entry);
    entry();
}
