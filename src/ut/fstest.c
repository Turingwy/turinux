#include "bio.h"
#include "fs.h"
#include "bio.h"
#include "buf.h"
#include "stat.h"
#include "dirent.h"

extern struct superblock sb;
void test()
{
    struct inode *in = iget(0, 1);
    ilock(in);
    int off;
    in = dirlookup(in, "hello.txt", &off);
    ilock(in);
    char n[1024];
    readi(in, n, 0, 1000);
    printk("%s", n);

}
