#include "types.h"
#include "multiboot.h"
#include "stdio.h"
#include "vmm.h"
#include "pmm.h"
#include "idt.h"
#include "console.h"
#include "memory.h"


static uint32_t pmstart, pmend;
static struct phpage *phpages;
static struct phpage *freelist;
static uint32_t phpages_count;

multiboot_t *glb_mboot_ptr;
void show_mmap() 
{
    glb_mboot_ptr = (multiboot_t *)((uint32_t)(*(multiboot_t **)((uint32_t)&mboot_ptr_tmp + PAGE_OFFSET)) + PAGE_OFFSET);
    mmap_entry_t *mmap = (mmap_entry_t *)(glb_mboot_ptr->mmap_addr + PAGE_OFFSET);
    int len = glb_mboot_ptr->mmap_length / sizeof(mmap_entry_t);
    printk("Kernel start at: %x", kernel_start);
    print_state(OK_STATE);
    printk("Kernel end at  : %x", kernel_end);
    print_state(OK_STATE);

    for(int i = 0;i < len; i++)
    {
            printk("base addr: %x", mmap[i].base_addr_low);
            print_state(OK_STATE);
            if(mmap[i].length_low >= 0x700000) {
                pmstart = V2P((uint32_t)kernel_end + 0x5000);
                pmend = mmap[i].base_addr_low + mmap[i].length_low;
            }
    }

}
// vstart, vend 为内核末端到内存末端之间区域的虚拟地址
void init_pmm() 
{
    show_mmap();
    freerange((void *)P2V(pmstart), (void *)P2V(pmend));
    printk("init physical memory map");
    print_state(OK_STATE);
}

void freerange(void *vstart, void *vend) 
{
    phpages = (struct phpage *)PGROUNDUP((uint32_t)vstart);
    int i;
    for(i = (V2P(vstart) >> 12) + 128, freelist = &phpages[i]; i < (V2P(vend) >> 12); i++) {
        phpages[i].pg_num = i;
        phpages[i].pg_flag = 0;
        phpages[i].pg_count = 0;
        phpages[i].next = &phpages[i+1];
    }

    phpages[i-1].next = NULL;
    phpages_count = i;
}

void kpfree(uint8_t *v)
{
    if((uint32_t)v % PAGE_SIZE || V2P(v) < pmstart || V2P(v) >= pmend)
    {
        printk("kfree fault");
        return;
    }
    memset(v, 0, PAGE_SIZE);
    int n = V2P(v) >> 12;
//    cli();
    if(phpages[n].pg_count > 0)
        return;
    phpages[n].pg_count--;
    if(phpages[n].pg_count == 0)
    {
        phpages[n].next = freelist;
        freelist = &phpages[n];
    }
//    sti();
}

uint8_t *kpalloc()
{
    if(!freelist) {
        printk("No free page\n");
        return NULL;
    }

//    cli();
    int n = freelist->pg_num;
    freelist->pg_count = 1;
    freelist = freelist->next;
//    sti();
    return (uint8_t *)P2V(n << 12);
}

struct phpage *find_phpage(uint32_t pa)
{
    int n = pa >> 12;
    if(n < phpages_count)
        return &phpages[n];
    else
        return NULL;
}
