#include <arch.h>
#include <console.h>
#include <intr.h>
#include <proc.h>
#include <macro.h>
#include <devsw.h>
#include <init.h>

char __buf[128];
u32 __index;

void __printn(u32 n, u32 base);

void printf(char *fmt, ...)
{
    u32 *argv = (u32*)(&fmt + 1);
    char *s;
    int c;
    while((c = *fmt++) != 0) {
        // not format specifiers
        if (c != '%') {
            arch_putchar(c);
            continue;
        }
        // if format specifiers, get type
        switch(c = *fmt++) {
            case 'd':
                __printn(*argv++, 10);
                break;
            case 'x':
            case 'p':
                printf("0x");
                __printn(*argv++, 16);
                break;
            case 'o':
                printf("0o");
                __printn(*argv++, 8);
                break;
            case 'c':
                arch_putchar(*argv++);
                break;
            case 's':
                if ((s = (char*)*argv++) == 0) {
                    printf("(null)");
                }
                else {
                    while ((c = *s++)) {
                        arch_putchar(c);
                    }
                }
                break;
            default:
                printf("Unknown format %c", c);
                return;
        }
    }
}

void panic(char *str)
{
    disable_intr();
    printf("panic: %s\n", str);
    while(1);
}

void __printn(u32 n, u32 base)
{
    static char digits[] = "0123456789ABCDEF";
    u32 m;
    if ((m = n / base)) {
        __printn(m, base);
    }
    arch_putchar(digits[(n % base)]);
}

void console_intr()
{
    int c = arch_kbd_getc();
    if (__index >= sizeof(__buf) - 1  || c == 0) {
        return;
    }
    switch (c) {
        case '\n':
            // wake up a process which waiting for input
            wakeup(__buf);
            break;
        case '\b':
            __index--;
            break;
        default:
            __buf[__index++] = c;
            break;
    }
    arch_putchar(c);
}

// if filetype == CONSOLE, use this read/write
int console_read(struct inode *ip, char *dst, u32 size)
{
    sleep(__buf); // wakeup in console_intr()
    disable_intr();
    int n = min(size, __index);
    for (int i = 0; i < n; i++) {
        *dst++ = __buf[i];
    }
    *dst = '\0';
    __index = 0;
    enable_intr();
    return n;
}

int console_write(struct inode *ip, char *src, u32 size)
{
    disable_intr();
    for (int i = 0; i < size; i++) {
        arch_putchar(src[i]);
    }
    enable_intr();
    return size;
}

void console_init()
{
    devsw[DEV_CONSOLE].read = console_read;
    devsw[DEV_CONSOLE].write = console_write;
    arch_console_init();
}