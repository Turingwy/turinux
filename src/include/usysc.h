#ifndef _SYSCALL_H_
#define _SYSCALL_H_

typedef unsigned int pid_t;

// open() omode
#define O_RDONLY 0x00       // open read only
#define O_WRONLY 0x01       // open write only
#define O_RDWR   0x02       // open read write
#define O_CREATE 0x200      // open create

int fork();
int exit();
int wait();
int kill(int pid);
pid_t getpid();
int sleep();
int link(char *oldpath, char *newpath);
int unlink(char *path);
int mkdir(char *path);
int open(char *path, int omode);
int close(int fd);
int write(int fd, char *addr, int len);
int read(int fd, char *addr, int len);
int exec(char *path, char **argv);
int dup(int fd);
int getcwd();
int chdir(char *path);
#endif
