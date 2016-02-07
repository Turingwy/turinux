#ifndef _BIO_H_
#define _BIO_H_

#include "types.h"

#define BUF_SIZE 1024

void binit();

struct buf *bread(uint32_t dev, uint32_t sector);

void bwrite(struct buf *b);

void brelse(struct buf *b);
#endif
