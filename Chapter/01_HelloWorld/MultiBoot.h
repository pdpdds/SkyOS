#pragma once
#include "stdint.h"
#include "windef.h"

/*   _emit is DB equivalent but not DD equivalent exist
so we define it ourself */
#define dd(x)                            \
        __asm _emit     (x)       & 0xff \
        __asm _emit     (x) >> 8  & 0xff \
        __asm _emit     (x) >> 16 & 0xff \
        __asm _emit     (x) >> 24 & 0xff

#define KERNEL_STACK			0x00400000
#define FREE_MEMORY_SPACE_ADDRESS       0x00400000

/*  This is the one of most important thing to be able to load a PE kernel
with GRUB. Because PE header are in the begining of the file, code section
will be shifted. The value used to shift the code section is the linker
align option /ALIGN:value. Note the header size sum is larger than 512,
so ALIGN value must be greater */
#define   ALIGN               0x400

/*   Must be >= 1Mb for GRUB
Base adress from advanced linker option
*/
#define KERNEL_LOAD_ADDRESS            0x100000


#define   HEADER_ADRESS         KERNEL_LOAD_ADDRESS+ALIGN

#define MULTIBOOT_HEADER_MAGIC         0x1BADB002
#define MULTIBOOT_BOOTLOADER_MAGIC      0x2BADB002
#define MULTIBOOT_HEADER_FLAGS         0x00010003 
#define STACK_SIZE              0x4000    
#define CHECKSUM            -(MULTIBOOT_HEADER_MAGIC + MULTIBOOT_HEADER_FLAGS)

#pragma pack(push,1)

/*

kernel_entry label 로의 jump 는 multiboot header structure 의 entry_addr 을 통해 가능
entry_addr = HEADER_ADDRESS + 0x20

HEADER_ADDRESS 는 KERNEL_LOAD_ADDRESS + ALIGN 으로 구성

- KERNEL_LOAD_ADDRESS : 0x100000 영역, Kernel 이 Load 되는 시점
- ALIGN : 0x400

+) ALIGN 이 0이 될 수 없는 이유?
PE format 을 보면 알겠지만, PE 를 기술하기 위한 Header 들이 선두에 있기 때문
그렇다 하더라도 1024 byte 위치는 file 의 처음 80K 안에 포함되기 때문에 
GRUB 는 PE file 을 scan 할 때 multiboot_entry 함수를 찾을 수 있음

여기서 중요한 것이 multiboot_entry 함수가 항상 0x100400 에 항상 배치되도록 보장

표면상으로는 어디에 있어도 상관 없는데,
80KB 내에 multi boot header structure 가 존재해야 한다는 제약 때문에 
이 함수가 파일 초반부에 나올 수 있도록 설정해 주어야 함

order.txt 파일을 통해 함수 순번을 정할 수 있음
?multiboot_entry@@YAXXZ

multiboot_entry 함수 이름을 해당 파일에 넣어두면 Visual Studio Compiler 는 해당 함수를 파일 선두에 배피
이 파일에 함수를 등록해두면, 함수들을 순차적으로 나열 가능

*/

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

struct ELFHeaderTable
{
	uint32_t num;
	uint32_t size;
	uint32_t addr;
	uint32_t shndx;
};
typedef struct multiboot_elf_section_header_table multiboot_elf_section_header_table_t;

struct AOUTSymbolTable
{
	unsigned int tabsize;
	unsigned int strsize;
	unsigned int addr;
	unsigned int reserved;
};

//Bochs 2.4 doesn't provide a ROM configuration table. Virtual PC does.
struct ROMConfigurationTable
{
	unsigned short Length;
	unsigned char Model;
	unsigned char Submodel;
	unsigned char BiosRevision;
	bool DualBus : 1;
	bool MicroChannelBus : 1;
	bool EBDAExists : 1;
	bool WaitForExternalEventSupported : 1;
	bool Reserved0 : 1;
	bool HasRTC : 1;
	bool MultipleInterruptControllers : 1;
	bool BiosUsesDMA3 : 1;
	bool Reserved1 : 1;
	bool DataStreamingSupported : 1;
	bool NonStandardKeyboard : 1;
	bool BiosControlCpu : 1;
	bool BiosGetMemoryMap : 1;
	bool BiosGetPosData : 1;
	bool BiosKeyboard : 1;
	bool DMA32Supported : 1;
	bool ImlSCSISupported : 1;
	bool ImlLoad : 1;
	bool InformationPanelInstalled : 1;
	bool SCSISupported : 1;
	bool RomToRamSupported : 1;
	bool Reserved2 : 3;
	bool ExtendedPOST : 1;
	bool MemorySplit16MB : 1;
	unsigned char Reserved3 : 1;
	unsigned char AdvancedBIOSPresence : 3;
	bool EEPROMPresent : 1;
	bool Reserved4 : 1;
	bool FlashEPROMPresent : 1;
	bool EnhancedMouseMode : 1;
	unsigned char Reserved5 : 6;
};

struct Module
{
	void *ModuleStart;
	void *ModuleEnd;
	char *Name;
	unsigned int Reserved;
};

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

/* Drive Info structure.  */
struct drive_info
{
	/* The size of this structure.  */
	unsigned long size;

	/* The BIOS drive number.  */
	unsigned char drive_number;

	/* The access mode (see below).  */
	unsigned char drive_mode;

	/* The BIOS geometry.  */
	unsigned short drive_cylinders;
	unsigned char drive_heads;
	unsigned char drive_sectors;

