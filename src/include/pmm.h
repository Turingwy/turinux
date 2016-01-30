#ifndef _PMM_H_
#define _PMM_H_
#include "types.h"

extern uint8_t kernel_start[];
extern uint8_t kernel_end[];
void show_mmap();
void init_pmm();
void freerange(void *vstart, void *vend);
void kpfree(uint8_t *);
uint8_t *kpalloc();

struct phpage
{
    uint8_t pg_flag;
    uint8_t pg_count;   // reference count
    uint16_t pg_num;
    struct phpage *next;
};

struct phpage *find_phpage(uint32_t va);
#endif
