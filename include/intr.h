#pragma once

void enable_intr();
void disable_intr();
void timer_intr();

// console.c
void console_intr();

// disk.c
void disk_intr();