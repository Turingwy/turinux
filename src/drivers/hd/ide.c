#include "types.h"
#include "proc.h"
#include "idt.h"
#include "ioport.h"
#include "buf.h"
/* busy. if bsy = 1, no other bits in the 
 * register are valid.
 */
#define IDE_BSY 0x80
// drive ready.
#define IDE_DRDY 0x40
// device fault 
#define IDE_DF 0x20
// an error occurred
#define IDE_ERR 0x01

#define IDE_CMD_READ 0x20
#define IDE_CMD_WRITE 0x30

static struct buf *idequeue;
static void idestart(struct buf *b);

static int hasdisk1;
/*
 * wait for ide disk to become ready.
 * return -1 if disk return a error code.
 * if don't have hdisk, the loop will never break,
 * in fact, even if it return to the main funcation, 
 * the only thing to do is shutdown.
 */
static int idewait(int checkerr)
{
    int r;
    while((r = inb(0x1f7) & (IDE_BSY|IDE_DRDY)) != IDE_DRDY)
        ;

    if(checkerr && (r & (IDE_DF|IDE_ERR))!=0)
        return -1;

    return 0;
}

void ideinit()
{
    idewait(1);
    
    /*
     * io port 0x1f6 is a 8-bit register.
     * 7 |  1  |        always 1
     * 6 |  L  |        LBA mode. This bit selects the mode of operation.
     * 5 |  1  |        always 1
     * 4 | DRV |        drive master is selected when drv = 0.
     *                  drive slave is selected when drv = 1
     * 3 | HS3 |        
     * 2 | HS2 |        HS0-HS3 contains bit 24-27 of the LBA.
     * 1 | HS1 |
     * 0 | HS0 |
     *  so first check if disk slave is present.
     */
    outb(0x1f6, 0xe0 | (1<<4));     // b'11110000'
    for(int i = 0; i < 1000; i++)
    {
        if(inb(0x1f7) != 0) {
            hasdisk1 = 1;
            break;
        }
    }
    outb(0x1f6, 0xe0 | (0 << 4));   // b'11100000'
    extern void ideintr();
    register_handler(46, ideintr);
}

static void idestart(struct buf *b)
{
    idewait(0);
    outb(0x3f6, 0);     // generate interrupt
    outb(0x1f2, 2);     // operate a sector
    uint32_t phsect = b->sector * 2;
    outb(0x1f3, phsect & 0xff);
    outb(0x1f4, (phsect >> 8) & 0xff);
    outb(0x1f5, (phsect >> 16) & 0xff);
    outb(0x1f6, 0xe0 | (b->dev << 4) | ((phsect >> 24) & 0x0f));
    if(b->flags & B_DIRTY)
    {
        outb(0x1f7, IDE_CMD_WRITE);
        outsl(0x1f0, b->data, 512/4);
    }
    else
    {
        outb(0x1f7, IDE_CMD_READ);
    }

}

void ideintr()
{
    static int first = 1;
    struct buf *b;
    if((b = idequeue) == NULL) {
        return;
    }
    if(!(b->flags & B_DIRTY) && idewait(1) >= 0)
    {
        if(first) {
            insl(0x1f0, b->data, 512/4);
            first = 0;
            return;
        }
        else {
            insl(0x1f0, b->data+512, 512/4);
//            for(int i = 0; i < 512;i++)
//                printk("%x", b->data[i]);
        }
    } 
    else if(b->flags & B_DIRTY)
    {
        if(first) {
            outsl(0x1f0, b->data+512, 512/4);
            first = 0;
        }
    }
    idequeue = b->qnext;
    b->flags |= B_VALID;
    b->flags &= ~B_DIRTY;
    wakeup(b);
    first = 1;
    if(idequeue != NULL)
        idestart(idequeue);
}

void iderw(struct buf *b)
{   
    // buf not busy;
    // TODO
    if(!(b->flags & B_BUSY))    
        return;
    // if B_VAILD is set, this means the block has contained
    // disk content, so nothing to do. TODO
    if((b->flags & (B_VALID|B_DIRTY)) == B_VALID)
        return;
    // if select disk1 but does not have disk1,it must be
    // a mistake TODO
    if(b->dev != 0 && !hasdisk1)
        return;
    cli();
    // append b to idequeue
    b->qnext = NULL;
    struct buf *p;
    for(p = idequeue; p && p->qnext; p = p->qnext)
        ;
    // if idequeue has only one block,start disk
    if(!p) {
        idequeue = b;
        idestart(b);
    }
    else {
        p->qnext = b;
    }
    // wait for request to finish;
    while((b->flags & (B_VALID|B_DIRTY)) != B_VALID) {
        sleep(b, 1);
    }
    sti();
}
