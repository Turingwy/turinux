#include "idt.h"
#include "types.h"
#include "idt.h"
#include "stdio.h"
idt_entry_t idt[256];

idt_ptr_t idt_ptr;

interrupt_handler_t handlers[256];

void isr_handler(regs_pt *regs) 
{
    if(handlers[regs->int_no] == NULL) 
    {
        printk("Catched unhandled interrupt %x\n", regs->int_no);
        return;
    }
    handlers[regs->int_no](regs);
}

void irq_handler(regs_pt *regs) 
{
    if(regs->int_no >= 40)
    {
        outb(0xA0, 0x20);
    }
    outb(0x20, 0x20);

    if(handlers[regs->int_no])
        handlers[regs->int_no](regs);
    else
        printk("Catched unhandled interrupt %x\n", regs->int_no);

}

void register_handler(int num, interrupt_handler_t func) {
    handlers[num] = func;
}
static void idt_set_gate(int num, uint32_t addr, uint16_t sele, uint8_t flags) 
{
    idt[num].offset_low = addr & 0xFFFF;
    idt[num].offset_high = (addr >> 16) & 0xFFFF;
    idt[num].flags = flags << 8;
    idt[num].selector = sele;
}

static inline void set_trap_gate(int num, uint32_t addr, uint16_t sele) {
    idt_set_gate(num, addr, sele, TRAP_FLAGS);
}

static inline void set_intr_gate(int num, uint32_t addr, uint16_t sele) {
    idt_set_gate(num, addr, sele, INTR_FLAGS);
}

static void init_8259APIC() {
    outb(0x20, 0x11);
    outb(0xA0, 0x11);
    outb(0x21, 0x20);
    outb(0xA1, 0x28);
    outb(0x21, 0x04);
    outb(0xA1, 0x02);
    outb(0x21, 0x01);
    outb(0xA1, 0x01);
    outb(0x21, 0x0);
    outb(0xA1, 0x0);
}



void init_idt()
{
    init_8259APIC();
    set_trap_gate( 0, (uint32_t)isr0,  0x08);
	set_trap_gate( 1, (uint32_t)isr1,  0x08);
	set_trap_gate( 2, (uint32_t)isr2,  0x08);
	set_trap_gate( 3, (uint32_t)isr3,  0x08);
	set_trap_gate( 4, (uint32_t)isr4,  0x08);
    set_trap_gate( 5, (uint32_t)isr5,  0x08);
    set_trap_gate( 6, (uint32_t)isr6,  0x08);
    set_trap_gate( 7, (uint32_t)isr7,  0x08);
    set_trap_gate( 8, (uint32_t)isr8,  0x08);
    set_trap_gate( 9, (uint32_t)isr9,  0x08);
    set_trap_gate(10, (uint32_t)isr10, 0x08);
    set_trap_gate(11, (uint32_t)isr11, 0x08);
    set_trap_gate(12, (uint32_t)isr12, 0x08);
    set_trap_gate(13, (uint32_t)isr13, 0x08);
    set_trap_gate(14, (uint32_t)isr14, 0x08);
    set_trap_gate(15, (uint32_t)isr15, 0x08);
    set_trap_gate(16, (uint32_t)isr16, 0x08);
    set_trap_gate(17, (uint32_t)isr17, 0x08);
    set_trap_gate(18, (uint32_t)isr18, 0x08);
    set_trap_gate(19, (uint32_t)isr19, 0x08);
    set_trap_gate(20, (uint32_t)isr20, 0x08);
    set_trap_gate(21, (uint32_t)isr21, 0x08);
    set_trap_gate(22, (uint32_t)isr22, 0x08);
    set_trap_gate(23, (uint32_t)isr23, 0x08);
    set_trap_gate(24, (uint32_t)isr24, 0x08);
    set_trap_gate(25, (uint32_t)isr25, 0x08);
    set_trap_gate(26, (uint32_t)isr26, 0x08);
    set_trap_gate(27, (uint32_t)isr27, 0x08);
    set_trap_gate(28, (uint32_t)isr28, 0x08);
    set_trap_gate(29, (uint32_t)isr29, 0x08);
    set_trap_gate(30, (uint32_t)isr30, 0x08);
    set_trap_gate(31, (uint32_t)isr31, 0x08);
    set_intr_gate(32, (uint32_t)irq0, 0x08);
    set_intr_gate(33, (uint32_t)irq1, 0x08);
    idt_ptr.limit = sizeof(idt_entry_t) * 256 - 1;
    idt_ptr.base = (uint32_t)idt;
    idt_flush((uint32_t)&idt_ptr);
}

