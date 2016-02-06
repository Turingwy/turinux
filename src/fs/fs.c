#include "fs.h"
#include "types.h"
#include "bio.h"
#include "buf.h"
#include "fs.h"
#include "memory.h"

struct superblock sb;

struct inode icache[NINODE];

void readsb(uint32_t dev)
{
    struct buf *b;
    b = bread(dev, 1);
    memcpy(&sb, b->data, sizeof(struct superblock));
    brelse(b);
}

int balloc(uint32_t dev) 
{
    readsb(dev);
//  printk("%d", sb.max_inode / IPB);
    for(int b = 0; b < sb.max_zone; b+=BPB)
    {
        struct buf *bp = bread(dev, BBLOCK(&sb, b));
        for(int bi = 0; bi < BPB && bi < (sb.max_zone - b); bi++)
        {
            int m = 1 << (bi % 8);
            if((bp->data[bi/8] & m) == 0)
            {
                bp->data[bi/8] |= m;
                bwrite(bp);
                brelse(bp);
                return b+bi+sb.zone0;
            }
        }
        brelse(bp);
    }
    return 0;
}

// allocated a new inode

int ialloc(uint32_t dev)
{
    readsb(dev);
    for(int inum = 1; inum < sb.max_inode; inum+=IPB)
    {
        struct buf *bp = bread(dev, IBLOCK(inum));
        for(int bi = 0; bi < IPB && bi < (sb.max_inode - inum); bi++)
        {
            int m = 1 <<  (bi % 8);
            if((bp->data[bi/8] & m) == 0)
            {
                bp->data[bi/8] |= m;
                bwrite(bp);
                brelse(bp);
                return inum + bi;
            }
        }
    }    
    return 0;
}

struct inode * iget(uint32_t dev, uint32_t inum)
{
    int empty = 0;
    for(int i = 0; i < NINODE; i++)
    {
       if(icache[i].ref > 0 && icache[i].dev == dev && icache[i].inum == inum)
        {
            icache[i].ref++;
            return &icache[i];
        }
       if(!empty && !icache[i].ref)
            empty = i;
    }

    if(!empty)
        return NULL;
    // if the inode doesn't in cache,
    // allocate fresh inode.
    icache[empty].ref++;
    icache[empty].dev = dev;
    icache[empty].inum = inum;
    icache[empty].flag = 0;
    return &icache[empty];
}

void idup(struct inode *in)
{
    ilock(in);
    in->ref++;
    iunlock(in);
}

void ilock(struct inode *in)
{
    if(in == 0 || in->ref < 1)
        return;     // TODO
    while(in->flag & I_BUSY)
        sleep(in, 0);
    in->flag |= I_BUSY;
    readsb(in->dev);

    if(!(in->flag & I_VALID)) 
    {
        struct buf *b = bread(in->dev, ISECTOR(&sb, in->inum));
        struct dinode *itable = (struct dinode *)(b->data);
        memcpy(in, &itable[(in->inum-1)%IPB], sizeof(struct dinode));
        in->flag |= I_VALID;
        brelse(b);
    }
}

void iunlock(struct inode *in)
{
    if(in == NULL || !(in->flag & I_BUSY) || in->ref < 1)
        return;         // TODO
    in->flag &= ~I_BUSY;
    wakeup(in);
}

#include "proc.h"
void iupdate(struct inode *in)
{
    struct buf *b = bread(in->dev, ISECTOR(&sb, in->inum));
    struct dinode *itable = (struct dinode *)b->data;
    memcpy(&itable[(in->inum-1)%IPB], in, sizeof(struct dinode));
    printk("pid: %d start:\n", current_proc->pid);
    bwrite(b);
    brelse(b);
    printk("pid: %d end:\n", current_proc->pid);
}

void iput(struct inode *in)
{
    if(in->ref == 1 && (in->flag & I_VALID)) {
        if(in->flag & I_BUSY)
            return;         // TODO
        iupdate(in);
        in->flag = 0;
    }
    in->ref--;
}
