; grub规范
MBOOT_HEADER_MAGIC  equ 0x1BADB002

MBOOT_PAGE_ALIGN    equ 1

MBOOT_MEM_INFO      equ 2

MBOOT_HEADER_FLAGS  equ MBOOT_PAGE_ALIGN | MBOOT_MEM_INFO

MBOOT_CHECKSUM      equ (-MBOOT_HEADER_MAGIC-MBOOT_HEADER_FLAGS)

[bits 32]
[section .init.text]

dd MBOOT_HEADER_MAGIC
dd MBOOT_HEADER_FLAGS
dd MBOOT_CHECKSUM

[global start]
[global mboot_ptr_tmp]
[extern kern_entry]
start:
    cli
    mov esp, stack_end - 4
    mov ebp, esp
    mov [mboot_ptr_tmp], ebx
    call kern_entry
end:
    hlt
    jmp end

[global page_flush]
page_flush:
    mov eax, [esp+4]
    mov cr3, eax
    mov eax, cr0
    or eax, 0x80000000
    mov cr0, eax
    mov eax, [esp + 8]
    mov ebx, [esp]
    mov esp, eax
    sub eax, 4
    mov ebp, eax
    push ebx
    ret


[section .init.data]
mboot_ptr_tmp:
    resb 4
stack:
    times 4096 db 0
stack_end:

