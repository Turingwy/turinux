#include "bio.h"
#include "fs.h"
#include "bio.h"
#include "buf.h"
#include "stat.h"
#include "dirent.h"
#include "file.h"
extern struct superblock sb;
void test()
{
/*    char name[100];
    struct inode *in = namepi("/entry.c", name);
    printk("%d\n", in->inum);
    printk("%s\n", name);
*/

    struct inode *in = create("/work", FDIR);
    iput(in);
/*    struct file *f = filealloc();
    f->in = in;
    f->rw = FD_READABLE;
    f->type = FD_INODE;
    char *n = kpalloc();
    fileread(f, n, 1000);
    printk("%s\n", n);
  */  

    /*char str[] = "///hel/lo/////worl/d";   
    strtok(str, "/");
    strtok(NULL, "/");
    printk(strtok(NULL, "/"));
    */
/*    struct inode *in = iget(0,1);
    ilock(in);
    uint32_t i = ialloc(0);
    printk("%d", i);
    struct inode *new_inode = iget(0, i);
    ilock(new_inode);
    new_inode->mode = FREG;
    writei(new_inode, "ni hao a!", 0, sizeof("ni hao a!"));
    dirlink(in, "nihao", i);
    iunlock(in);
    iput(in);
    iunlock(new_inode); 
   iput(new_inode);
   */
    
}
