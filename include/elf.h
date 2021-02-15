#pragma once

#include <types.h>

#define ELF_MAGIC 0x464C457FU /* "\x7FELF" in little endian */

/* ELF File header */
struct elfhdr {
    u32 magic; /* must equal ELF_MAGIC */
    u8 elf[12];
    u16 type;
    u16 machine;
    u32 version;
    u32 entry;
    u32 phoff;
    u32 shoff;
    u32 flags;
    u16 ehsize;
    u16 phentsize;
    u16 phnum;
    u16 shentsize;
    u16 shnum;
    u16 shstrndx;
};

/* ELF Program section header */
struct proghdr {
    u32 type;
    u32 off;
    u32 vaddr;
    u32 paddr;
    u32 filesz;
    u32 memsz;
    u32 flags;
    u32 align;
};

// Values for Proghdr type
#define ELF_PROG_LOAD 1