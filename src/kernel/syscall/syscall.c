#include "types.h"
#include "syscall.h"
#include "idt.h"
#include "proc.h"
#include "stdio.h"
#include "timer.h"

int fetchint(struct proc *proc, uint32_t addr, int *ip)
{
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
    *pp = (char *)p;
    return 0;
}

int argstr(int n, char **pp)
{
    int p;
    if(argint(n, &p) < 0)
        return -1;
    char *str = (char *)p;
    while((uint32_t)str)
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
int sys_exec();
int sys_read();
int sys_write();
int sys_open();
int sys_create();
int sys_close();
int sys_link();
int sys_unlink();
int sys_mkdir();

syscall_t syscalls[SYSCALL_LEN] = {
        [0]  sys_fork,
        [1]  sys_exit,
        [2]  sys_wait,
        [3]  sys_read,
        [4]  sys_write,
        [5]  sys_kill,
        [6]  sys_exec,
        [7]  sys_open,
        [8]  sys_create,
        [9]  sys_close,
        [10] sys_getpid,
        [12] sys_sleep,
        [13] sys_puts,
        [14] sys_link,
        [15] sys_unlink,
        [16] sys_mkdir
};

void syscall(regs_pt *regs)
{
    current_proc->regs = regs;
    int syscall_no = regs->eax;
    if(syscall_no >= 0 && syscall_no < SYSCALL_LEN && syscalls[syscall_no]
) {
           printk("pid:%d, eax:%d\n", current_proc->pid,regs->eax);
           regs->eax = syscalls[syscall_no]();
    }
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
        sleep(&ticks, 0);
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
    printk("pid:%d\n", current_proc->pid);
    return current_proc->pid;
}

void sys_puts()
{
/*    char *str;
    if(argstr(0, &str) < 0)
        return -1;;
    printk(str);
    return 0;
*/
    test();
}


int sys_exec()
{
    char *path, **argv;
    if(argstr(0, &path) < 0 || argptr(1, &argv, 4) < 0) {
        printk("%s\n", path);
        return -1;
    }
    return exec(path, argv);

}

int sys_read()
{
    int fd;
    char *addr;
    int len;
    if(argint(0, &fd) < 0 || argptr(1, &addr, 1) < 0 ||
                    argint(2, &len) < 0)
            return -1;
    return read(fd, addr, len);
}

int sys_write()
{
    int fd;
    char *addr;
    int len;
    if(argint(0, &fd) < 0 || argptr(1, &addr, 1) < 0 ||
                    argint(2, &len) < 0)
            return -1;
    return write(fd, addr, len);
}

int sys_close()
{
    int fd;
    if(argint(0, &fd) < 0)
        return -1;
    return close(fd);
}

int sys_link()
{
    char *old_path, *new_path;
    if(argptr(0, &old_path, 1) < 0 || argptr(1, &new_path, 1) < 0)
        return -1;
    return ilink(old_path, new_path);
}

int sys_unlink()
{
    char *path;
    if(argptr(0, &path, 1) < 0)
        return -1;
    return iunlink(path);
}

int sys_create()
{
    char *path;
    uint32_t mode;

    if(argptr(0, &path, 1) < 0 || argint(1, &mode) < 0)
        return -1;
    return create(path, mode);
}

int sys_open()
{ 
    char *path;
    uint32_t omode;

    if(argptr(0, &path, 1) < 0 || argint(1, &omode) < 0)
        return -1;
    return open(path, omode);
}

int sys_mkdir()
{
    char *path;

    if(argptr(0, &path, 1) < 0)
        return -1;
    return mkdir(path);

}

