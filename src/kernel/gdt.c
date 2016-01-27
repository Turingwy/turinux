#include "types.h"
#include "gdt.h"

gdt_entry_t gdt[GDT_LENGTH];

gdt_ptr_t gdt_ptr;

void set_gdt_entry(int num, uint32_t base, uint32_t limit,uint32_t access,uint32_t gran)
{
    gdt[num].base_low = base & 0xFFFF;
    gdt[num].base_mid = (base >> 16) & 0xFF;
    gdt[num].base_high = (base >> 24) & 0xFF;
    gdt[num].seg_limit_low = limit & 0xFF;
    gdt[num].access = access;
    gran <<= 4;
    gran |= (limit >> 16 & 0x0F);
    gdt[num].gran = gran & 0xFF;
}


void init_gdt()
{
    set_gdt_entry(0, 0, 0, 0, 0); 
    set_gdt_entry(SEG_KCODE, 0, 0xFFFFFFFF, 0x9A, 0x0C);        // kernel cs
    set_gdt_entry(SEG_KDATA, 0, 0xFFFFFFFF, 0x92, 0x0C);        // kernel ds
    set_gdt_entry(SEG_UCODE, 0, 0xFFFFFFFF, 0xFA, 0x0C);        // user cs
    set_gdt_entry(SEG_UDATA, 0, 0xFFFFFFFF, 0xF2, 0x0C);        // user ds
    gdt_ptr.addr = gdt;
    gdt_ptr.limit = sizeof(gdt_entry_t) * GDT_LENGTH - 1;
    gdt_flush(&gdt_ptr);
}
