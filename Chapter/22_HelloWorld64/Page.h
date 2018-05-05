#ifndef __PAGE_H__
#define __PAGE_H__

#include "windef.h"


/*
	페이지 엔트리 구조
	PML4, PDP 엔트리
	63		:EXB
	62 ~ 52 : Avail
	51 ~ 40 : 예약됨
	39 ~ 12 : PML4 기준 주소
	11 ~ 9  : Avail
	8  ~ 6  : 예약됨
	5		: A
	4		: PCD
	3		: PWT
	2		: P/S
	1		: R/W
	0		: P
	
	PD 엔트리
	63		: EXB
	63 ~  52: Avail
	61 ~  40: 예약됨
	39 ~  21: 페이지 기준 주소
	20 ~  13: 예약됨
	12		: PAT
	11 ~  9 : Avail
	8		: G
	7		: PS1
	6		: D
	5		: A
	4		: PCD
	3		: PWT
	2		: U/S
	1		: R/W
	0		: P

	EXB: 실행 가능여부 (1이면 데이터 전용), Avail: OS가 임시 사용가능
	기준주소: 다음 레벨 또는 페이지의 기준주소
	A:접근여부, PCD: 케시 활성화 여부, PWT: 케시 정책, U/S: 페이지 권한
	R/W: 읽기 쓰기 정책(읽기전용 0), P: 엔트리의 유효여부, PAT PAT선택에 사용되는 비트
	G: CR3 레지스터 변경시 페이지 테이블 케시 PLB에서 교체 안함(CR4에서 PGE가 1일때 유효)
	PS: 페이지 크기(1 2MB, 0 4KB)
	D: 쓰기 수행 여부
*/


#define PAGE_FLAG_P			0x00000001
#define PAGE_FLAG_RW		0x00000002
#define PAGE_FLAG_US		0x00000004
#define PAGE_FLAG_PWT		0x00000008
#define PAGE_FLAG_PCD		0x00000010
#define PAGE_FLAG_A			0x00000020
#define PAGE_FLAG_D			0x00000040
#define PAGE_FLAG_PS		0x00000080
#define PAGE_FLAG_G			0x00000100
#define PAGE_FLAG_PAT		0x00001000	
#define PAGE_FLAG_EXB		0x80000000	

#define PAGE_FLAG_DEFAULT	(PAGE_FLAG_P | PAGE_FLAG_RW)
#define PAGE_TABLE_SIZE		0x1000 //4KB
#define PAGE_DEFAULT_SIZE	0X200000

#define PAGE_MAX_ENTRY_COUNT		512

#pragma(push, 1)




/*
	IA-32e 에서 메모리 주소 필드
	63			48 47	39 38			   30 29	   21 20			0
	|SIGNEXTENSION| PML4 | DIRECTORY POINTER | DIRCTORY  |    OFFSET     |
	

	PML4는 CR3레지스터에 지정된 PML4 테이블에서 PML4엔트리를 참조합니다.
	그리고,그 엔트리는 디렉토리 포인터 테이블의 시작주소를 가지고 있다. 
	그 디렉토리 포인터 테이블에서 디렉토리 포인터를 이용헤 엔트리를 뽑아낸다.
	그리고 그 엔트리는 디렉토리 테이블의 시작 주소를 가지고 있고,디렉토리를 이용해
	디렉토리 테이블에서 엔트리를 뽑아낸다. 그 엔트리는 물리주소 페이지 시작 지점을 가지고 있고, 
	거기서 Offset을 이용해 데이터를 저장한다.
	
	So, We Need Space for this Structure
	
	PML4 Table Need 512 * 8 Byte = 4KB
	PAGE DIRECTORY POINTER Table Need 512 * 8 Byte = 4KB
	PAGE DIRECTORY Need 512 * 8 Byte * 64 = 256KB (for 64GB Memory)
	
	So we using 4KB + 4KB+ 256KB = 264KB Memory Space.

	Following Code Implementation
	
	We Use 4 Level Paging, So NOT USE PTENTRY.
*/

typedef struct __Struct_PageEntry
{		
	/*0----------------31 bit */
	DWORD dwLowAddress;
	/*32---------------64 bit */
	DWORD dwHighAddress;

} PML4ENTRY, PDPTENTRY, PDENTRY, PTENTRY;	

#pragma(pop)

void InitializePageTable();
void SetPageEntryData(PTENTRY* pEntry, DWORD dwHighBaseAddress, 
					  DWORD dwLowBaseAddress, DWORD dwLowFlag, DWORD dwHighFlag);


	
#endif /*__PAGE_H__ */
