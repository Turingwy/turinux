#include "fs.h"
#include "dirent.h"
#include "bmap.h"
#include "bio.h"
#include "buf.h"

#define MIN(a, b)   ((a) > (b) ? (b) : (a))

// read data(offset $off, length $len) from inode $in(MUST BE LOCKED) 
// to destination $dest
int readi(struct inode *in, char *dest, uint32_t off, uint32_t len)
{
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

