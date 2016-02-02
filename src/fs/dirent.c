#include "types.h"
#include "rwi.h"
#include "dirent.h"
#include "string.h"
#include "stat.h"

struct inode *dirlookup(struct inode *dp, char *dirname, uint32_t *offp)
{
    if(!ISDIR(dp->mode))
        return NULL;

    struct dirent dir;
    for(int off = 0; off < dp->size; off+=sizeof(struct dirent))
    {
        if(readi(dp, (char *)&dir, off, sizeof(struct dirent)) != sizeof(struct dirent))
            return NULL;
        if(!strcmp(dir.name, dirname))
        {
            *offp = off;
            return iget(dp->dev, dir.ino);
        }
    }
    return NULL;
}


