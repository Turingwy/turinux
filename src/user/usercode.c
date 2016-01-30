__attribute__((section(".initcode"))) void aaa() {

    char str[] = "hello\n";
    // sys_fork
//    int eax;
//    asm volatile ("movl %%eax, %0":"=r"(eax));
//    if(eax != 0)
//        asm volatile("push $0\nmovl %0, %%eax\nint $128" :: "r"(2));
    // sys_puts
//    else
//        asm volatile("push %0\nmovl %0, %%eax\nint $128" :: "r"(1));
        asm volatile ("push %0\npush $0\n"::"r"(str));
        asm volatile ("movl %0, %%eax\nint $128"::"r"(13));
         asm volatile ("movl %0, %%eax\nint $128"::"r"(13));

    for(;;);
}
