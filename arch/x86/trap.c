#include <x86/trap.h>
#include <x86/trapframe.h>
#include <arch.h>
#include <x86/gate.h>
#include <x86/segs.h>
#include <string.h>
#include <cpu.h>
#include <proc.h>
#include <intr.h>
#include <console.h>
#include <x86/asm.h>

struct gatedesc idt[256];
static u16 idtr[3];
extern u32 vectors[]; // in vectors.S: array of 256 entry pointers

void __setup_pic();
void __eoi_pic(u32 no); // send "end of interrupt" to PIC

void arch_trap_init()
{
    // setup IDT (Interrupt Descriptor Table)
    for (int i = 0; i < 256; i++) {
        SETGATE(idt[i], 0, SEG_KCODE, vectors[i], 0);
    }
    SETGATE(idt[T_SYSCALL], 1, SEG_KCODE, vectors[T_SYSCALL], DPL_USER);
    idtr[0] = sizeof(idt) - 1;
    idtr[1] = (u32)idt;
    idtr[2] = (u32)idt >> 16;
    asm volatile("lidt (%0)" ::"r"(idtr));
    // setup PIC (Programmable Interrupt Controller)
    __setup_pic();
}

/* 
system call is trap, so interrupts may happen even during processing.
but other interrupts are not traps, so there are no more interrupts during processing.
*/
void arch_trap(struct trapframe *tf)
{
    if (tf->trapno == T_SYSCALL) {
        memmove(tf, cpu.procp->tf, sizeof(struct trapframe));
        tf->eax = 0; // TODO: return syscall(tf->eax)
        return;
    }
    switch(tf->trapno) {
        case T_IRQ0 + IRQ_TIMER:
            timer_intr();
            __eoi_pic(IRQ_TIMER);
            break;
        case T_IRQ0 + IRQ_KBD:
            console_intr();
            __eoi_pic(IRQ_KBD);
            break;
        case T_IRQ0 + IRQ_IDE:
            disk_intr();
            __eoi_pic(IRQ_IDE);
            break;
        default:
            // See Intel SDM Vol.3A chap.6
            printf("unknown trap No.%d", tf->trapno);
            printf("EIP: %x\n", tf->eip);
            printf("ESP: %x\n", tf->esp);
            printf("EBP: %x\n", tf->ebp);
            printf("CR2: %x\n", lcr2());
            panic("unknown trap");
    }
}

/* Programmable Interrupt Controller (PIC) ports */
#define PIC0_CMD 0x20 /* cmd port */
#define PIC1_CMD 0xA0
#define PIC0_DATA 0x21 /* data port */
#define PIC1_DATA 0xA1
/* PIC Initialization Control Words */
#define PIC_ICW1 0x11        /* 0x1: read ICW4, 0x10: reserve, must 1*/
#define PIC0_ICW2 0x20       /* int vec[0x20=32] */
#define PIC1_ICW2 0x28       /* int vec[0x28=40] */
#define PIC0_ICW3 0x04       /* IR2 connect slave PIC */
#define PIC1_ICW3 0x02       /* connect master PIC IR2 */
#define PIC_ICW4 0x03        /* set in x86 mode */
#define PIC_EOI 0x20         /* end of interrupt */
#define PIC0_MASK 0b11110000 /* IRQ MASK without IR0(timer),IR1(kbd),IR2(slave) */
#define PIC1_MASK 0b10111111 /* MASK without IDE(14) */
#define PIC_MASKALL 0xFF

void __setup_pic()
{
    outb(PIC0_DATA, PIC_MASKALL);
    outb(PIC1_DATA, PIC_MASKALL);
    outb(PIC0_CMD, PIC_ICW1);
    outb(PIC1_CMD, PIC_ICW1);
    outb(PIC0_DATA, PIC0_ICW2);
    outb(PIC1_DATA, PIC1_ICW2);
    outb(PIC0_DATA, PIC0_ICW3);
    outb(PIC1_DATA, PIC1_ICW3);
    outb(PIC0_DATA, PIC_ICW4);
    outb(PIC1_DATA, PIC_ICW4);
    outb(PIC0_DATA, PIC0_MASK);
    outb(PIC1_DATA, PIC1_MASK);
}

void __eoi_pic(u32 no)
{
    if (no >= 8) {
        outb(PIC1_CMD, PIC_EOI);
    }
    outb(PIC0_CMD, PIC_EOI);
} 