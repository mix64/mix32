#include <arch.h>
#include <x86/asm.h>
#include <proc.h>
#include <fs.h>
#include <string.h>

#define IDE_DATA 0x1F0
#define IDE_SECTCNT 0x1F2
#define IDE_ADDR1 0x1F3
#define IDE_ADDR2 0x1F4
#define IDE_ADDR3 0x1F5
#define IDE_ADDR4 0x1F6
#define IDE_COMMAND 0x1F7
#define IDE_RDCMD 0x20
#define IDE_WRCMD 0x30

void __wait_disk_ready();
void __disk_sendreq(u32 blockno);
u32 __disk_wchan;
char __disk_tmp[SECTSIZE];

void arch_disk_read(u32 blockno, void *dst)
{
    __wait_disk_ready();
    __disk_sendreq(blockno);
    outb(IDE_COMMAND, IDE_RDCMD);
    sleep(&__disk_wchan);
    insl(IDE_DATA, dst, SECTSIZE/4);
}

void arch_disk_write(u32 blockno, void *src, u32 size)
{
    __wait_disk_ready();
    __disk_sendreq(blockno);
    outb(IDE_COMMAND, IDE_WRCMD);
    memmove(src, __disk_tmp, size);
    outsl(IDE_DATA, __disk_tmp, SECTSIZE/4);
    memset(__disk_tmp, 0, sizeof(__disk_tmp));
}

void arch_disk_intr()
{
    wakeup(&__disk_wchan);
}

void __wait_disk_ready()
{
    while (1) {
        if ((inb(IDE_COMMAND) & 0xC0) == 0x40) {
            break;
        }
    }
}

void __disk_sendreq(u32 blockno)
{
    u32 n = (blockno & 0x0FFFFFFF);
    outb(IDE_SECTCNT, 1);
    outb(IDE_ADDR1, n & 0xFF);
    outb(IDE_ADDR2, (n >> 8) & 0xFF);
    outb(IDE_ADDR3, (n >> 16) & 0xFF);
    outb(IDE_ADDR4, (n >> 24) | 0xE0);
}