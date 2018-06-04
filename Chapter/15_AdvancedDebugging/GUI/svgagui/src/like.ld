ENTRY (_start)

SECTIONS
{
    . = 0x00100000;

    .text :
    {
        *(.text)
    }

    .rodata ALIGN (0x1000) :
    {
        *(.rodata)
    }

    .data ALIGN (0x1000) :
    {
        *(.data)
    }

    .bss :
    {
        _sbss = .;
        *(COMMON)
        *(.bss)
        _ebss = .;
    }
}