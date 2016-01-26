#include "console.h"
#include "string.h"
#include "gdt.h"
#include "idt.h"
#include "pmm.h"
#include "vmm.h"
#include "timer.h"
void page_flush(uint32_t addr, uint32_t stack_top);

uint8_t kern_stack[STACK_SIZE];
void main();
__attribute__((section(".init.data"))) pgdirect_entry_t pgdirect[PGDIRECT_COUNT] __attribute__ ((aligned(PAGE_SIZE))) ;
__attribute__((section(".init.data"))) pgtable_entry_t pgtable_lo[PGTABLE_COUNT] __attribute__((aligned(PAGE_SIZE)));
__attribute__((section(".init.data"))) pgtable_entry_t pgtable_hi[PGTABLE_COUNT] __attribute__((aligned(PAGE_SIZE)));

__attribute__((section(".init.text"))) void kern_entry() 
{
    pgdirect[0] = (uint32_t)pgtable_lo | PAGE_WRITE | PAGE_PRESENT;
    pgdirect[PGDIRECT_INDEX(PAGE_OFFSET)] = (uint32_t)pgtable_hi | PAGE_WRITE | PAGE_PRESENT;
    
    for(int i = 0; i < PGTABLE_COUNT; i++) {
        pgtable_lo[i] = i << 12 | PAGE_WRITE | PAGE_PRESENT;
        pgtable_hi[i] = i << 12 | PAGE_WRITE | PAGE_PRESENT;
    }
    page_flush((uint32_t)pgdirect, (uint32_t)(kern_stack + STACK_SIZE));
    main();
}

void main() 
{
    cls();
    init_gdt();
    init_idt();
    init_timer();
    init_vmm();
    init_pmm();
    int *i = kmalloc(4);
    *i = 15;
    printk("%d\n", *i);
    kfree(i);
    *i = 5; 
    for(;;);            // never return back! 
}
