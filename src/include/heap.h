#ifndef _HEAP_H_
#define _HEAP_H_

#ifndef NULL
#define NULL 0
#endif

#include "types.h"

#define HEAP_START 0xA0000000        // kernel heap base address
typedef struct heap_header
{
    struct heap_header *prev;
    struct heap_header *next;
    uint32_t allocated : 1;
    uint32_t length : 31;
} heap_header_t;

void init_heap();

void *kmalloc(uint32_t len);

void kfree(void *p);

#endif
