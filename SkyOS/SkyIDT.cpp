#include "SkyIDT.h"
#include "SkyString.h"
#include "DebugDisplay.h"

#ifdef _MSC_VER
#pragma pack (push, 1)
#endif

//! describes the structure for the processors idtr register
struct idtr {

	//! size of the interrupt descriptor table (idt)
	uint16_t		limit;

	//! base address of idt
	uint32_t		base;
};

#ifdef _MSC_VER
#pragma pack (pop, 1)
#endif

//! interrupt descriptor table
static struct idt_descriptor	_idt[I86_MAX_INTERRUPTS];

//! idtr structure used to help define the cpu's idtr register
static struct idtr				_idtr;

//============================================================================
//    INTERFACE DATA
//============================================================================
//============================================================================
//    IMPLEMENTATION PRIVATE FUNCTION PROTOTYPES
//============================================================================

//! installs idtr into processors idtr register
static void idt_install();

//! default int handler used to catch unregistered interrupts
static void i86_default_handler();

//============================================================================
//    IMPLEMENTATION PRIVATE FUNCTIONS
//============================================================================

//! installs idtr into processors idtr register
static void idt_install() {
#ifdef _MSC_VER
	_asm lidt[_idtr]
#endif
}


//! default handler to catch unhandled system interrupts.
static void i86_default_handler() {

#ifdef _MSC_VER
	_asm cli
#endif


	DebugClrScr(0x18);
	DebugGotoXY(0, 0);
	DebugSetColor(0x1e);
	DebugPrintf("*** [i86 Hal] i86_default_handler: Unhandled Exception");

	for (;;);
}

//============================================================================
//    INTERFACE FUNCTIONS
//============================================================================

//! returns interrupt descriptor
idt_descriptor* GetInterrputDescriptor(uint32_t i) {

	if (i>I86_MAX_INTERRUPTS)
		return 0;

	return &_idt[i];
}

//! installs a new interrupt handler
bool InstallIDTInterrupt(uint32_t i, uint16_t flags, uint16_t sel, I86_IRQ_HANDLER irq) {

	if (i>I86_MAX_INTERRUPTS)
		return false;

	if (!irq)
		return false;

	//! get base address of interrupt handler
	uint64_t		uiBase = (uint64_t)&(*irq);

	//memset((void*)&_idt[i], 0, sizeof(idt_descriptor));

	if ((flags & 0x0500) == 0x0500) {
		_idt[i].sel = sel;
		_idt[i].flags = uint8_t(flags);		
	}
	else
	{
		//! store base address into idt
		_idt[i].baseLo = uint16_t(uiBase & 0xffff);
		_idt[i].baseHi = uint16_t((uiBase >> 16) & 0xffff);
		_idt[i].reserved = 0;
		_idt[i].flags = uint8_t(flags);
		_idt[i].sel = sel;
	}

	return	true;
}

bool InitializeIDT(uint16_t codeSel) {

	//! set up idtr for processor
	_idtr.limit = sizeof(struct idt_descriptor) * I86_MAX_INTERRUPTS - 1;
	_idtr.base = (uint32_t)&_idt[0];

	//! null out the idt
	memset((void*)&_idt[0], 0, sizeof(idt_descriptor) * I86_MAX_INTERRUPTS - 1);

	//! register default handlers
	for (int i = 0; i < I86_MAX_INTERRUPTS; i++)
	{
		if (false == InstallIDTInterrupt(i, I86_IDT_DESC_PRESENT | I86_IDT_DESC_BIT32, codeSel, (I86_IRQ_HANDLER)i86_default_handler))
			return false;
	}

	//! install our idt
	idt_install();

	return true;
}