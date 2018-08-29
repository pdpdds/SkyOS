#include "gdt.h"
#include "string.h"
#include "memory.h"
#include "windef.h"
#include "defines.h"

#ifdef _MSC_VER
#pragma pack (push, 1)
#endif

typedef struct tag_gdtr {
	USHORT m_limit; // GDT의 크기
	UINT m_base; // GDT의 시작 주소
}gdtr;

#ifdef _MSC_VER
#pragma pack (pop, 1)
#endif

//! global descriptor table is an array of descriptors
static struct gdt_descriptor	_gdt [MAX_DESCRIPTORS];

//! gdtr data
static gdtr				_gdtr;


//! install gdtr
static void InstallGDT () {
#ifdef _MSC_VER
	_asm lgdt [_gdtr]
#endif
}

//! Setup a descriptor in the Global Descriptor Table
void gdt_set_descriptor(uint32_t i, uint64_t base, uint64_t limit, uint8_t access, uint8_t grand)
{
	if (i > MAX_DESCRIPTORS)
		return;

	//! null out the descriptor
	memset ((void*)&_gdt[i], 0, sizeof (gdt_descriptor));

	//! set limit and base addresses
	_gdt[i].baseLo	= uint16_t(base & 0xffff);
	_gdt[i].baseMid	= uint8_t((base >> 16) & 0xff);
	_gdt[i].baseHi	= uint8_t((base >> 24) & 0xff);
	_gdt[i].limit	= uint16_t(limit & 0xffff);

	//! set flags and grandularity bytes
	_gdt[i].flags = access;
	_gdt[i].grand = uint8_t((limit >> 16) & 0x0f);
	_gdt[i].grand |= grand & 0xf0;
}


//! returns descriptor in gdt
gdt_descriptor* i86_gdt_get_descriptor (int i) {

	if (i > MAX_DESCRIPTORS)
		return 0;

	return &_gdt[i];
}

//GDT 초기화 및 GDTR 레지스터에 GDT 로드
int GDTInitialize()
{
	//GDTR 레지스터에 로드될 _gdtr 구조체의 값 초기화
	//_gdtr 구조체의 주소는 페이징 전단계이며 실제 물리주소에 해당 변수가 할당되어 있다.
	//디스크립터의 수를 나타내는 MAX_DESCRIPTORS의 값은 5이다.
	//NULL 디스크립터, 커널 코드 디스크립터, 커널 데이터 디스크립터, 유저 코드 디스크립터
	//유저 데이터 디스크립터 이렇게 총 5개이다.
	//디스크립터당 6바이트이므로 GDT의 크기는 30바이트다.
	_gdtr.m_limit = (sizeof(struct gdt_descriptor) * MAX_DESCRIPTORS) - 1;
	_gdtr.m_base = (uint32_t)&_gdt[0];

	//NULL 디스크립터의 설정
	gdt_set_descriptor(0, 0, 0, 0, 0);

	//커널 코드 디스크립터의 설정
	gdt_set_descriptor(1, 0, 0xffffffff,
		I86_GDT_DESC_READWRITE | I86_GDT_DESC_EXEC_CODE | I86_GDT_DESC_CODEDATA |
		I86_GDT_DESC_MEMORY, I86_GDT_GRAND_4K | I86_GDT_GRAND_32BIT |
		I86_GDT_GRAND_LIMITHI_MASK);

	//커널 데이터 디스크립터의 설정
	gdt_set_descriptor(2, 0, 0xffffffff,
		I86_GDT_DESC_READWRITE | I86_GDT_DESC_CODEDATA | I86_GDT_DESC_MEMORY,
		I86_GDT_GRAND_4K | I86_GDT_GRAND_32BIT | I86_GDT_GRAND_LIMITHI_MASK);

	//유저모드 디스크립터의 설정
	gdt_set_descriptor(3, 0, 0xffffffff,
		I86_GDT_DESC_READWRITE | I86_GDT_DESC_EXEC_CODE | I86_GDT_DESC_CODEDATA |
		I86_GDT_DESC_MEMORY | I86_GDT_DESC_DPL, I86_GDT_GRAND_4K | I86_GDT_GRAND_32BIT |
		I86_GDT_GRAND_LIMITHI_MASK);

	//유저모드 데이터 디스크립터의 설정
	gdt_set_descriptor(4, 0, 0xffffffff, I86_GDT_DESC_READWRITE | I86_GDT_DESC_CODEDATA | I86_GDT_DESC_MEMORY | I86_GDT_DESC_DPL,
		I86_GDT_GRAND_4K | I86_GDT_GRAND_32BIT | I86_GDT_GRAND_LIMITHI_MASK);

	//GDTR 레지스터에 GDT 로드
	InstallGDT();

	return 0;
}
