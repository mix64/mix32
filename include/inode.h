#pragma once

#include <types.h>

// inode.type
#define INODE_T_DIR 1
#define INODE_T_FILE 2
#define INODE_T_DEV 3

struct inode {
    u32 num;
    u16 refcnt;
    u16 valid;
    Lock lock;
    u32 devno;

    // copy from disk
    u16 type;
    u16 mode;
    u16 uid;
    u16 gid;
    u32 nlink;
    u32 size;
    u32 addr[124];
};

#define DIR_FNAMESIZE 28
#define DIR_NENT 16

// sizeof(struct directory) == 512 == SECTSIZE
struct directory {
    struct dir {
        u32 ino;
        char fname[DIR_FNAMESIZE];
    } dir[DIR_NENT];
};

struct inode *ialloc();
struct inode *iget(u32 ino);
void irelse(struct inode *ip);
struct inode *idup(struct inode *ip);
int readi(struct inode *ip, void *dst, u32 offset, u32 size);
int writei(struct inode *ip, void *src, u32 offset, u32 size);
struct inode *namei(char *path);
struct inode *get_rootdir();
