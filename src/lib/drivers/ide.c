#include "types.h"
#include "proc.h"
#include "idt.h"
#include "io_port.h"

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

int havedisk1;
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
    while((r = inb(0x1f7) & (IDE_BUSY|IDE_READY) != IDE_READY)
        ;

    if(checkerr && (r & (IDE_DF|IDE_ERR)!=0)
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
            havedisk1 = 1;
            break;
        }
    }

    outb(0x1f6, 0xe0 | (0 << 4));   // b'11100000'
}

static void iderequest(struct buf *b)
{
    idewait(0);
    outb(0x3f6, 0);     // generate interrupt
    outb(0x1f2, 1);     // operate a sector
    outb(0x1f3, b->sector & 0xff);
    outb(0x1f4, (b->sector >> 8) & 0xff);
    outb(0x1f5, (b->sector >> 16) & 0xff);
    outb(0x1f6, 0xe0 | (b->dev << 4) | ((b->sector >> 24) & 0xff));
    if(b->flags & B_DIRTY)
    {
        outb(0x1f7, IDE_CMD_WRITE);
        out
    }


}
