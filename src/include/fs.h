#ifndef _FS_H_
#define _FS_H_

#include "types.h"

struct superblock 
{
    uint16_t max_inode;
    uint16_t max_zone;
    uint16_t imap_blk;
    uint16_t zmap_blk;
    uint16_t zone0;
    uint16_t log_bz;
    uint32_t max_filesz;
    uint16_t magic;         // 0x138F
    uint16_t padding;
};

// on disk inode structure
struct dinode
{
    uint16_t mode;
    uint16_t uid;
    uint32_t size;
    uint32_t mtime;
    uint8_t  gid;
    uint8_t nlinks;
    uint16_t zone[9]; 
};

struct inode 
{
    uint16_t mode;
    uint16_t uid;
    uint32_t size;
    uint32_t mtime;
    uint8_t  gid;
    uint8_t nlinks;
    uint16_t zone[9]; 
    // only in core
    uint16_t ref;
    uint16_t dev;
    uint32_t inum;
    uint32_t flag;
};

#define I_BUSY 0x1
#define I_VALID 0x2
#define I_DIRTY 0x4

#define NINODE 128

extern struct inode icache[];

// minix superblock's magic num
#define SP_MAGIC 0x138F
// logical block's size
#define BSIZE 1024
// bits per block
#define BPB (BSIZE * 8)

#define IPB (BSIZE / sizeof(struct dinode))

// data block number 2 sector no
#define BBLOCK(sp, bn)  (2 + ((sp)->imap_blk) + ((bn)/BPB))

#define IBLOCK(in)  (2 + ((in)/BPB))

#define ISECTOR(sp, in) (2 + ((sp)->imap_blk) + ((sp)->zmap_blk) \
                + ((in)-1)/IPB)
#endif
