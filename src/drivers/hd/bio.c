#include "types.h"
#include "buf.h"
#include "proc.h"
#include "bio.h"


struct {
    // disk block cache in kernel
    struct buf buf[BUF_SIZE];
    // lru_head links list of all buf, through prev/next.
    // the most recently used block is almost at the head,
    // so when we search a valid cache, search from head to tail.
    // when we search a free cache, search from tail to head.
    struct buf lru_head;
} bcache;

// link all bufs, lru_head point to the head
void binit()
{
    bcache.lru_head.next = &bcache.buf[0];
    bcache.lru_head.prev = &bcache.buf[BUF_SIZE-1];
    for(int i = 1; i < BUF_SIZE-1; i++) {
        bcache.buf[i].next = &bcache.buf[i+1];
        bcache.buf[i].prev = &bcache.buf[i-1];
    }
    bcache.buf[0].prev = &bcache.lru_head;
    bcache.buf[0].next = &bcache.buf[1];
    bcache.buf[BUF_SIZE-1].prev = &bcache.buf[BUF_SIZE-2];
    bcache.buf[BUF_SIZE-1].next = &bcache.lru_head;

}
// get a cache for sector on device now.
// if not found,allocate fresh block.
//
static struct buf *bget(uint32_t dev, uint32_t sector)
{
    struct buf *b;
    cli();
loop:
    for(b = bcache.lru_head.next; b != &bcache.lru_head; b = b->next)
    {
        if(b->dev == dev && b->sector == sector) 
        {
            if((b->flags & B_BUSY)) 
            {
                sleep(b, 1);
                // when thread wakeup,it's possible that 
                // it't not the first one being sched.
                // so the safe way is goto the head,and restart.
                goto loop;
            }
            b->flags |= B_BUSY;
            sti();
            return b;
        }
    }
    // if not found, try to allocated fresh block.
    for(b = bcache.lru_head.prev; b != &bcache.lru_head; b = b->prev)
    {
        if(!(b->flags & B_BUSY))
        {
            b->dev = dev;
            b->sector = sector;
            // clean old flag, and set B_BUSY,
            // so bread must call iderw to flush this buffer cache.
            b->flags = B_BUSY;
            sti();
            return b;
        }
    }
    sti();
    return NULL;
}

struct buf *bread(uint32_t dev, uint32_t sector)
{
    struct buf *b;
    b = bget(dev, sector);
    if(!b)
        return NULL;
    if(!(b->flags & B_VALID))
        iderw(b);
    return b;
}

void bwrite(struct buf *b)
{
    if(!(b->flags & B_BUSY))
        return;             // TODO

    b->flags |= B_DIRTY;
    iderw(b);
}

void brelse(struct buf *b)
{
    if(!(b->flags & B_BUSY))
        return;         // TODO
    
    b->next->prev = b->prev;
    b->prev->next = b->next;
    b->next = bcache.lru_head.next;
    b->prev = &bcache.lru_head;
    bcache.lru_head.next->prev = b;
    bcache.lru_head.next = b;
    b->flags &= ~B_BUSY;

    wakeup(b);
}


