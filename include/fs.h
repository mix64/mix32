#pragma once

#include <types.h>

#define SECTSIZE 512
#define DISKSIZE 0x400000 // 4MB
#define NSECTOR (DISKSIZE / SECTSIZE)

struct superblock {
    u32 size;       // disk size
    u16 mgr;        // free block manager
    u16 mgr_size;
    u32 kernel;    // kernel block
    u32 rootdir;   // rootdir block
} sb;

#define BOOT_BLOCK 0
#define SUPER_BLOCK 1
#define SUPER_ADDR 0x10000