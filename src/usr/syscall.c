int syscall();

#define _SYSCALL(name, sysno)      \
        int name() {return syscall(sysno);}

_SYSCALL(fork, 0)
_SYSCALL(exit, 1)
_SYSCALL(wait, 2)
_SYSCALL(kill, 5)
_SYSCALL(create, 8)
_SYSCALL(getpid, 10)
_SYSCALL(sleep, 12)
_SYSCALL(link, 14)
_SYSCALL(getcwd, 18)

int open(char *path, int omode)
{
    return syscall(7, path, omode);
}

int close(int fd)
{
    return syscall(9, fd);
}
int write(int fd, char *addr, int len)
{
    return syscall(4, fd, addr, len);
}

int read(int fd, char *addr, int len)
{
    return syscall(3, fd, addr, len);
}

int exec(char *path, char **argv)
{
    return syscall(6, path, argv);
}

int unlink(char *path)
{
    return syscall(15, path);
}

int dup(int fd)
{
    return syscall(17, fd);
}

int mkdir(char *path)
{
    return syscall(16, path);
}

int chdir(char *path)
{
    return syscall(19, path); 
}
