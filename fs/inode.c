#include <inode.h>
#include <intr.h>
#include <macro.h>
#include <disk.h>
#include <string.h>
#include <lock.h>
#include <devsw.h>
#include <cpu.h>
#include <proc.h>

struct inode inode[64];
struct inode *__dirlookup(struct inode *ip, char *fname);

struct inode *ialloc()
{
    struct inode *ip = 0;
    disable_intr();
    for (int i = 0; i < NELEM(inode); i++) {
        if (inode[i].valid == 0) {
            ip = &inode[i];
            ip->valid = 1;
            ip->refcnt = 1;
            break;
        }
    }
    enable_intr();
    return ip;
}

struct inode *iget(u32 ino)
{
    struct inode *ip = 0;
    disable_intr();
    // find inode cache
    for (int i = 0; i < NELEM(inode); i++) {
        if (inode[i].valid && inode[i].num == ino) {
            ip = &inode[i];
            ip->refcnt++;
            goto out;
        }
    }
    // if no cache, read from disk
    if ((ip = ialloc()) == 0) {
        goto out;
    }
    ip->num = ino;
    ip->devno = 0;
    if (disk_read(ino, &ip->type, SECTSIZE) == -1) {
        irelse(ip);
        ip = 0;
    }
out:
    enable_intr();
    return ip;
}

void irelse(struct inode *ip)
{
    if (ip == 0) {
        panic("irelse");
    }
    disable_intr();
    if (ip->refcnt > 1) {
        ip->refcnt--;
    }
    else {
        // clear inode cache
        memset(ip, 0, sizeof(*ip));
    }
    enable_intr();
}

struct inode *idup(struct inode *ip)
{
    if (ip == 0) {
        panic("idup");
    }
    disable_intr();
    ip->refcnt++;
    enable_intr();
    return ip;
}

// to call this function, ip->lock must be holding
int readi(struct inode *ip, char *dst, u32 offset, u32 size)
{
    u32 m;
    if (!holding(&ip->lock)) {
        return -1;
    }
    // if inode is devices (ex. console)
    if (ip->type == INODE_T_DEV) {
        if (ip->devno < 0 || ip->devno >= NDEV || !devsw[ip->devno].read) {
            return -1;
        }
        return devsw[ip->devno].read(ip, dst, size);
    }
    // check offset is valid, not overflow, and not beyond file maxsize (512*124)
    if (offset > ip->size || (offset + size) < offset || (offset/SECTSIZE) >= 124) {
        return -1;
    }
    if (offset + size > ip->size) {
        size = ip->size - offset;
    }
    for (int cnt = 0; cnt < size; cnt += m, offset += m, dst += m) {
        // size:1000, offset:100
        // 1st - min(1000-0, 512-100%512) = 412, read[100:100+412]
        // 2nd - min(1000-412, 512-512%512) = 512, read[512:1024]
        // 3rd - min(1000-924, 512-1024%512) = 76, read[1024:1100]
        m = min(size - cnt, SECTSIZE - offset % SECTSIZE);
        if (disk_read(ip->addr[offset/SECTSIZE], dst, m) == -1) {
            return -1;
        }
    }
    return size;
}

// to call this function, ip->lock must be holding
int writei(struct inode *ip, char *src, u32 offset, u32 size)
{
    u32 m;
    char _cache[SECTSIZE];
    if (!holding(&ip->lock)) {
        return -1;
    }
    // if inode is devices (ex. console)
    if (ip->type == INODE_T_DEV) {
        if (ip->devno < 0 || ip->devno >= NDEV || !devsw[ip->devno].write) {
            return -1;
        }
        return devsw[ip->devno].write(ip, dst, size);
    }
    // check offset is valid, not overflow, and not beyond file maxsize (512*124), and result not beyond file maxsize
    if (offset > ip->size || (offset + size) < offset || (offset/SECTSIZE) >= 124 || (offset+size) > (124*SECTSIZE)) {
        return -1;
    }
    for (int cnt = 0; cnt < size; cnt += m, offset += m, src += m) {
        // size:1000, offset:100
        // 1st - min(1000-0, 512-100%512) = 412, read[0:512] -> write[100:100+412]
        // 2nd - min(1000-412, 512-512%512) = 512, write[512:1024]
        // 3rd - min(1000-924, 512-1024%512) = 76, read[1024:1536] -> write[1024:1100]
        m = min(size - cnt, SECTSIZE - offset % SECTSIZE);
        if (m == SECTSIZE) {
            // rewrite entire of sector
            if (disk_write(ip->addr[offset/SECTSIZE], src, m) == -1) {
                return -1;
            }
        }
        else {
            // rewrite a part of sector, read sector first
            if (disk_read(ip->addr[offset/SECTSIZE], _cache, SECTSIZE) == -1) {
                return -1;
            }
            memmove(src, _cache[offset%SECTSIZE], m);
            if (disk_write(ip->addr[offset/SECTSIZE], _cache, SECTSIZE) == -1) {
                return -1;
            }
        }
    }
}

struct inode *namei(char *path)
{
    struct inode *ip;
    char fname[DIR_FNAMESIZE];
    if (path == 0) {
        return 0;
    }
    if (*path == '/') {
        // if name starts with '/' start from root.
        ip = get_rootdir();
        path++;
    }
    else {
        // otherwise start from current dir.
        ip = idup(cpu.procp->cwd)
    }
    // save path length
    int n = strlen(path);
    char *index = path;
    // 1. path="home/user/mix32/kernel", index is same
    // 2. path="home\0user/mix32/kernel", index is same, dirlookup("home")
    // 3. path="home\0user\0mix32/kernel", index="user\0mix32/kernel", dirlookup("user")
    // 4. path="home\0user\0mix32\0kernel", index="mix32\0kernel", dirlookup("mix32")
    for (int i = 0; i < n; i++) {
        if (path[i] == '/') {
            path[i] = '\0';
            ip = __dirlookup(ip, index);
            // no entry
            if (ip == 0) {
                return 0;
            }
            // update index
            index = &path[i+1];
        }
    }
    // 5. index="kernel", dirlookup("kernel")
    ip = __dirlookup(ip, index);
    return ip;
}

struct inode *__dirlookup(struct inode *ip, char *fname)
{
    struct disk_cache *dc = 0;
    struct directory *d;
    struct inode *newip = 0;
    for (int i = 0; i < NELEM(ip->addr); i++) {
        if (ip->addr[i] != 0) {
            dc = disk_read(ip->addr[i], 0, SECTSIZE);
            d = (struct directory *)dc->data;
            // find entry from directroy
            for (int j = 0; j < DIR_NENT; j++) {
                if (strcmp(d[j].dir->fname, fname, DIR_FNAMESIZE)) {
                    // read inode
                    newip = iget(d[j].dir->ino);
                    disk_cache_free(dc);
                    goto out;
                }
            }
            disk_cache_free(dc);
        }
    }
out:
    irelse(ip);
    return newip;
}

struct inode *get_rootdir()
{
    return iget(sb.rootdir);
}