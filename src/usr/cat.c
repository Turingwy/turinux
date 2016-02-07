#include "types.h"
#include "usysc.h"
#include "string.h"
#include "stdio.h"

int buf[1024];

int main(int argc, char **argv)
{
    int len;
    if(argc==1)
    {
        for(;;)
        {
            len = read(1, (char *)buf, 1024);
            buf[len] = '\0';
            printf("%s\n", buf);
        } 
    }
    else
    {
        for(int i = 1; i < argc; i++)
        {
            int fd = open(argv[i], O_RDONLY);
            if(fd < 0) {
                printf("cat: %s: No such file\n", argv[i]);
                continue;
            }
            while((len = read(fd, (char *)buf, 1024)) != 0) {
                buf[len] = '\0';
                printf("%s\n", buf);
            }
        }         
    }
    exit();
}
