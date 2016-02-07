#include "fs.h"
#include "dirent.h"
#include "bio.h"
#include "buf.h"
#include "dev.h"
#include "stat.h"
#include "memory.h"

#define MIN(a, b)   ((a) > (b) ? (b) : (a))

// read data(offset $off, length $len) from inode $in(MUST BE LOCKED) 
// to destination $dest
int readi(struct inode *in, char *dest, uint32_t off, uint32_t len)
{
     // if this inode is a char device,
     // it's device number is in zone[0]
     // note:
     // devs[devno].read is a callback function that read from 
     // character device.
     if(ISCHR(in->mode))
     {
        int devno = in->zone[0];
        if(devno > DEVSCNT)
            return -1;
        return devs[devno].read(in, dest, len);
     }

     if(in->size < off || len < 0)
        return -1;
     if(off + len > in->size)
        len = in->size - off;

     int m, i;
     struct buf *bp;
     for(i = 0; i < len; i+=m, off+=m, dest+=m)
     {
        bp = bread(in->dev, bmap(in, off/BSIZE, 1));
        m = MIN(len-i, BSIZE - off%BSIZE);
        memcpy(dest, bp->data + off%BSIZE, m);
        brelse(bp);
     }
     return len;
}

int writei(struct inode *in, char *src, uint32_t off, uint32_t len)
{
    if(ISCHR(in->mode))
    {
        uint16_t devno = in->zone[0];
        if(devno >= DEVSCNT)
            return -1;
        return devs[devno].write(in, src, len);        
    }

    if(in->size < off || len < 0)       // can't write a blank space
        return -1;
    if(off + len > MAX_FILESZ*BSIZE)    // MAX_FILESZ is minix's max file logical block size
        return -1;

    int i, m = 0;
    struct buf *bp;
    for(i = 0; i < len; i+=m, off+=m, src+=m)
    {
        // if goto file EOF, bmap can auto alloc a free block,
        // and save addr of this block in file'inode
        bp = bread(in->dev, bmap(in, off/BSIZE,1));
        m = MIN(len-i, BSIZE - off%BSIZE);
        memcpy(bp->data + off%BSIZE, src, m);
        bwrite(bp);
        brelse(bp);
    }

    if(off > in->size) 
        in->size = off;      // it's doesn't matter if we don't update this inode,because caller will unlock inode, and iupdate() is called.
    return i;
}

