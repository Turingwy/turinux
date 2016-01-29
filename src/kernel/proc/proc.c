#include "types.h"
#include "vmm.h"
#include "pmm.h"
#include "stdio.h"
#include "idt.h"
#include "proc.h"
#include "gdt.h"

struct cpu cpuinfo;
struct proc *current_proc;
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
        while(last_proc->next != running_procs)
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

static pgd_t *copyuvm(pgd_t *ppgd) 
{
    pgd_t *pgd = kpalloc();
    if(!pgd)
        return NULL;
    for(int i = 0; i < PGDIRECT_INDEX(PAGE_OFFSET); i+=1)
    {
        if(ppgd[i] & PAGE_PRESENT != 0) 
        {
            pte_t *pte = kpalloc();
            if(!pte)    return;
            pgd[i] = V2P(pte) | PAGE_PRESENT | PAGE_USER | PAGE_WRITE;
            memcpy(pte, P2V(ppgd[i] & PAGE_MASK),PAGE_SIZE);
            for(int j = 0; j < PGTABLE_COUNT; j++)
            {
                if(pte[j] & PAGE_PRESENT != 0)
                {
                    char *mem = kpalloc();
                    memset(mem, P2V(pte[j] & PAGE_MASK),PAGE_SIZE);
                    uint32_t pa = i << 22 | j << 12;
                    map(pgd, V2P(mem), pa, PAGE_WRITE|PAGE_PRESENT|PAGE_USER);
                }
            }
        }
    }
    for(int i = PGDIRECT_INDEX(PAGE_OFFSET); i < PGDIRECT_COUNT; i++)
            pgd[i] = kern_pgd[i];
    return pgd;
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
    p->parent = NULL;
    current_proc = p;

}

void scheduler(void)
{
    struct proc *p = current_proc->next;
    for(;;)
    {
        while(p != current_proc)
        {
            if(p->state == TASK_RUNNING)
                break;
            p = p->next;
        }
        current_proc = p;
        switchuvm(p);
        printk("pid:%d\n", p->pid);
        switch_to(&(cpuinfo.scheduler), p->context);
        p = p->next;
        current_proc = p;
        running_procs = p;
        switch_pgd(V2P(kern_pgd));
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
}


void sched()
{
    switch_to(&(current_proc->context), cpuinfo.scheduler);
}

int fork()
{
    struct proc *np;
    if((np = allocproc()) == NULL) 
        return -1;
    np->pgd = copyuvm(current_proc->pgd);
    if(np->pgd == NULL)
    {
        kpfree(np);
        return -1;
    }
    np->parent = current_proc;
    *np->regs = *current_proc->regs;
    np->regs->eax = 0;
    np->pid = now_pid++;
    np->state = TASK_RUNNING;
    strcpy(np->name, current_proc->name);
    return np->pid;
}

void sleep(void *chan)
{
    cli();
    current_proc->chan = chan;
    current_proc->state = TASK_SLEEPING;
    sti();
    sched();
    current_proc->chan = NULL;    
}



void wakeup(void *chan)
{
    struct proc *p;
    for(p = current_proc->next; p!=current_proc; p=p->next) {
        if(p->state == TASK_SLEEPING && p->chan == chan)
                p->state = TASK_RUNNING;

    }
}

int kill(pid_t pid)
{
    struct proc *p;
    for(p = current_proc->next; p != current_proc; p=p->next) 
    {
        if(p->pid == pid)
        {
            p->killed = 1;
            if(p->state == TASK_SLEEPING)
                p->state = TASK_RUNNING;
            return 0;
        } 
    }
    return -1;
}

void exit()
{
    struct proc *p;
    if(current_proc == init_proc) {
        printk("init exiting");
        return;
    }
    wakeup(p->parent);
    for(p = current_proc->next; p != current_proc; p = p->next)
    {
        if(p->parent == current_proc)
        {
            p->parent = init_proc;
            if(p->state == TASK_ZOMBIE)
                    wakeup(init_proc);
        }
    }
    current_proc->state = TASK_ZOMBIE;
    sched();
}

int wait()
{
    struct proc *p;
    struct proc *prev;
    for(;;) {
        int haskids = 0;
        for(p = current_proc->next, prev = current_proc; p != current_proc; p = p->next, prev = prev->next)
        {
            if(p->parent != current_proc)
                continue;
            haskids = 1;
            if(p->state == TASK_ZOMBIE)
            {
                pid_t pid = p->pid;
                prev->next = p->next;
                kpfree(p->pgd);
                kpfree(p);
                return pid;
            }
        }
        
        if(!haskids || current_proc->killed)
            return -1;
        sleep(current_proc);
    }
}
