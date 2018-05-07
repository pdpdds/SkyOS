#include "kmain.h"

extern "C" void main64();
extern void __cdecl InitializeConstructors();

#pragma pack(push, 1)
typedef struct _Charactor_Struct
{
	BYTE bCharactor;
	BYTE bAttribute;

} CHARACTER_MEMORY;
#pragma pack(pop)

void _PrintStringXY()
{
	ushort* Address = (ushort*)(0xB8000);
	int i = 0;	
	return;

	for (i = 0; i < 1; i++)
	{
		//Address[i].bCharactor = (BYTE)p[i];
		//Address[i].bAttribute = 0x0A;

		//*((ushort*)Address) = (_str[i] | (ushort)(((uchar)((0 << 4) | (15 & 0xF))) << 8));
		//Address += 2;
		*(Address) = ('T' | (ushort)(((uchar)((0 << 4) | (15 & 0xF))) << 8));
		//Address += (ushort)2;
	}

	for (;;);
}
char afff = 'I';
char* afffs = "marry";
void _PrintStringXY2(char* str)
{
	char test = afffs[0];

	if(test == 0)
		*((ushort*)0xB8008) = ('L' | (ushort)(((uchar)((0 << 4) | (15 & 0xF))) << 8));
	else
	*((ushort*)0xB8008) = (test | (ushort)(((uchar)((0 << 4) | (15 & 0xF))) << 8));
}

//void kmain64(unsigned long magic, unsigned long addr)
void kmain64()
{
	InitializeConstructors();
	afffs[0] = 'm';
	int skyos64 = 0x0000000;
	char* aaa = (char*)skyos64;

	if((QWORD)kmain64 == 0x00000204)
		for (;;);

	for (int i = 0; i < 0x00300000; i++)
	{
		if (aaa[i] == 'm')
			if (aaa[i + 1] == 'a')
				if (aaa[i + 2] == 'r')
				{
					*((ushort*)0xB8002) = ('G' | (ushort)(((uchar)((0 << 4) | (15 & 0xF))) << 8));
					for (;;);
				}
	}

	*((ushort*)0xB8000) = (afff | (ushort)(((uchar)((0 << 4) | (15 & 0xF)) ) << 8));
	*((ushort*)0xB8002) = ('A' | (ushort)(((uchar)((0 << 4) | (15 & 0xF))) << 8));
	
	//SkyConsole::Initialize();

	//SkyConsole::Print("64Bit Kernel Entered..\n");
	
	*((ushort*)0xB8004) = ('9' | (ushort)(((uchar)((0 << 4) | (15 & 0xF))) << 8));
	*((ushort*)0xB8006) = ('8' | (ushort)(((uchar)((0 << 4) | (15 & 0xF))) << 8));
	_PrintStringXY2("DSFSD");
	for (;;);
}

