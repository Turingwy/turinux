#include "stdio.h"
#include "usysc.h"

int main(int argc, char **argv)
{
    if(argc == 1) 
        printf("mkdir: missing operand\n");
    else
    {
        for(int i = 1;i < argc; i++)
        {
            if(mkdir(argv[i]) < 0)
                printf("mkdir: mkdir failed: File exists\n");
        }
    }
    exit();
}
