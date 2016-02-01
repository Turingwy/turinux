#include "bio.h"
#include "fs.h"

void test()
{
    struct inode *in = iget(0, 1);
    ilock(in);
    printk("%d\n", in->flag);
    iunlock(in);
    iput(in);

}
