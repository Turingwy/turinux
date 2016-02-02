#ifndef _RWI_H_
#define _RWI_H_

#include "types.h"
#include "fs.h"

int readi(struct inode *in, char *dest, uint32_t off, uint32_t len);

int writei(struct inode *in, char *src, uint32_t off, uint32_t len);

#endif
