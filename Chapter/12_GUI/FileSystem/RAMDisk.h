#pragma once
#include "windef.h"
#include "MintFileSystem.h"


////////////////////////////////////////////////////////////////////////////////
//
// 매크로
//
////////////////////////////////////////////////////////////////////////////////
// 램 디스크의 총 섹터 수. 8Mbyte 크기로 생성
#define RDD_TOTALSECTORCOUNT        ( 8 * 1024 * 1024 / 512)

////////////////////////////////////////////////////////////////////////////////
//
// 구조체
//
////////////////////////////////////////////////////////////////////////////////
// 1바이트로 정렬
#pragma pack( push, 1 )

// 램 디스크의 자료구조를 저장하는 구조체
typedef struct kRDDManagerStruct
{
    // 램 디스크용으로 할당 받은 메모리 영역
    BYTE* pbBuffer;
    
    // 총 섹터 수
    DWORD dwTotalSectorCount;
} RDDMANAGER;

#pragma pack( pop)

////////////////////////////////////////////////////////////////////////////////
//
//  함수
//
////////////////////////////////////////////////////////////////////////////////
bool kInitializeRDD( DWORD dwTotalSectorCount );
bool kReadRDDInformation(bool bPrimary, bool bMaster,
        HDDINFORMATION* pstHDDInformation );
int kReadRDDSector(bool bPrimary, bool bMaster, DWORD dwLBA, int iSectorCount,
        char* pcBuffer );
int kWriteRDDSector(bool bPrimary, bool bMaster, DWORD dwLBA, int iSectorCount,
        char* pcBuffer );
