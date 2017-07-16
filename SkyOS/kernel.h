#pragma once
#include "StdIntTypes.h"

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

#pragma pack(push,1)
struct MULTIBOOT_HEADER {
	uint32_t magic;
	uint32_t flags;
	uint32_t checksum;
	uint32_t header_addr;
	uint32_t load_addr;
	uint32_t load_end_addr;
	uint32_t bss_end_addr;
	uint32_t entry_addr;
};

struct multiboot_elf_section_header_table
{
	uint32_t num;
	uint32_t size;
	uint32_t addr;
	uint32_t shndx;
};
typedef struct multiboot_elf_section_header_table multiboot_elf_section_header_table_t;

struct multiboot_mmap_entry
{
	uint32_t size;
	uint64_t addr;
	uint64_t len;
#define MULTIBOOT_MEMORY_AVAILABLE              1
#define MULTIBOOT_MEMORY_RESERVED               2
	uint32_t type;
};
typedef struct multiboot_mmap_entry multiboot_memory_map_t;

struct multiboot_info
{
	/* Multiboot info version number */
	uint32_t flags;

	/* Available memory from BIOS */
	uint32_t mem_lower;
	uint32_t mem_upper;

	/* "root" partition */
	uint32_t boot_device;

	/* Kernel command line */
	uint32_t cmdline;

	/* Boot-Module list */
	uint32_t mods_count;
	uint32_t mods_addr;

	multiboot_elf_section_header_table_t elf_sec;

	/* Memory Mapping buffer */
	uint32_t mmap_length;
	uint32_t mmap_addr;

	/* Drive Info buffer */
	uint32_t drives_length;
	uint32_t drives_addr;

	/* ROM configuration table */
	uint32_t config_table;

	/* Boot Loader Name */
	uint32_t boot_loader_name;

	/* APM table */
	uint32_t apm_table;

	/* Video */
	uint32_t vbe_control_info;
	uint32_t vbe_mode_info;
	uint16_t vbe_mode;
	uint16_t vbe_interface_seg;
	uint16_t vbe_interface_off;
	uint16_t vbe_interface_len;
};
typedef struct multiboot_info multiboot_info_t;
#pragma pack(pop)



void main(unsigned long, unsigned long);