#include "types.h"
#include "vmm.h"
#include "idt.h"
#include "stdio.h"

void page_fault(regs_pt *regs)
{
    uint32_t cr2;
    asm volatile("mov %%cr2, %0" : "=r" (cr2));

    printk("Page fault at %x, virtual address at %x.\n", regs->eip, cr2);
    printk("Error code: %x\n", regs->err_code);
    
    if(!(regs->err_code & 0x01)) 
    {
        printk("Page wasn't present.\n");
    }

    if(regs->err_code & 0x2) 
    {
        printk("Write error.\n");
    }
    else
    {
        printk("Read error.\n");
    }

    if(regs->err_code & 0x04) 
    {
        printk("In user mode.\n");
    }
    else 
    {
        printk("In kernel mode.\n");
    }
    if(regs->err_code & 0x08) 
    {
        printk("reversed bits were overwritten.\n");
    }
    if(regs->err_code & 0x10) 
    {
        printk("The fault was occured during an instruction fetch.\n");
    }

    for(;;);
}
