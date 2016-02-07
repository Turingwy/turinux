#include "stdio.h"
#include "usysc.h"
#include "dirent.h"


void read_dir(int fd)
{
    struct dirent dir;
    while(read(fd, (char *)&dir, sizeof(struct dirent)) == sizeof(struct dirent))
    {
        if(dir.ino != 0)
            printf("%s\n", dir.name);
    }
    printf("\n");
}

int main(int argc, char *argv[])
{
    int fd = 0;
    if(argc == 1)       // ls list cwd
    {
        fd = getcwd();
        if(fd < 0)
            exit();
        read_dir(fd);
    } 
    else
    {
        char *path = argv[1];
        printf("%s\n", path);
        fd = open(path, O_RDONLY);
        if(fd < 0)
            exit();
        read_dir(fd);
    }
    close(fd);
    exit();
}
