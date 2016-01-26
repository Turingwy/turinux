ENTRY(start)
SECTIONS
{
    . = 0x100000;
    PROVIDE( kernel_start = . + 0xC0000000);
    .init.text :
    {
        *(.init.text)
        . = ALIGN(4096);
    }    

    .init.data :
    {
        *(.init.data)
        . = ALIGN(4096);
    }
    . += 0xC0000000;
    .text : AT(ADDR(.text) - 0xC0000000)  
    {
        ./src/boot/boot.o(.text) *(.text)
        . = ALIGN(4096);
    }
    .data : AT(ADDR(.data) - 0xC0000000) 
    {
        *(.data)
        *(.rodata)
        . = ALIGN(4096);
    }

    .bss : AT(ADDR(.bss) - 0xC0000000)
    {
        *(.bss)
        . = ALIGN(4096);
    }
    PROVIDE (kernel_end = . );


}


