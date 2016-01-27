#ifndef _PROC_H_
#define _PROC_H_

#include "types.h"
#include "gdt.h"
typedef uint32_t pid_t;

enum proc_state {
        TASK_RUNNING, 
        TASK_SLEEPING,
        TASK_SWAIT,
        TASK_ZOMBIE,
        TASK_DEAD
};


// cs 等段寄存器无需保存,因为它们对于内核态而言为固定的常量
typedef struct context
{
    uint32_t edi;
    uint32_t esi;
    uint32_t ebx;
    uint32_t ebp;
    uint32_t eip;
} context;

struct proc 
{
    pgd_t *pgd;
    uint8_t *kstack;
    volatile enum proc_state state;
    volatile uint32_t pid;
    struct proc *parent;
    regs_pt *regs;
    context *context;
    struct proc *next;
    char name[16];
}; 

extern pid_t now_pid;

struct cpu
{
    struct context *scheduler;
    struct taskstate ts;
    struct proc *current_proc;
};

static inline void ltr(uint16_t selector) 
{
    asm volatile("ltr %0" :: "r"(selector));
}

#endif
