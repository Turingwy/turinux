#include "types.h"
#include "syscall.h"
#include "idt.h"
#include "proc.h"
#include "stdio.h"
#include "timer.h"
int fetchint(struct proc *proc, uint32_t addr, int *ip)
{
    if(addr > PAGE_SIZE || addr + 4 > PAGE_SIZE)
        return -1;
    *ip = *(int *)addr;
    return 0;
}

int argint(int n, int *ip)
{
    return fetchint(current_proc, current_proc->regs->useresp + 4 +4*n, ip);
}

int argptr(int n, char **pp, int sz) 
{
    int p;
    if(argint(n, &p) < 0)
        return -1;
    if((uint32_t)p >= PAGE_SIZE || (uint32_t)p + sz >= PAGE_SIZE)
        return -1;
    *pp = (char *)p;
    return 0;
}

int argstr(int n, char **pp)
{
    int p;
    if(argint(n, &p) < 0)
        return -1;
    char *str = (char *)p;
    while((uint32_t)str < PAGE_SIZE)
    {
        if(*str++ == '\0') {
            *pp = (char *)p;
            return 0;
        }
    }
    return -1;
}

void sys_puts();
int sys_fork();
int sys_exit();
int sys_wait();
int sys_kill();
pid_t sys_getpid();
int sys_sleep();
syscall_t syscalls[SYSCALL_LEN] = {
        [0]  sys_fork,
        [1]  sys_exit,
        [2]  sys_wait,
        [5]  sys_kill,
        [10] sys_getpid,
        [12] sys_sleep,
        [13] sys_puts};

void syscall(regs_pt *regs)
{
    current_proc->regs = regs;
    int syscall_no = regs->eax;
    if(syscall_no >= 0 && syscall_no < SYSCALL_LEN && syscalls[syscall_no]
)
           regs->eax = syscalls[syscall_no]();
    else
    {
        printk("pid %d,name %d, unkown system call %d\n", current_proc->pid, current_proc->name, syscall_no);
    }
}

int sys_fork()
{
    return fork();
}

int sys_sleep()
{
    int n;
    if(argint(0, &n) < 0)
        return -1;

    int ticks0 = ticks;
    while(ticks - ticks0 < n) {
        if(current_proc->killed)
            return -1;
        sleep(&ticks);
    }

    return 0;
}

int sys_exit()
{
    exit();
    return 0;
}

int sys_wait()
{
    return wait();
}

int sys_kill()
{
    pid_t kill_pid;
    if(argint(0, &kill_pid) < 0)
        return -1;

    return kill(kill_pid);
}

pid_t sys_getpid()
{
    return current_proc->pid;
}

void sys_puts()
{
    char *str;
    if(argstr(0, &str) < 0)
        return -1;;
    printk(str);
    return 0;
}
