/**
 *  file    RAMDisk.c
 *  date    2009/05/22
 *  author  kkamagui 
 *          Copyright(c)2008 All rights reserved by kkamagui
 *  brief   램 디스크에 관련된 함수를 정의한 소스 파일
 */

#include "RAMDisk.h"
#include "string.h"
#include "memory.h"
#include "Math.h"

// 램 디스크를 관리하는 자료구조
static RDDMANAGER gs_stRDDManager;

/**
 *  램 디스크 디바이스 드라이버 초기화 함수
 */
bool kInitializeRDD( DWORD dwTotalSectorCount )
{
    // 자료구조 초기화
    memset( &gs_stRDDManager, 0, sizeof( gs_stRDDManager ) );
    
    // 램 디스크로 사용할 메모리를 할당
    gs_stRDDManager.pbBuffer = new BYTE[dwTotalSectorCount * 512];
    if( gs_stRDDManager.pbBuffer == NULL )
    {
        return FALSE;
    }
    
    // 총 섹터 수와 동기화 객체를 설정
    gs_stRDDManager.dwTotalSectorCount = dwTotalSectorCount;
 
    return TRUE;
}

/**
 *  램 디스크의 정보를 반환
 */
bool kReadRDDInformation(bool bPrimary, bool bMaster,
        HDDINFORMATION* pstHDDInformation )
{
    // 자료구조 초기화
	memset( pstHDDInformation, 0, sizeof( HDDINFORMATION ) );
    
    // 총 섹터 수와 시리얼 번호, 그리고 모델 번호만 설정
    pstHDDInformation->dwTotalSectors = gs_stRDDManager.dwTotalSectorCount;
    memcpy( pstHDDInformation->vwSerialNumber, "0000-0000", 9 );
	memcpy( pstHDDInformation->vwModelNumber, "MINT RAM Disk v1.0", 18 );

    return TRUE;
}

/**
 *  램 디스크에서 여러 섹터를 읽어서 반환
 */
int kReadRDDSector(bool bPrimary, bool bMaster, DWORD dwLBA, int iSectorCount,
        char* pcBuffer )
{
    int iRealReadCount = 0;
    
    // LBA 어드레스부터 끝까지 읽을 수 있는 섹터 수와 읽어야 할 섹터 수를 비교해서
    // 실제로 읽을 수 있는 수를 계산
    iRealReadCount = (int)MIN( gs_stRDDManager.dwTotalSectorCount - (dwLBA + (DWORD)iSectorCount), (DWORD)iSectorCount );

    // 램 디스크 메모리에서 데이터를 실제로 읽을 섹터 수만큼 복사해서 반환
    memcpy( pcBuffer, gs_stRDDManager.pbBuffer + ( dwLBA * 512 ), 
             iRealReadCount * 512 );
    
    return iRealReadCount;
}

/**
 *  램 디스크에 여러 섹터를 씀
 */
int kWriteRDDSector(bool bPrimary, bool bMaster, DWORD dwLBA, int iSectorCount,
        char* pcBuffer )
{
    int iRealWriteCount;
    
    // LBA 어드레스부터 끝까지 쓸 수 있는 섹터 수와 써야 할 섹터 수를 비교해서
    // 실제로 쓸 수 있는 수를 계산
    iRealWriteCount = (int)MIN( gs_stRDDManager.dwTotalSectorCount - (dwLBA + (DWORD)iSectorCount), (DWORD)iSectorCount );

    // 데이터를 실제로 쓸 섹터 수만큼 램 디스크 메모리에 복사
	memcpy( gs_stRDDManager.pbBuffer + ( dwLBA * 512 ), pcBuffer,
             iRealWriteCount * 512 );
    
    return iRealWriteCount;    
}

