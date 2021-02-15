#include <arch.h>
#include <x86/asm.h>

#define KBD_STAT 0x64 /* kbd controller status port(I) */
#define KBS_DIB 0x01  /* kbd data in buffer */
#define KBD_DATA 0x60 /* kbd data port(I) */

#define KBD_FLAG_SHIFT 0x01

static u8 flags = 0;

static const u8 keymap[64] = {
    0, 0x1B, '1', '2', '3', '4', '5', '6',
    '7', '8', '9', '0', '-', '=', '\b', '\t',
    'q', 'w', 'e', 'r', 't', 'y', 'u', 'i',
    'o', 'p', '[', ']', '\n', 0, 'a', 's',
    'd', 'f', 'g', 'h', 'j', 'k', 'l', ';',
    '\'', '`', 0, '\\', 'z', 'x', 'c', 'v',
    'b', 'n', 'm', ',', '.', '/', 0, '*',
    0, ' ', 0, 0, 0, 0, 0, 0};

static const u8 shiftmap[64] = {
    0, 033, '!', '@', '#', '$', '%', '^',
    '&', '*', '(', ')', '_', '+', '\b', '\t',
    'Q', 'W', 'E', 'R', 'T', 'Y', 'U', 'I',
    'O', 'P', '{', '}', '\n', 0, 'A', 'S',
    'D', 'F', 'G', 'H', 'J', 'K', 'L', ':',
    '"', '~', 0, '|', 'Z', 'X', 'C', 'V',
    'B', 'N', 'M', '<', '>', '?', 0, '*',
    0, ' ', 0, 0, 0, 0, 0, 0};

int arch_kbd_getc()
{
    u8 n = inb(KBD_DATA);
    int c = 0;

    switch (n) {
    case 0x2A:
    case 0x36:
        flags |= KBD_FLAG_SHIFT;
        break;

    case 0xAA:
    case 0xB6:
        flags &= ~KBD_FLAG_SHIFT;
        break;

    default:
        if (n < 0x40) {
            if (flags & KBD_FLAG_SHIFT) {
                c = shiftmap[n];
            }
            else {
                c = keymap[n];
            }
        }
        break;
    }
    return c;
}
