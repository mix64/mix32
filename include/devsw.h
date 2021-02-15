#pragma once

#include <types.h>

#define NDEV 10
struct devsw {
    int (*read)(struct inode *, char *, u32);
    int (*write)(struct inode *, char *, u32);
} devsw[NDEV];

#define DEV_CONSOLE 1
