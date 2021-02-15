#pragma once

typedef unsigned int u32;
typedef unsigned short u16;
typedef unsigned char u8;

typedef struct {
    u32 locked;
    struct proc *procp; // process which holding lock
} Lock;