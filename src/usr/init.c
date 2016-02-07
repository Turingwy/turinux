#include "usysc.h"
#include "stdio.h"

char buf[1024];

int main()
{
    int pid, len, fd, wpid;
    char *argv[] = {"sh", NULL};
    open("/dev/tty", O_RDWR);        // stdin
    dup(0);                         // stdout
    dup(0);                         // stderr
    fd = open("/icon", O_RDONLY);
    len = read(fd, buf, 1024);
    buf[len] = '\0';
    printf("%s\n", buf);
    close(fd);
    
    for(;;)
    {
        printf("%s", "init: start work...\n");
        pid = fork();
        if(pid < 0)
        {
            printf("init: fork failed\n");
            for(;;);
        }
        if(pid == 0)
        {
            printf("init: exec sh\n");
            exec("sh", argv);
            printf("init: execute shell failed\n");
            for(;;);
        }
        if((wpid = wait()) > 0)
            printf("init: wait proc %d\n", wpid);
    }
}
