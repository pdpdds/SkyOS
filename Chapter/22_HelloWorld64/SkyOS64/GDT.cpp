#include "GDT.h"
#include "memory.h"

/*
    GDT와 TSS를 초기화 합니다.
    32비트 코드에서 사용했던 GDT의 C언어 버전이라고 생각해도 무관.
    0x142000 는 1MB영역에서 페이지 자료구조 264KB를 더한 위치
    
    0x142000 |GDTR |
    0x142010 | GDT |
             |     |
    0x142038 | TSS |
             |     |
    0x1420A0 | IDTR|
    0x1420B0 | IDT |
             |     |
    각 디스크립터 별 위치.
    구체적인건 
    https://devsdk.github.io/development/2017/07/10/ReadyInterrupt.html

*/
void InitializeGDTWithTSS()
{
    GDTR* _gdtr = (GDTR*)GDTR_POINTER;

    GDT_ENTRY8* _gdt_entry      = (GDT_ENTRY8*)(GDTR_POINTER + sizeof(GDTR)); 
    TSS_SEGMENT* _tss_segment    = (TSS_SEGMENT*)((QWORD)_gdt_entry+GDT_TABLE_SIZE);    
    _gdtr->Size         = GDT_TABLE_SIZE - 1;
    _gdtr->BaseAddress  = (QWORD)_gdt_entry;

    SetGDT_Entry8((&_gdt_entry[0]), 0, 0, 0, 0, 0);
    SetGDT_Entry8((&_gdt_entry[1]),0,0xFFFFF, GDT_ENTRY_HIGH_CODE, GDT_ENTRY_LOW_KERNEL_CODE, GDT_TYPE_CODE);
    SetGDT_Entry8((&_gdt_entry[2]),0,0xFFFFF, GDT_ENTRY_HIGH_DATA, GDT_ENTRY_LOW_KERNEL_DATA, GDT_TYPE_DATA);
    //TSS 초기화 ( 104 바이트 )
    //SetGDT_Entry16(((GDT_ENTRY16*)(&_gdt_entry[3])), (QWORD)_tss_segment, sizeof(TSS_SEGMENT)-1, GDT_ENTRY_HIGH_TSS,
                  //  GDT_ENTRY_LOW_TSS, GDT_TYPE_TSS);

   
    
}
void SetGDT_Entry8(GDT_ENTRY8* _entry, DWORD _BaseAddress,
                     DWORD _Size, BYTE _HighFlags, BYTE _LowFlags, BYTE _Type)
{
    _entry->Low_Size            =   _Size & 0xFFFF;
    _entry->Low_BaseAddress     = _BaseAddress & 0xFFFF;
    _entry->Low_BaseAddress1    = ( _BaseAddress >> 16 ) & 0xFF;
    _entry->Low_Flags           = _LowFlags | _Type;
    _entry->High_FlagsAndSize   = ((_Size>>16) & 0x0F) | _HighFlags;
    _entry->High_BaseAddress    = (_BaseAddress>>24) & 0xFF;
}

void SetGDT_Entry16(GDT_ENTRY16* _entry, QWORD _BaseAddress,
                     DWORD _Size, BYTE _HighFlags, BYTE _LowFlags, BYTE _Type)
{
    _entry->Low_Size            = _Size & 0xFFFF;
    _entry->Low_BaseAddress     = _BaseAddress & 0xFFFF;
    _entry->Mid_BaseAddress     = (_BaseAddress >> 16 ) & 0xFF;
    _entry->Low_Flags           = _LowFlags | _Type;
    _entry->High_FlagsAndSize   = ((_Size >> 16) & 0x0F) | _HighFlags;
    _entry->High_BaseAddress    = (_BaseAddress  >> 24) & 0xFF;
    _entry->High_BaseAddress2   = (_BaseAddress>>32);
    _entry->Reserved            = 0;
}