	/* The array of I/O ports used for the drive.  */
	unsigned short drive_ports;
};

struct APMTable
{
	unsigned short Version;
	unsigned short CS;
	unsigned int Offset;
	unsigned short CS16Bit;	//This is the 16-bit protected mode code segment
	unsigned short DS;
	unsigned short Flags;
	unsigned short CSLength;
	unsigned short CS16BitLength;
	unsigned short DSLength;
};

struct VbeInfoBlock
{
	char Signature[4];
	unsigned short Version;
	short OEMString[2];
	unsigned char Capabilities[4];
	short VideoModes[2];
	short TotalMemory;
};

/*struct VbeModeInfo
{
	unsigned short Attributes;
	unsigned char WinA;
	unsigned char WinB;
	unsigned short Granularity;
	unsigned short WinSize;
	unsigned short SegmentA;
	unsigned short SegmentB;
	unsigned short WindowFunctionPointer[2];
	unsigned short Pitch;
	unsigned short XResolution;
	unsigned short YResolution;
	unsigned char CharacterWidth;
	unsigned char CharacterHeight;
	unsigned char Planes;
	unsigned char BitsPerPixel;
	unsigned char Banks;
	unsigned char MemoryModel;
	unsigned char BankSize;
	unsigned char ImagePages;
	unsigned char Reserved;

	unsigned char RedMask;
	unsigned char RedPosition;

	unsigned char GreenMask;
	unsigned char GreenPosition;

	unsigned char BlueMask;
	unsigned char BluePosition;

	unsigned char ReservedMask;
	unsigned char ReservedPosition;

	unsigned char DirectColorAttributes;

	unsigned int FrameBuffer;
};*/

struct VbeModeInfo
{
	UINT16 ModeAttributes;
	char WinAAttributes;
	char WinBAttributes;
	UINT16 WinGranularity;
	UINT16 WinSize;
	UINT16 WinASegment;
	UINT16 WinBSegment;
	UINT32 WinFuncPtr;
	short BytesPerScanLine;
	short XRes;
	short YRes;
	char XCharSize;
	char YCharSize;
	char NumberOfPlanes;
	char BitsPerPixel;
	char NumberOfBanks;
	char MemoryModel;
	char BankSize;
	char NumberOfImagePages;
	char res1;
	char RedMaskSize;
	char RedFieldPosition;
	char GreenMaskSize;
	char GreenFieldPosition;
	char BlueMaskSize;
	char BlueFieldPosition;
	char RsvedMaskSize;
	char RsvedFieldPosition;
	char DirectColorModeInfo; //MISSED IN THIS TUTORIAL!! SEE ABOVE
							  //VBE 2.0
	UINT32 PhysBasePtr;
	UINT32 OffScreenMemOffset;
	short OffScreenMemSize;
	//VBE 2.1
	short LinbytesPerScanLine;
	char BankNumberOfImagePages;
	char LinNumberOfImagePages;
	char LinRedMaskSize;
	char LinRedFieldPosition;
	char LingreenMaskSize;
	char LinGreenFieldPosition;
	char LinBlueMaskSize;
	char LinBlueFieldPosition;
	char LinRsvdMaskSize;
	char LinRsvdFieldPosition;
	char res2[194];
};

/*

일반적으로 PC 는 이전 컴퓨터와 호환성을 위해 16 bit mode 로 booting
즉 16 bit 명령어 해석

GRUB 는 초기에 이런 BIOS call 을 통해 여러 정보를 획득한 후
system을 보호 모드로 전환

보호 모드로 전환되면 32bit address 가 가능해져서
4GB memory 영역에 접근 가능

GRUB 는 보호 모드로 전환 후 Kernel 을 호출
Kernel 입장에서는 HW 관련 정보를 얻기가 어려움
따라서 GRUB 가 Kernel 에 넘겨주는 Structure information을 활용해 정보를 알아내야 함

*/

struct multiboot_info
{
	uint32_t flags;
	// flag : flag 값을 확인해서 VESA mode 가 가능한지의 여부 파악 가능

	// BIOS 로부터 얻은 이용 가능한 메모리 영역 정보
	uint32_t mem_lower;
	uint32_t mem_upper;

	uint32_t boot_device; // booting device 번호
	char *cmdline; // kernel 영역에 넘기는 commandline

	// booting module list 
	uint32_t mods_count;
	Module *Modules;

	// linux file 과 관계된 정보
	union
	{
		AOUTSymbolTable AOUTTable;
		ELFHeaderTable ELFTable;
	} SymbolTables;

	// Memory mapping 정보
	// 이 정보를 통해 memory 특정 block 을 사용할 수 있는지 파악 가능
	uint32_t mmap_length;
	uint32_t mmap_addr;

	// 해당 PC 에 존재하는 drive 에 대한 정보
	uint32_t drives_length;
	drive_info * drives_addr;

	// ROM configuration table
	ROMConfigurationTable *ConfigTable;

	// Boot Loader 이름
	char* boot_loader_name;

	/* APM table */
	APMTable *APMTable;

	/* Video */
	VbeInfoBlock *vbe_control_info;
	VbeModeInfo *vbe_mode_info;
	uint16_t vbe_mode;
	uint16_t vbe_interface_seg;
	uint16_t vbe_interface_off;
	uint16_t vbe_interface_len;
};
typedef struct multiboot_info multiboot_info_t;



#pragma pack(pop)
