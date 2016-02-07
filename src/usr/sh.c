#include "file.h"
#include "usysc.h"
#include "exec.h"
#include "dirent.h"
#include "string.h"
#include "stdio.h"

int split_argv(char *cmd);
int cd(char *path);

char *argv[MAXARGV];
char workdir[NAMELEN] = "/";
int main()
{
    char cmd[256];
    for(;;)
    {
        printf("%s $ ", workdir);
        read(0, cmd, 256);
        if(split_argv(cmd) < 0)
            continue;
        if(strcmp(cmd, "cd") == 0) {
            cd(argv[1]);
            continue;
        }
        if(strcmp(cmd, "exit") == 0)
            exit();
        
        int pid = fork();
        if(pid < 0 ) {
            printf("sh: fork failed!\n");
            for(;;);
        }
        if(pid == 0) {
            if(exec(cmd, argv) == -1) {
                printf("sh: execute %s failed, proc %d exited!\n", cmd, getpid());
                exit();
            }
        }
        wait();
    }
}

int split_argv(char *cmd)
{
    int i;
    if(!(*cmd))
        return -1;
    char *c = strtok(cmd, " ");
    
    for(i = 0; c; i++)
    {
        argv[i] =  c;
        c = strtok(NULL, " ");
    }
    argv[i] = NULL;
    return 0;
}

int cd(char *path)
{
    if(chdir(path) < 0)
        printf("cd: no such file or directory: %s\n", argv[1]);
    strcpy(workdir, path);
}
