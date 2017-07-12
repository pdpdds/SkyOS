/*   _emit is DB equivalent but not DD equivalent exist
so we define it ourself */
#define dd(x)                            \
        __asm _emit     (x)       & 0xff \
        __asm _emit     (x) >> 8  & 0xff \
        __asm _emit     (x) >> 16 & 0xff \
        __asm _emit     (x) >> 24 & 0xff

#define KERNEL_STACK         0x00104000

/*  This is the one of most important thing to be able to load a PE kernel
with GRUB. Because PE header are in the begining of the file, code section
will be shifted. The value used to shift the code section is the linker
align option /ALIGN:value. Note the header size sum is larger than 512,
so ALIGN value must be greater */
#define   ALIGN               0x400

/*   Must be >= 1Mo for GRUB
Base adress from advanced libker option
*/
#define LOADBASE            0x100000


#define   HEADER_ADRESS         LOADBASE+ALIGN

#define MULTIBOOT_HEADER_MAGIC         0x1BADB002
#define MULTIBOOT_BOOTLOADER_MAGIC      0x2BADB002
#define MULTIBOOT_HEADER_FLAGS         0x00010003 
#define STACK_SIZE              0x4000    
#define CHECKSUM            -(MULTIBOOT_HEADER_MAGIC + MULTIBOOT_HEADER_FLAGS)



void main(unsigned long, unsigned long);