#ifndef _DIRENT_H_
#define _DIRENT_H_

#include "types.h"

#define NAMELEN 12

#define NDIRECT 7

#define NINDIRECT 512

#define MAX_FILESZ 262663

struct dirent
{
    uint16_t ino;
    uint8_t name[NAMELEN];
    uint8_t padding[18];
};

struct inode *dirlookup(struct inode *dp, char *dirname, uint32_t *offp);

struct inode *namei(char *path, char *name);

struct inode *namepi(char *path, char *name);

#endif

