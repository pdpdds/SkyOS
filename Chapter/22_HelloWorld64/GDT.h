#pragma once
#include <windef.h>

/*
GDT Descriptor
Load GDTR on 0x142000
*/


/*

; GDT Entry 필드
; Limit or Size[32]: 세그먼트 크기
; Base Address [20]: 세그먼트 시작 주소
; Type		    [4]: 세그먼트 타입
; S 			[1]: 디스크립터 타입
; DPL			[2]: 권한
; P				[1]: 유효 여부
; AVL			[1]: 임시
; L				[1]: IA-32e 에서 64,32 구분
; D/B			[1]: 기본 연산 크기 (0-16, 1-32)
; G				[1]: 가중치 1일시 4KB 곱함

*/


#define GDT_TYPE_CODE       0x0A    //0b00001010
#define GDT_TYPE_DATA       0x02    //0b00000010
#define GDT_TYPE_TSS        0x09    //0b00001001
//Flag Low Area
#define GDT_ENTRY_LOW_S     0x10    //0b00010000
#define GDT_ENTRY_LOW_DPL0  0x00    //0b00000000
#define GDT_ENTRY_LOW_DPL1  0x20    //0b00100000
#define GDT_ENTRY_LOW_DPL2  0x40    //0b01000000
#define GDT_ENTRY_LOW_DPL3  0x60    //0b01100000
#define GDT_ENTRY_LOW_P     0x80    //0b10000000
//Flag High Area
#define GDT_ENTRY_HIGH_L    0x20    //0b00100000
#define GDT_ENTRY_HIGH_DB   0x40    //0b01000000
#define GDT_ENTRY_HIGH_G    0x80    //0b10000000

//Kernel Code Low Area Flags
#define GDT_ENTRY_LOW_KERNEL_CODE   (GDT_TYPE_CODE |GDT_ENTRY_LOW_S| \
                                GDT_ENTRY_LOW_DPL0 | GDT_ENTRY_LOW_P)
//Kernel Data Low Area Flags
#define GDT_ENTRY_LOW_KERNEL_DATA   (GDT_TYPE_DATA |GDT_ENTRY_LOW_S| \
                                GDT_ENTRY_LOW_DPL0 | GDT_ENTRY_LOW_P)
//User Code Low Area Flags
#define GDT_ENTRY_LOW_USER_CODE   (GDT_TYPE_CODE |GDT_ENTRY_LOW_S| \
                                GDT_ENTRY_LOW_DPL3 | GDT_ENTRY_LOW_P)
//User Data Low Area Flags
#define GDT_ENTRY_LOW_USER_DATA   (GDT_TYPE_DATA |GDT_ENTRY_LOW_S| \
                                GDT_ENTRY_LOW_DPL3 | GDT_ENTRY_LOW_P)


#define GDT_ENTRY_HIGH_CODE (GDT_ENTRY_HIGH_G | GDT_ENTRY_HIGH_L)
#define GDT_ENTRY_HIGH_DATA (GDT_ENTRY_HIGH_G | GDT_ENTRY_HIGH_L)

#define GDT_ENTRY_LOW_TSS   (GDT_ENTRY_LOW_DPL0 | GDT_ENTRY_LOW_P)
#define GDT_ENTRY_HIGH_TSS  (GDT_ENTRY_HIGH_G)

#define GDT_KERNEL_CODE_SEGMENT 0x08
#define GDT_KERNEL_DATA_SEGMENT 0x10
#define GDT_TSS_SEGMENT         0x18

#define GDTR_POINTER            0x142000    //1MB + 264KB

#define GDT_MAX_ENTRY8_COUNT    3
#define GDT_MAX_ENTRY16COUNT    0  

#define GDT_TABLE_SIZE          ((sizeof(GDT_ENTRY8) * GDT_MAX_ENTRY8_COUNT)\
                                  + (sizeof(GDT_ENTRY16) * GDT_MAX_ENTRY16COUNT)) 

#define TSS_SEGMENT_SIZE        (sizeof(TSS_SEGMENT))

#pragma pack(push, 1)

typedef struct _Struct_DTR_Struct
{
    WORD Size;              //2Byte
    DWORD BaseAddress;      //8Byte
    WORD _WORD_PADDING;     //2Byte
    DWORD _DWORD_PADDING;   //4Byte
                            //=16Byte .
    
}GDTR, IDTR;

typedef struct _Struct_GDT_Entry8
{
    WORD Low_Size;          //2Byte
    WORD Low_BaseAddress;   //2Byte

    BYTE Low_BaseAddress1;  //1Byte
    BYTE Low_Flags;         //1Byte
    BYTE High_FlagsAndSize; //1Byte
    BYTE High_BaseAddress;  //1Byte
}GDT_ENTRY8;

typedef struct tag_GDTR_STRUCT
{
	GDTR _gdtr;
	GDT_ENTRY8 _gdt[3];
}GDTR_STRUCT;


typedef struct _Struct_GDT_Entry16
{
    WORD Low_Size;
    WORD Low_BaseAddress;

    BYTE Mid_BaseAddress;
    BYTE Low_Flags;
    BYTE High_FlagsAndSize;
    BYTE High_BaseAddress;

    DWORD High_BaseAddress2;
    DWORD Reserved;

}GDT_ENTRY16;


//104Byte
typedef struct _Struct_TSS_Segment
{
    DWORD Reserved;     //4Byte
    QWORD RSP[3];       //8 * 3 = 24Byte
    QWORD Reserved2;    //8Byte
    QWORD IST[7];       //8 * 7 = 56Byte
    QWORD Reserved3;    //8Byte
    WORD  Reserved4;    //2Byte
    WORD  IOMapBaseAddress; //2Byte
}TSS_SEGMENT;


#pragma pack(pop)


void InitializeGDT();

void SetGDT_Entry8(GDT_ENTRY8* _entry, DWORD _BaseAddress,
                     DWORD _Size, BYTE _HighFlags, BYTE _LowFlags, BYTE _Type);
