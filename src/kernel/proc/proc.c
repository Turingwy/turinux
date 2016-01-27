#include "types.h"
#include "vmm.h"
#include "pmm.h"
#include "stdio.h"
#include "idt.h"
#include "proc.h"
#include "gdt.h"

struct cpu cpuinfo;
struct proc *running_procs;
struct proc *waiting_procs;
struct proc *init_proc;
pid_t now_pid;
static struct proc *allocproc() 
{
    struct proc *p;
    uint8_t *sp;        // top of kernel stack;
    if((p = (struct proc *)kpalloc()) == 0)
        return NULL;
    struct proc *last_proc = running_procs;
    if(last_proc == NULL)
        running_procs = p;
    else {
        while(last_proc->next)
            last_proc = last_proc->next;      
        last_proc->next = p;
    }

    p->kstack = (uint8_t *)p;
    sp = p->kstack + STACK_SIZE;

    sp -= sizeof(regs_pt);  // trapret use this regs to get back;
    p->regs = (struct regs_pt *)sp;
    sp -= sizeof(struct context);  // kernel context
    p->context = (struct context *)sp;
    memset(p->context, 0, sizeof(struct context));
    p->context->eip = (uint32_t)intrret;
    p->next = running_procs;
    return p;
}

void inituvm(pgd_t *pgd, uint8_t *start, uint8_t *sz)
{
    uint32_t mem = kpalloc();
    bzero(mem, PAGE_SIZE);
    map(pgd, 0, V2P(mem), PAGE_WRITE | PAGE_PRESENT | PAGE_USER);
    memcpy(mem, start, (uint32_t)sz);
} 

pgd_t *setup_kpgd() 
{
    pgd_t *pgd = kpalloc();
    bzero(pgd, sizeof(pgd_t));
    for(int i = PGDIRECT_INDEX(PAGE_OFFSET); i < PGDIRECT_COUNT; i++) 
        pgd[i] = kern_pgd[i];
    return pgd;
}
extern uint8_t kernel_end[], binary_size[];
// init first user process;
void userinit()
{
    struct proc *p;
    p = allocproc();
    init_proc = p;
    p->pgd = setup_kpgd();
    inituvm(p->pgd, kernel_end, binary_size);
    memset(p->regs, 0, sizeof(regs_pt));
    p->regs->cs = SEG_UCODE << 3 | 0x3;
    p->regs->ds = SEG_UDATA << 3| 0x3;
    p->regs->useresp = PAGE_SIZE;
    p->regs->eip = 0x0;
    p->regs->userss = p->regs->ds;
    p->regs->eflags = 0x200;
    strcpy(p->name, "init");
    p->state = TASK_RUNNING;
    p->pid = now_pid++;
}

void scheduler(void)
{
    struct proc *p;
    for(;;)
    {
        p = running_procs;
        while(p)
        {
            if(p->state == TASK_RUNNING)
                break;
            p = p->next;
        } 
        cpuinfo.current_proc = p;
        switchuvm(p);
        switch_to(&(cpuinfo.scheduler), p->context);
        for(;;);
        switch_pgd(V2P(kern_pgd));
        cpuinfo.current_proc = NULL;
    }
}

void switchuvm(struct proc *p) 
{
    cli();
    set_gdt_entry(SEG_TSS, &(cpuinfo.ts), sizeof(cpuinfo.ts)-1, 0x89, 0x4F);
    cpuinfo.ts.ss0 = SEG_KDATA << 3;
    cpuinfo.ts.esp0 = (uint32_t)p->kstack + STACK_SIZE;
    ltr(SEG_TSS << 3);
    switch_pgd(V2P(p->pgd));
    // sti();
}

