#pragma once

#include <types.h>

// Architecture-dependent functions

// intr.c
void arch_disable_intr();
void arch_enable_intr();

// mm.c
void arch_set_pte(u32 *pte, u32 pa);
u32 *arch_get_pte(u32 *pgdir, u32 va, int is_alloc);
u32 arch_v2p(u32 *pgdir, u32 va);

// initcode.c
void arch_copy_initcode(u32 pa);

// proc.c
void arch_setup_proc(struct proc *p);

// console.c
void arch_putchar(int c);
void arch_console_init();

// trap.c
void arch_trap_init();

// kbd.c
int arch_kbd_getc();

// disk.c
void arch_disk_read(u32 blockno, void *dst);
void arch_disk_write(u32 blockno, void *src, u32 size);
void arch_disk_intr();

// timer.c
void arch_timer_init();