#include <arch.h>
#include <lock.h>
#include <disk.h>
#include <string.h>
#include <cpu.h>
#include <intr.h>

#define NCACHE 128
struct disk_cache disk_cache[NCACHE];
Lock disk_lock;

struct disk_cache *__disk_cache_alloc();

// return
//      success : cache address
//      failure : -1
// if dst==0, lock disk cache
struct disk_cache *disk_read(u32 blockno, void *dst, u32 size)
{
    struct disk_cache *dc;
    if (size > SECTSIZE) {
        return 0;
    }
    // get cache
    for (dc = disk_cache; dc < &disk_cache[NCACHE]; dc++) {
        if (dc->blockno == blockno && (dc->flags & CACHE_VALID)) {
            memmove(dc->data, dst, size);
            dc->last_access = cpu.tick;
            return 0;
        }
    }
    acquire(&disk_lock);
    // allocate free cache
    dc = __disk_cache_alloc(blockno);
    // read from disk
    arch_disk_read(blockno, dc->data);
    // copy cache data to *dst
    if (dst) {
        memmove(dc->data, dst, size);
    }
    else {
        dc->flags &= CACHE_USED;
    }
    // setup cache and return 
    dc->flags &= CACHE_VALID;
    dc->last_access = cpu.tick;
    release(&disk_lock);
    return dc;
}

// return
//      success : 0
//      failure : -1
int disk_write(u32 blockno, void *src, u32 size)
{
    struct disk_cache *dc;
    if (size > SECTSIZE || src == 0) {
        return -1;
    }
    acquire(&disk_lock);
    // update cache
    for (dc = disk_cache; dc < &disk_cache[NCACHE]; dc++) {
        if (dc->blockno == blockno && (dc->flags & CACHE_VALID)) {
            memset(dc->data, 0, SECTSIZE);
            memmove(src, dc->data, size);
            dc->last_access = cpu.tick;
        }
    }
    // write to disk
    arch_disk_write(blockno, src, size);
    release(&disk_lock);
    return 0;
}

void disk_cache_free(struct disk_cache *dc)
{
    dc->flags &= ~CACHE_USED;
}

void disk_intr()
{
    arch_disk_intr();
}

// if using this function, it need to lock "disk_lock"
struct disk_cache *__disk_cache_alloc(u32 blockno)
{
    struct disk_cache *dc;
    // get unused cache
    for (dc = disk_cache; dc < &disk_cache[NCACHE]; dc++) {
        if (dc->flags == 0) {
            goto out;
        }
    }
    // if no unused cache, use cache which oldest last access.
    dc = &disk_cache[0];
    for (int i = 0; i < NCACHE; i++) {
        if ((disk_cache[i].flags & CACHE_USED) == 0 && dc->last_access > disk_cache[i].last_access) {
            dc = &disk_cache[i];
        }
    }
out:
    memset(dc, 0, sizeof(*dc));
    dc->blockno = blockno;
    return dc;
}
