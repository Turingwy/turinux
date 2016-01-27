#ifndef _VMM_H_
#define _VMM_H_
#include "types.h"

typedef uint32_t pgdirect_entry_t;
typedef uint32_t pgtable_entry_t;
typedef pgdirect_entry_t pgd_t;
typedef pgtable_entry_t pte_t;
#define STACK_SIZE 4096
#define PAGE_SIZE 4096
#define PGDIRECT_COUNT 1024
#define PGTABLE_COUNT 1024
#define PGTABLE_SIZE 256        // 映射 1G 地址
#define PAGE_OFFSET 0xC0000000
#define PAGE_MASK   0xFFFFF000
#define PGDIRECT_INDEX(addr)  ((addr)>>22)
#define PGTABLE_INDEX(addr) ((addr)>>12 & 0x3FF)
#define PG_INDEX(addr)      ((addr) & 0xFFF)
#define V2P(vaddr)          ((uint32_t)(vaddr) - PAGE_OFFSET)
#define P2V(paddr)          ((uint32_t)(paddr) + PAGE_OFFSET)
#define PGROUNDUP(vaddr)    ((vaddr) & PAGE_MASK)
#define PAGE_PRESENT 1
#define PAGE_WRITE   2
#define PAGE_USER    4
pgdirect_entry_t kern_pgd[];
pgtable_entry_t kern_pte[][PGTABLE_COUNT];

void init_vmm();
void init_pg();
void switch_pgd(uint32_t paddr);
void map(pgd_t *cur_pgd, uint32_t va, uint32_t pa, uint32_t flags);
void unmap(pgd_t *cur_pgd, uint32_t va);
uint32_t getmapping(pgd_t *cur_pgd, uint32_t va, uint32_t *pa);
#endif
