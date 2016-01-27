[section .text]

%macro ISR_NOERRCODE 1
[global isr%1]              ; 这里很像jinja2里的拦截器的感觉啊
isr%1:
    cli
    push 0                  ; 为了与有错误码的中断相对应
    push %1
    jmp isr_common_stub     ; 无需return,中断处理程序退出之后直接刷ip即可回到正常流程中
%endmacro

%macro ISR_ERRCODE 1
[global isr%1]
isr%1:
    push %1                 ; CPU在进入入口函数之前已经保存了错误码,这里不用再保存
    jmp isr_common_stub     
%endmacro

ISR_NOERRCODE 0 ; 0 #DE 除 0 异常
ISR_NOERRCODE 1 ; 1 #DB 调试异常
ISR_NOERRCODE 2 ; 2 NMI
ISR_NOERRCODE 3 ; 3 BP 断点异常
ISR_NOERRCODE 4 ; 4 #OF 溢出
ISR_NOERRCODE 5 ; 5 #BR 对数组的引用超出边界
ISR_NOERRCODE 6 ; 6 #UD 无效或未定义的操作码
ISR_NOERRCODE 7 ; 7 #NM 设备不可用无数学协处理器()
ISR_ERRCODE 8   ; 8 #DF 双重故障有错误代码()
ISR_NOERRCODE 9 ; 9 协处理器跨段操作
ISR_ERRCODE 10  ; 10 #TS 无效TSS有错误代码()
ISR_ERRCODE 11  ; 11 #NP 段不存在有错误代码()
ISR_ERRCODE 12  ; 12 #SS 栈错误有错误代码()
ISR_ERRCODE 13  ; 13 #GP 常规保护有错误代码()
ISR_ERRCODE 14  ; 14 #PF 页故障有错误代码()
ISR_NOERRCODE 15 ; 15 CPU 保留
ISR_NOERRCODE 16 ; 16 #MF 浮点处理单元错误
ISR_ERRCODE 17   ; 17 #AC 对齐检查
ISR_NOERRCODE 18 ; 18 #MC 机器检查
ISR_NOERRCODE 19 ; 19 #XM SIMD单指令多数据()浮点异常
; Intel 保留
ISR_NOERRCODE 20
ISR_NOERRCODE 21
ISR_NOERRCODE 22
ISR_NOERRCODE 23
ISR_NOERRCODE 24
ISR_NOERRCODE 25
ISR_NOERRCODE 26
ISR_NOERRCODE 27
ISR_NOERRCODE 28
ISR_NOERRCODE 29
ISR_NOERRCODE 30
ISR_NOERRCODE 31

; 用来实现system call
ISR_NOERRCODE 128

%macro IRQ 2
[global irq%1]
irq%1:
    push 0
    push %2
    jmp irq_common_stub
%endmacro

IRQ 0, 32
IRQ 1, 33

[extern isr_handler]
[global isr_common_stub]
[global intrret]

isr_common_stub:
    pusha
    xor eax, eax
    mov ax, ds
    push eax

    mov ax, 0x10
    mov ds, ax
    mov es, ax
    mov fs, ax
    mov gs, ax
    mov ss, ax

    push esp        ; struct regs ptr
    call isr_handler
    add esp, 4
    jmp  intrret

[global irq_common_stub]
[extern irq_handler]
irq_common_stub:
    pusha
    xor eax, eax
    mov ax, ds
    push eax
    mov ax, 0x10
    mov ds, ax
    mov es, ax
    mov fs, ax
    mov gs, ax
    mov ss, ax
    
    push esp
    call irq_handler
    add  esp, 4
intrret:
    pop ebx
    mov ds, bx
    mov es, bx
    mov fs, bx
    mov gs, bx
    popa
    add esp, 8
    iret
