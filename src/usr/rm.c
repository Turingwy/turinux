#include "stdio.h"
#include "usysc.h"

int main(int argc, char **argv)
{
    if(argc == 1)
        printf("rm: missing operand\n");
    else
    {
        if(unlink(argv[1])< 0)
            printf("rm: rm failed\n");
    }
    exit();
}
