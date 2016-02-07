[global syscall]
syscall:
    pop    eax      ; return addr
    pop    edx      ; syscall_no
    push   eax
    mov    eax, edx
    int    0x80
    ret





