[section .text]
[global inb]
[global inw]
[global outb]
[global outw]
inb:
    push ebp
    mov ebp, esp
    push dx
    mov dx, [ebp + 8]
    xor eax, eax
    in al, dx
    pop  dx
    pop ebp
    ret

outb:
    push ebp
    mov ebp, esp
    push dx
    xor eax, eax
    mov dx, [ebp + 8]
    mov al, [ebp + 12]
    out dx, al
    pop dx
    pop ebp
    ret

inw:
    push ebp
    mov ebp, esp
    push dx
    mov dx, [ebp + 8]
    xor eax, eax
    in al, dx
    pop dx
    pop ebp
    ret
outw:
    push ebp
    mov ebp, esp
    push dx
    mov dx, [ebp + 8]
    xor eax, eax
    mov ax, [ebp + 12]
    out dx, ax
    pop dx
    pop ebp
    ret


