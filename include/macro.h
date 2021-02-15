#pragma once

#define min(a, b) ((a) < (b) ? (a) : (b))
#define max(a, b) ((a) > (b) ? (a) : (b))

/* number of elements in fixed-size array */
#define NELEM(x) (sizeof(x) / sizeof((x)[0]))
#define INDEX(ADDR, BASE, SIZE) (((u8)(ADDR) - (u8)(BASE)) / (SIZE))