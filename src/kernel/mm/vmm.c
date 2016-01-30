#include "vmm.h"
#include "pmm.h"
#include "types.h"
#include "stdio.h"
#include "page_fault.h"
__attribute__((aligned(PAGE_SIZE))) pgdirect_entry_t kern_pgd[PGDIRECT_COUNT];
__attribute__((aligned(PAGE_SIZE))) pgtable_entry_t kern_pgt[PGTABLE_SIZE][PGTABLE_COUNT];

void init_pg() 
{
    uint32_t kern_pgd_index = PGDIRECT_INDEX(PAGE_OFFSET);
    for(int i = kern_pgd_index, j = 0; i < PGDIRECT_COUNT; i++, j++)
        // kern_pgt的地址为虚拟地址，MMU要求页表地址为物理地址
        kern_pgd[i] = ((uint32_t)kern_pgt[j] - PAGE_OFFSET) | PAGE_WRITE | PAGE_PRESENT;
    for(int i = 0; i < PGTABLE_SIZE; i++)
    {
        for(int j = 0; j < PGTABLE_COUNT; j++)
        {
            kern_pgt[i][j] = (i << 22) | (j << 12) | PAGE_WRITE | PAGE_PRESENT;
        }
    }
    kern_pgt[0][0] = 0;             // 方便追查NULL指针
    uint32_t paddr = (uint32_t)kern_pgd - PAGE_OFFSET;
    switch_pgd(paddr);
}

void switch_pgd(uint32_t paddr) 
{
    asm volatile ("mov %0, %%cr3" : : "r" (paddr));
}

void init_vmm() 
{
    init_pg();        
    register_handler(0x0E, page_fault);
}


void map(pgd_t *cur_pgd, uint32_t va, uint32_t pa, uint32_t flags)
{
    uint32_t pgd_idx = PGDIRECT_INDEX(va);
    uint32_t pte_idx = PGTABLE_INDEX(va);
    pte_t *pte = cur_pgd[pgd_idx] & PAGE_MASK;
    if(!pte) 
    {
        pte = (pte_t *)(V2P((uint32_t)kpalloc()));
        cur_pgd[pgd_idx] = (uint32_t)pte | PAGE_WRITE | PAGE_PRESENT | PAGE_USER;
    }
    pte = (pte_t *)P2V((uint32_t)pte);

    pte[pte_idx] = PGROUNDUP(pa) | flags;
    asm volatile ("invlpg (%0)" : : "a" (va));
}

void unmap(pgd_t *cur_pgd, uint32_t va)
{
    uint32_t pgd_idx = PGDIRECT_INDEX(va);
    uint32_t pte_idx = PGTABLE_INDEX(va);
    pte_t *pte = cur_pgd[pgd_idx] & PAGE_MASK;
    if(!pte) 
        return;
    pte = (pte_t *)P2V((uint32_t)pte);
    pte[pte_idx] = 0;
    asm volatile ("invlpg (%0)" : : "a" (va));
}

uint32_t getmapping(pgd_t *cur_pgd, uint32_t va, uint32_t *pa)
{
    uint32_t pgd_idx = PGDIRECT_INDEX(va);
    uint32_t pte_idx = PGTABLE_INDEX(va);
    pte_t *pte = cur_pgd[pgd_idx] & PAGE_MASK;
    if(!pte) 
    {
        return 0;
    }

    pte = (pte_t *)P2V((uint32_t)pte);
    if(pte[pte_idx] & PAGE_PRESENT && pa)
    {
        *pa = pte[pte_idx] & PAGE_MASK;
        return 1;
    }
    return 0;
}

