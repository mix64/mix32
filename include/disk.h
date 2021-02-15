#pragma once

#include <types.h>
#include <fs.h>

struct disk_cache {
    u32 blockno;
    u32 flags;
    u32 last_access;
    u8 data[SECTSIZE];
};

// disk_cache flags
#define CACHE_USED  0x1
#define CACHE_VALID 0x2
#define CACHE_DIRTY 0x4

struct disk_cache *disk_read(u32 blockno, void *dst, u32 size);
int disk_write(u32 blockno, void *src, u32 size);
void disk_cache_free(struct disk_cache *dc);