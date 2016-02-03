#include "types.h"
#include "file.h"
#include "rwi.h"
#include "fs.h"
#include "proc.h"
#include "dirent.h"
#include "stat.h"

// file table in kernel
struct file ftable[FTABLE_SZ];

// allocate a free file structure in ftable
struct file*filealloc()
{
    for(int n = 0; n < FTABLE_SZ; n++)
    {
        if(ftable[n].ref == 0) {
            ftable[n].ref = 1;
            return &ftable[n];
        }
    }
    return NULL;
}

// duplicate a file structure.
// only when f is being used.
struct file *filedup(struct file *f)
{
    if(!f->ref)
        return NULL;
    f->ref++;
    return f;
}

int fileclose(struct file *f)
{
    if(f->ref < 1)
        return 0;
    f->ref--;
    if(f->ref)
        return 1;
    
    if(f->type & FD_INODE)
        iput(f->in);
    memset(f, 0, sizeof(struct file));
    return 1;
}

// read from f to addr(kernel memory)
int fileread(struct file *f, char *addr, int len)
{
    int r;

    // f isn't readable
    if(!(f->rw & FD_READABLE))
        return -1;
    
    if(f->type & FD_INODE)
    {
        ilock(f->in);
        if((r = readi(f->in, addr, f->offset, len)) > 0)
            f->offset += r;
        iunlock(f->in);
        return r;
    }
    
    if(f->type & FD_PIPE)
    {
        ;           // TODO
    }
    return -1;
}

int filewrite(struct file *f, char *addr, int len)
{
    int r;

    if(!(f->rw & FD_WRITABLE))
        return -1;

    if(f->type & FD_INODE)
    {
        ilock(f->in);
        if(!(r = writei(f->in, addr, f->offset, len)) > 0)
            f->offset += r;
        iunlock(f->in);
        return r;
    }

    if(f->type & FD_PIPE)
    {
        ;
    }
    return -1;
}

static inline struct file *filelookup(int _fd)
{
    return _fd < FD_SIZE ? current_proc->fd[_fd]:NULL;
}


static int fdalloc(struct file *f)
{
    struct file **fs = current_proc->fd;
    for(int i = 0; i < FD_SIZE; i++)
    {
        if(fs[i])
            continue;
        fs[i] = f;
        return i;
    }

    return -1;
}

int fddup(struct file *f)
{
    int fd;
    if((fd = fdalloc(f)) == -1)
        return -1;
    filedup(f);
    return fd;
}

int read(int _fd, char *addr, int len)
{
    struct file *f = filelookup(_fd);
    if(f)
        return fileread(f, addr, len);
    return -1;
}

int write(int _fd, char *addr, int len)
{
    struct file *f = filelookup(_fd);
    if(f)
        return filewrite(f, addr, len);
    return -1;
}

int close(int _fd)
{
    struct file *f = filelookup(_fd);
    if(!f)
        return -1;
    fileclose(f);
    current_proc->fd[_fd] = 0;
}

int ilink(char *old_path, char *new_path)
{
    char name[NAMELEN];
    struct inode *oi = namei(old_path, 0);
    struct inode *npi = namepi(new_path, name);
    
    if(!oi || !npi)
        return -1;

    ilock(oi);
    if(ISDIR(oi->mode)) {
        iunlock(oi);
        iput(oi);
        iput(npi);
        return -1;
    }
    oi->nlinks++;
    int inum = oi->inum;
    iunlock(oi);

    ilock(npi);
    if(dirlink(npi, name, inum) < 0)  
    {
        iunlock(npi);
        iput(npi);
        ilock(oi);
        oi->nlinks--;
        iunlock(oi);
        iput(oi);
        return -1;
    }
    
    iunlock(npi);
    iput(npi);
    iput(oi);
    return 0;
}

int iunlink(char *path)
{
    char name[NAMELEN];
    struct inode *dp, *ip;
    if((dp = namepi(path, name)) == NULL)
        return -1;
    if(!strcmp(name, ".") || !strcmp(name, ".."))
        return -1;
        
    ilock(dp);
    int offset;
    if((ip = dirlookup(dp, name, &offset)) == NULL) 
    {
        iunlock(dp);
        return -1;    
    }

    struct dirent di;
    memset(di, 0, sizeof(struct dirent));
    writei(dp, &di, offset, sizeof(struct dirent));
    iunlock(dp);
    ilock(ip);
    ip->nlinks--;
    iunlock(ip);
    iput(ip);
    iput(dp);
    return 0;
}

int create(char *path, uint16_t mode)
{
    char name[NAMELEN];
    struct inode *dp = namepi(path, name);
    struct inode *ip;
    if(!dp)
        return NULL;
    ilock(dp);

    // open a exist file 
    if(ip = dirlookup(dp, name, 0))
    {
        iunlock(dp);
        iput(dp);
        if(ISREG(mode) && ISREG(ip->mode)) {
            iunlock(ip);
            return ip;
        }
        iunlock(ip);
        iput(ip);
        return NULL;
    }

    // create a new file inode
    int ino;
    if(!(ino = ialloc(0)))
        return NULL;

    ip = iget(0, ino);
    ilock(ip);
    ip->nlinks = 1;
    
    if(ISDIR(mode))
    {
        dp->nlinks++;       // for ..
        ip->mode = mode;
        dirlink(ip, ".", ip->inum);
        dirlink(ip, "..", dp->inum);
    }

    dirlink(dp, name, ip->inum);    
    iunlock(ip);
    iunlock(dp);
    iput(dp);
    return ip;    
}

