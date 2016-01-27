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
void set_gdt_entry(int num, uint32_t base, uint32_t limit,uint32_t access,uint32_t gran);

struct taskstate {
  uint32_t link;         // Old ts selector
  uint32_t esp0;         // Stack pointers and segment selectors
  uint16_t ss0;        //   after an increase in privilege level
  uint16_t padding1;
  uint32_t *esp1;
  uint16_t ss1;
  uint16_t padding2;
  uint32_t *esp2;
  uint16_t ss2;
  uint16_t padding3;
  void *cr3;         // Page directory base
  uint32_t *eip;         // Saved state from last task switch
  uint32_t eflags;
  uint32_t eax;          // More saved state (registers)
  uint32_t ecx;
  uint32_t edx;
  uint32_t ebx;
  uint32_t *esp;
  uint32_t *ebp;
  uint32_t esi;
  uint32_t edi;
  uint16_t es;         // Even more saved state (segment selectors)
  uint16_t padding4;
  uint16_t cs;
  uint16_t padding5;
  uint16_t ss;
  uint16_t padding6;
  uint16_t ds;
  uint16_t padding7;
  uint16_t fs;
  uint16_t padding8;
  uint16_t gs;
  uint16_t padding9;
  uint16_t ldt;
  uint16_t padding10;
  uint16_t t;          // Trap on task switch
  uint16_t iomb;       // I/O map base address
};

#define GDT_LENGTH 6
#define SEG_KCODE 1
#define SEG_KDATA 2
#define SEG_UCODE 3
#define SEG_UDATA 4
#define SEG_TSS   5
#endif
