#include "types.h"
#include "multiboot.h"
#include "stdio.h"
#include "vmm.h"
#include "pmm.h"
/*
 * 空闲链表
 */
struct run 
{
    struct run *next;
};

uint32_t pmstart, pmend;
struct run *freelist;

multiboot_t *glb_mboot_ptr;
void show_mmap() 
{
    glb_mboot_ptr = (multiboot_t *)((uint32_t)(*(multiboot_t **)((uint32_t)&mboot_ptr_tmp + PAGE_OFFSET)) + PAGE_OFFSET);
    mmap_entry_t *mmap = (mmap_entry_t *)(glb_mboot_ptr->mmap_addr + PAGE_OFFSET);
    int len = glb_mboot_ptr->mmap_length / sizeof(mmap_entry_t);
    printk("Kernel start at: %x\n", kernel_start);
    printk("Kernel end at  : %x\n", kernel_end);
    for(int i = 0;i < len; i++)
    {
            printk("base addr: %x\n", mmap[i].base_addr_low);
            if(mmap[i].length_low >= 0x700000) {
                pmstart = V2P((uint32_t)kernel_end + 0x1000);
                pmend = mmap[i].base_addr_low + mmap[i].length_low;
            }
    }

}
// vstart, vend 为内核末端到内存末端之间区域的虚拟地址
void init_pmm() 
{
    show_mmap();
    freerange(P2V(pmstart), P2V(pmend));
}

void freerange(void *vstart, void *vend) 
{
    uint8_t *p;
    p = (uint8_t *)PGROUNDUP((uint32_t)vstart);
    for(; p + PAGE_SIZE <= (uint8_t*)vend; p+=PAGE_SIZE)
        kpfree(p);
}

void kpfree(uint8_t *v)
{
    struct run *r;
    if((uint32_t)v % PAGE_SIZE || v < pmstart || V2P(v) >= pmend)
    {
        printk("kfree fault");
        return;
    }
    memset(v, 0, PAGE_SIZE);
    r = (struct run *)v;
    r->next = freelist;
    freelist = r;
}

uint8_t *kpalloc()
{
    struct run *r;
    r = freelist;
    if(r)
        freelist = freelist->next;
    return r;
}
