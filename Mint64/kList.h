#pragma once
#include "windef.h"

////////////////////////////////////////////////////////////////////////////////
//
// 구조체
//
////////////////////////////////////////////////////////////////////////////////
// 1바이트로 정렬
#pragma pack( push, 1 )

// 데이터를 연결하는 자료구조
// 반드시 데이터의 가장 앞부분에 위치해야 함
typedef struct kListLinkStruct
{
    // 다음 데이터의 어드레스와 데이터를 구분하기 위한 ID
    void* pvNext;
    DWORD qwID;
} LISTLINK;

/*
// 리스트에 사용할 데이터를 정의하는 예
// 반드시 가장 앞부분은 LISTLINK로 시작해야 함
struct kListItemExampleStruct
{
    // 리스트를 연결하는 자료구조
    LISTLINK stLink;
    
    // 데이터들
    int iData1;
    char cData2;
};
*/

// 리스트를 관리하는 자료구조
typedef struct kListManagerStruct
{
    // 리스트 데이터의 수
    int iItemCount;

    // 리스트의 첫 번째와 마지막 데이터의 어드레스
    void* pvHeader;
    void* pvTail;
} LIST;

#pragma pack( pop )


////////////////////////////////////////////////////////////////////////////////
//
// 함수
//
////////////////////////////////////////////////////////////////////////////////
void kInitializeList( LIST* pstList );
int kGetListCount( const LIST* pstList );
void kAddListToTail( LIST* pstList, void* pvItem );
void kAddListToHeader( LIST* pstList, void* pvItem );
void* kRemoveList( LIST* pstList, DWORD qwID );
void* kRemoveListFromHeader( LIST* pstList );
void* kRemoveListFromTail( LIST* pstList );
void* kFindList( const LIST* pstList, DWORD qwID );
void* kGetHeaderFromList( const LIST* pstList );
void* kGetTailFromList( const LIST* pstList );
void* kGetNextFromList( const LIST* pstList, void* pstCurrent );
