#include "fs.h"
#include "buf.h"
#include "bio.h"
#include "dirent.h"

// the inode MUST be locked!
// when this function come back,
// MUST unlock inode!
uint32_t bmap(struct inode *in, uint16_t bn, int create)
{
    if(bn > MAX_FILESZ)
        return 0;
    uint32_t addr = 0;
    if(bn < NDIRECT)
    {
        if((addr = in->zone[bn]) == 0 && create) 
            in->zone[bn] = addr = balloc(in->dev);
        return addr;
    }
    
    bn -= NDIRECT;
    if(bn < NINDIRECT)
    {
        if((addr = in->zone[NDIRECT]) == 0) {
            if(create ==0)
                return 0;
            in->zone[NDIRECT] = addr = balloc(in->dev);
        }
        struct buf *bp = bread(in->dev, addr);
        uint16_t *nos = (uint16_t *)bp->data;
        if((addr = nos[bn]) == 0 && create) 
        {
            nos[bn] = addr = balloc(in->dev);
            bwrite(bp);
        }
        brelse(bp);
        return addr;
    }

    bn -= NINDIRECT;
    if((addr = in->zone[NDIRECT+1]) == 0) 
    {
        if(create == 0)
            return 0;
        in->zone[NDIRECT+1] = addr = balloc(in->dev);
        struct buf *bp = bread(in->dev, addr);
        uint16_t *segs = (uint16_t *)bp->data;
        int segment = bn/NINDIRECT;
        if((addr = segs[segment]) == 0)
        {
            if(!create) 
            {
                brelse(bp);
                return 0;
            }
            segs[segment] = addr = balloc(in->dev);
            bwrite(bp);
        }     
        brelse(bp);
        bp = bread(in->dev, addr);
        uint16_t *nos = (uint16_t *)bp->data;
        int no = bn%NINDIRECT;
        if((addr = nos[no]) == 0 && create)
        {
            nos[no] = addr = balloc(in->dev);
            bwrite(bp);
        }
        brelse(bp);
        return addr;
    }
    return 0;
}
