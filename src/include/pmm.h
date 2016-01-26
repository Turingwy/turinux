#ifndef _PMM_H_
#define _PMM_H_
extern uint8_t kernel_start[];
extern uint8_t kernel_end[];
void show_mmap();
void init_pmm();
void freerange(void *vstart, void *vend);
void kpfree(uint8_t *);
uint8_t *kpalloc();
#endif
