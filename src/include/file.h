#ifndef _FILE_H_
#define _FILE_H_

#include "types.h"
#include "fs.h"

struct file 
{
    uint8_t type;
    uint8_t rw;
    uint16_t ref;
    uint32_t offset;
//  struct pipe *pipe;
    struct inode *in;
};

#define FTABLE_SZ 128

#define FD_INODE 0x01
#define FD_PIPE 0x02

#define FD_READABLE 0x01
#define FD_WRITABLE 0x02

#define O_RDONLY 0x00
#define O_WRONLY 0x01
#define O_RDWR   0x02
#define O_CREATE 0x200

struct file *filealloc();

int fdalloc(struct file *f);

int fddup(struct file *f);

int fddup_fd(int fd);

int read(int _fd, char *addr, int len);

int write(int _fd, char *addr, int len);

int close(int _fd);

int ilink(char *old_path, char *new_path);

int iunlink(char *path);

struct inode *create(char *path, uint16_t mode);

int open(char *path, int omode);

int mkdir(char *path);

#endif



