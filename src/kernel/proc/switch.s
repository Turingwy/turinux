[global switch_to]
switch_to:
    mov eax, [esp + 4]
    mov edx, [esp + 8]
    push ebp
    push ebx
    push esi
    push edi
    mov [eax], esp
    
    mov esp, edx
    pop edi
    pop esi
    pop ebx
    pop ebp
    ret
