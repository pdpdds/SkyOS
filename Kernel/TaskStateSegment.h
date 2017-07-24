#pragma once
#include "windef.h"

#ifdef _MSC_VER
#pragma pack (push, 1)
#endif

/////////////////////////////////////////////////////////
//쓰레드 컨텍스트 스위칭시 저장해야 될 데이터들. 해당 쓰레드가 복원될 때 레지스터에 값들을 세팅해야 한다.
//CR3 페이징을 위한 페이지 디렉토리의 주소를 가리킴
//EIP Instruction Pointer의 주소
/////////////////////////////////////////////////////////

struct TaskStateSegment
{
	UINT32 PREV_LINK;
	UINT32 ESP0;
	UINT32 SS0;
	UINT32 ESP1;
	UINT32 SS1;
	UINT32 ESP2;
	UINT32 SS2;
	UINT32 CR3;
	UINT32 EIP;
	UINT32 EFLAGS;
	UINT32 EAX;
	UINT32 ECX;
	UINT32 EDX;
	UINT32 EBX;
	UINT32 ESP;
	UINT32 EBP;
	UINT32 ESI;
	UINT32 EDI;
	UINT32 ES;
	UINT32 CS;
	UINT32 SS;
	UINT32 DS;
	UINT32 FS;
	UINT32 GS;
	UINT32 LDT_SEL;
	UINT16 TRAP;
	UINT16 IOBASE;
};

#ifdef _MSC_VER
#pragma pack (pop, 1)
#endif