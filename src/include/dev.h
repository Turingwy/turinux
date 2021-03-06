#ifndef _DEVS_H_
#define _DEVS_H_

#include "types.h"
#include "fs.h"

struct dev
{
    int (*read)(struct inode *in, char *addr, int len);
    int (*write)(struct inode *in, char *addr, int len);
};

extern struct dev devs[];

void consoleinit();

void kbdinit();

#define DEVSCNT 64

#define CONSOLE 0

#endif

