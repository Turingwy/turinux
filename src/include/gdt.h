#ifndef _GDT_H_
#define _GDT_H_
#include "types.h"

typedef struct {
    uint16_t seg_limit_low;
    uint16_t base_low;
    uint8_t base_mid;
    uint8_t access;
    uint8_t gran;
    uint8_t base_high;
} __attribute__((packed)) gdt_entry_t;

typedef struct {
    uint16_t limit;
    uint32_t addr;
} __attribute__((packed)) gdt_ptr_t;

void init_gdt();

void gdt_flush(uint32_t addr);

#define GDT_LENGTH 5
#endif
