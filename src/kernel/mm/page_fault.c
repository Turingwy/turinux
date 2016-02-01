#include "types.h"
#include "vmm.h"
#include "idt.h"
#include "stdio.h"
#include "pmm.h"
#include "proc.h"
#include "buf.h"

void do_wp_page(uint32_t va)
{
    uint32_t pa;
    getmapping(current_proc->pgd, va, &pa);
    struct phpage *pg = find_phpage(pa);
    if(!pg)  return;         // TODO

    if(pg->pg_count > 1)
    {
        char *old_page = (char *)(P2V(pa));
        char *new_page = kpalloc();
        memcpy(new_page, old_page, PAGE_SIZE);
        map(current_proc->pgd, va, V2P(new_page), PAGE_PRESENT|PAGE_USER|PAGE_WRITE);
        pg->pg_count--;
    }
    else if(pg->pg_count == 1)
        map(current_proc->pgd, va, pa, PAGE_PRESENT|PAGE_USER|PAGE_WRITE);
}

void page_fault(regs_pt *regs)
{
    uint32_t cr2;
    asm volatile("mov %%cr2, %0" : "=r" (cr2));

    printk("Page fault at %x, virtual address at %x.\n", regs->eip, cr2);
    //printk("Error code: %x\n", regs->err_code);
    
    if(!(regs->err_code & PAGE_PRESENT))
    {
        printk("Page wasn't present.\n");
    }

    if(regs->err_code & PAGE_WRITE) 
    {
        do_wp_page(cr2);
        return;
    }
    else
    {
        printk("Read error.\n");
    }

    if(regs->err_code & PAGE_USER) 
    {
        printk("In user mode.\n");
    }
    else 
    {
        printk("In kernel mode.\n");
    }

    for(;;);
}
