#ifndef _STAT_H_
#define _STAT_H_

#include "types.h"

// simple file stat
struct stat
{
    uint32_t dev;
    uint32_t ino;
    uint32_t mode;
    uint32_t nlink;
    uint32_t uid;
    uint32_t gid;
    uint32_t size;
    uint32_t mtime;
};

#define FMASK 0xF000
#define FREG 0x8000
#define FBLK 0x6000
#define FDIR 0x4000
#define FCHR 0x2000
#define FFIFO 0x1000

#define ISREG(m) (((m) & FMASK ) == FREG)

#define ISBLK(m) (((m) & FMASK ) == FBLK)

#define ISDIR(m) (((m) & FMASK ) == FDIR)

#define ISCHR(m) (((m) & FMASK ) == FCHR)

#define ISFIFO(m) (((m) & FMASK ) == FFIFO)

#endif
