#include <stddef.h>
#include <stdio.h>
#include <stdint.h>
#include <io.h>
#include "isr.h"
#include "idt.h"

ISRHandler g_ISRHandlers[256];              // 256 Fucking handlers (aka Memory wasting)

static const char* const g_Exceptions[] = {
    "Division by zero error",
    "Debug",
    "Non-maskable Interrupt",
    "Breakpoint",
    "Overflow",
    "Bound Range Exceeded",
    "Invalid Opcode",
    "Device Not Available",
    "Double Fault",
    "Coprocessor Segment Overrun",
    "Invalid TSS",
    "Segment Not Present",
    "Stack-Segment Fault",
    "General Protection Fault",
    "Page Fault",
    "Reserved",
    "x87 Floating-Point Exception",
    "Alignment Check",
    "Machine Check",
    "SIMD Floating-Point Exception",
    "Virtualization Exception",
    "Control Protection Exception ",
    "Reserved",
    "Reserved",
    "Reserved",
    "Reserved",
    "Reserved",
    "Reserved",
    "Hypervisor Injection Exception",
    "VMM Communication Exception",
    "Security Exception",
    "Reserved"
};

void ISR_Init() {
    x64_ISR_SetGates();
    for (int i = 0;i < 256;i++) {
        x64_IDT_EnableGate(i);
    }
	__asm__ ("sti");
}

void x64_ISR_Handler(Pushed_Regs* regs) {
    if(g_ISRHandlers[regs->interrupt] != NULL)
        g_ISRHandlers[regs->interrupt](regs);

    else if(regs->interrupt >= 32)
        printf("Fuck this Interrupt: %lld, Not handled!", regs->interrupt);

    else {
        printf("Fuck this Exeption: %lld Which means: %s\n", regs->interrupt, g_Exceptions[regs->interrupt]);

        printf("  rax=%llx  rbx=%llx  rcx=%llx  rdx=%llx  rsi=%llx  rdi=%llx\n",
               regs->rax, regs->rbx, regs->rcx, regs->rdx, regs->rsi, regs->rdi);

        printf("  rbp=%llx  rip=%llx  rflags=%llx  cs=%llx  ss=%llx\n",
               regs->rbp, regs->rip, regs->rflags, regs->cs, regs->ss);

        printf("  interrupt=%llx  errorcode=%llx\n", regs->interrupt, regs->error_code);

        printf("KERNEL PANIC!!!\n");
        panic();
    }
}

void x64_Set_Interrupt_Handler(int interrupt, ISRHandler handler) {
    g_ISRHandlers[interrupt] = handler;
    x64_IDT_EnableGate(interrupt);
}