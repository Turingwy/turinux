#include "types.h"
#include "rwi.h"
#include "dirent.h"
#include "string.h"
#include "stat.h"
#include "proc.h"

struct inode *dirlookup(struct inode *dp, char *dirname, uint32_t *offp)
{
    if(!ISDIR(dp->mode))
        return NULL;

    struct dirent dir;
    for(int off = 0; off < dp->size; off+=sizeof(struct dirent))
    {
        if(readi(dp, (char *)&dir, off, sizeof(struct dirent)) != sizeof(struct dirent))
            return NULL;
        if(!strcmp(dir.name, dirname) && dir.ino)
        {
            if(offp)
                *offp = off;
           
            return iget(dp->dev, dir.ino);
        }
    }
    return NULL;
}

int dirlink(struct inode *dp, char *dirname, uint32_t ino)
{
    if(!ISDIR(dp->mode) || ino < 1) {
        return -1;
    }
    
    if(dirlookup(dp, dirname, 0)) 
        return -1;

    struct dirent dir;
    int off;
    for(off = 0; off < dp->size; off+=sizeof(struct dirent))
    {
        if(!readi(dp, (char *)&dir, off, sizeof(struct dirent))) {
            return -1;
        }
        // found a new entry
        if(!dir.ino)
            break;
    }
    
    dir.ino = ino;
    strcpy(dir.name, dirname);
    writei(dp, (char *)&dir, off, sizeof(struct dirent));
    return 0;
}

static struct inode *_name(char *p, int nameiparent, char *dirname)
{
    struct inode *in, *next;
    char *name;
    char arr[64];
    char *path = arr;
    strcpy(arr, p);
    if(*path == '/') {
        in = iget(0, 1);
        while(*++path == '/')
            ;
    }
    else
        in = idup(current_proc->cwd);
    name = strtok(path, "/");
    while(name)
    {
//        printk("File system search:%s\n", name);
        ilock(in);
        if(!ISDIR(in->mode)) 
        {
            iunlock(in);
            iput(in);
            return NULL;
        }
        if(nameiparent && !tok_hasnext())
        {
            iunlock(in);
            if(dirname)
                strcpy(dirname, name);
            return in;
        }
        if(!(next = dirlookup(in, name, 0))) {
            iunlock(in);
            iput(in);
            return NULL;
        }
        iunlock(in);
        iput(in);
        in = next;
        if(!tok_hasnext() && dirname)
            strcpy(dirname, name);
        name = strtok(NULL, "/");
    }
    
    return in;
}

struct inode *namei(char *path, char *name)
{
    return _name(path, 0, name);
}

struct inode *namepi(char *path, char *name)
{
    return _name(path, 1, name);
}
