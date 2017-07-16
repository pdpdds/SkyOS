#include "SkyGDT.h"
#include "SkyString.h"

#ifdef _MSC_VER
#pragma pack (push, 1)
#endif

//! processor gdtr register points to base of gdt. This helps
//! us set up the pointer
struct gdtr {

	//! size of gdt
	uint16_t		m_limit;

	//! base address of gdt
	uint32_t		m_base;
};

#ifdef _MSC_VER
#pragma pack (pop, 1)
#endif

//============================================================================
//    IMPLEMENTATION REQUIRED EXTERNAL REFERENCES (AVOID)
//============================================================================
//============================================================================
//    IMPLEMENTATION PRIVATE DATA
//============================================================================

//! global descriptor table is an array of descriptors
static struct gdt_descriptor	_gdt[MAX_DESCRIPTORS];

//! gdtr data
static struct gdtr				_gdtr;

//============================================================================
//    INTERFACE DATA
//============================================================================
//============================================================================
//    IMPLEMENTATION PRIVATE FUNCTION PROTOTYPES
//============================================================================

//! install gdtr
static void gdt_install();

//============================================================================
//    IMPLEMENTATION PRIVATE FUNCTIONS
//============================================================================

//! install gdtr
static void gdt_install() {
#ifdef _MSC_VER
	_asm lgdt[_gdtr]
#endif
}

//! Setup a descriptor in the Global Descriptor Table
void SetGDTDescriptor(uint32_t i, uint64_t base, uint64_t limit, uint8_t access, uint8_t grand)
{
	if (i > MAX_DESCRIPTORS)
		return;

	//! null out the descriptor
	memset((void*)&_gdt[i], 0, sizeof(gdt_descriptor));

	//! set limit and base addresses
	_gdt[i].baseLo = uint16_t(base & 0xffff);
	_gdt[i].baseMid = uint8_t((base >> 16) & 0xff);
	_gdt[i].baseHi = uint8_t((base >> 24) & 0xff);
	_gdt[i].limit = uint16_t(limit & 0xffff);

	//! set flags and grandularity bytes
	_gdt[i].flags = access;
	_gdt[i].grand = uint8_t((limit >> 16) & 0x0f);
	_gdt[i].grand |= grand & 0xf0;
}


//! returns descriptor in gdt
gdt_descriptor* GetGDTDescriptor(int i) {

	if (i > MAX_DESCRIPTORS)
		return 0;

	return &_gdt[i];
}


//! initialize gdt
bool InitializeGDT() {

	//! set up gdtr
	_gdtr.m_limit = (sizeof(struct gdt_descriptor) * MAX_DESCRIPTORS) - 1;
	_gdtr.m_base = (uint32_t)&_gdt[0];

	//! set null descriptor
	SetGDTDescriptor(0, 0, 0, 0, 0);

	//! set default code descriptor
	SetGDTDescriptor(1, 0, 0xffffffff,
		I86_GDT_DESC_READWRITE | I86_GDT_DESC_EXEC_CODE | I86_GDT_DESC_CODEDATA | I86_GDT_DESC_MEMORY,
		I86_GDT_GRAND_4K | I86_GDT_GRAND_32BIT | I86_GDT_GRAND_LIMITHI_MASK);

	//! set default data descriptor
	SetGDTDescriptor(2, 0, 0xffffffff,
		I86_GDT_DESC_READWRITE | I86_GDT_DESC_CODEDATA | I86_GDT_DESC_MEMORY,
		I86_GDT_GRAND_4K | I86_GDT_GRAND_32BIT | I86_GDT_GRAND_LIMITHI_MASK);

	//! set default user mode code descriptor
	SetGDTDescriptor(3, 0, 0xffffffff,
		I86_GDT_DESC_READWRITE | I86_GDT_DESC_EXEC_CODE | I86_GDT_DESC_CODEDATA | I86_GDT_DESC_MEMORY | I86_GDT_DESC_DPL,
		I86_GDT_GRAND_4K | I86_GDT_GRAND_32BIT | I86_GDT_GRAND_LIMITHI_MASK);

	//! set default user mode data descriptor
	SetGDTDescriptor(4, 0, 0xffffffff,
		I86_GDT_DESC_READWRITE | I86_GDT_DESC_CODEDATA | I86_GDT_DESC_MEMORY | I86_GDT_DESC_DPL,
		I86_GDT_GRAND_4K | I86_GDT_GRAND_32BIT | I86_GDT_GRAND_LIMITHI_MASK);

	//! install gdtr
	gdt_install();

	return true;
}
