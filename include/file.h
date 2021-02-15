#pragma once

#include <types.h>

#define STDIN 0
#define STDOUT 1
#define STDERR 2

struct file {
    u32 refcnt;             /* reference count */
    struct inode *inode;    /* pointer to inode structure */
    u32 offset;             /* read/write character pointer */
} file[64];

struct file *fget(int index);
struct file *falloc();
void ffree(struct file *f);
struct file *fdup(struct file *f);