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

#define FD_INODE 1
#define FD_PIPE 2

#define FD_READABLE 1
#define FD_WRITABLE 2
#endif



