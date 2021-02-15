#include <console.h>
#include <string.h>
#include <arch.h>

#define SCREEN_X 80 // console width
#define SCREEN_Y 25 // console height
#define CGA_ADDR 0xB8000 // CGA memory

static int cursor;
static u16 *screen = (u16*)CGA_ADDR; // base address

void arch_putchar(int c)
{
    switch(c) {
        case '\n':
            cursor += SCREEN_X - (cursor % SCREEN_X);
            break;
        case '\b':
            break;
        default:
            screen[cursor++] = 0x0A00 | (0xFF & c);
            if (cursor < 0 || cursor > SCREEN_X * SCREEN_Y) {
                panic("cursor overflow");
            }
            if (cursor == SCREEN_X * (SCREEN_Y - 1)) { // scroll up
                memmove(&screen[SCREEN_X], screen, sizeof(screen[0]) * (SCREEN_Y - 2) * SCREEN_X);
                cursor -= SCREEN_X;
                memset(&screen[cursor], 0, sizeof(screen[0]) * SCREEN_X);
            }
    }
}

void arch_console_init()
{
    cursor = 0;
    memset(screen, 0, SCREEN_X * SCREEN_Y * 2);
}