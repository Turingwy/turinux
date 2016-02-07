#include "file.h"

__attribute__((section(".initcode"))) void aaa() {

    char path[] = "/dev/tty";
    // sys_fork
//    int eax;
//    asm volatile ("movl %%eax, %0":"=r"(eax));
//    if(eax != 0)
//        asm volatile("push $0\nmovl %0, %%eax\nint $128" :: "r"(2));
    // sys_puts
//    else
//        asm volatile("push %0\nmovl %0, %%eax\nint $128" :: "r"(1));


    asm volatile ("push %0\npush %1\npush $0\n"::"r"(O_RDWR),"r"(path));
    asm volatile ("movl %0, %%eax\nint $128"::"r"(7));
    asm volatile ("movl %0, %%eax\nint $128"::"r"(13));
    for(;;);
}
