#include "GDT.h"

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

GDTR_STRUCT g_gdtr;

void InitializeGDT()
{
    GDTR* _gdtr = (GDTR*)&g_gdtr;
    GDT_ENTRY8* _gdt_entry      = (GDT_ENTRY8*)&g_gdtr._gdt[0];
    _gdtr->Size         = GDT_TABLE_SIZE - 1;
    _gdtr->BaseAddress  = (QWORD)_gdt_entry;

    SetGDT_Entry8((&_gdt_entry[0]), 0, 0, 0, 0, 0);
    SetGDT_Entry8((&_gdt_entry[1]),0,0xFFFFF, GDT_ENTRY_HIGH_CODE, GDT_ENTRY_LOW_KERNEL_CODE, GDT_TYPE_CODE);
    SetGDT_Entry8((&_gdt_entry[2]),0,0xFFFFF, GDT_ENTRY_HIGH_DATA, GDT_ENTRY_LOW_KERNEL_DATA, GDT_TYPE_DATA);  
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