[section .text]
[global idt_flush]
idt_flush:
    mov eax, [esp + 4]
    mov byte [0xB8000], 'a'
    lidt [eax]
    ret 

