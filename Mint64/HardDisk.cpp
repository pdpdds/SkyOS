/**
 *  file    HardDisk.c
 *  date    2009/04/20
 *  author  kkamagui 
 *          Copyright(c)2008 All rights reserved by kkamagui
 *  brief   하드 디스크 컨트롤러에 관련된 소스 파일
 */

#include "HardDisk.h"
#include "PIT.h"
#include "Hal.h"
#include "string.h"
#include "Console.h"

// 하드 디스크를 관리하는 자료구조
static HDDMANAGER gs_stHDDManager;

/**
 *  하드 디스크 디바이스 드라이버를 초기화
 */
BOOL kInitializeHDD(void)
{
    // 뮤텍스 초기화
    kInitializeMutex( &( gs_stHDDManager.stMutex ) );

    // 인터럽트 플래그 초기화
    gs_stHDDManager.bPrimaryInterruptOccur = FALSE;
    gs_stHDDManager.bSecondaryInterruptOccur = FALSE;

    // 첫 번째와 두 번째 PATA 포트의 디지털 출력 레지스터(포트 0x3F6와 0x376)에 0을
    // 출력하여 하드 디스크 컨트롤러의 인터럽트를 활성화
    OutPortByte( HDD_PORT_PRIMARYBASE + HDD_PORT_INDEX_DIGITALOUTPUT, 0 );
    OutPortByte( HDD_PORT_SECONDARYBASE + HDD_PORT_INDEX_DIGITALOUTPUT, 0 );
    
    // 하드 디스크 정보 요청
    if( kReadHDDInformation( TRUE, TRUE, &( gs_stHDDManager.stHDDInformation ) ) 
            == FALSE )
    {
        gs_stHDDManager.bHDDDetected = FALSE;
        gs_stHDDManager.bCanWrite = FALSE;
        return FALSE;
    }

    // 하드 디스크가 검색되었으면 QEMU에서만 쓸 수 있도록 설정
    gs_stHDDManager.bHDDDetected = TRUE;
    if( strcmp( (const char*)gs_stHDDManager.stHDDInformation.vwModelNumber, "QEMU") == 0 )
    {
        gs_stHDDManager.bCanWrite = TRUE;
    }
    else
    {
        gs_stHDDManager.bCanWrite = FALSE;
    }
    return TRUE;
}

/**
 *  하드 디스크의 상태를 반환
 */
static BYTE kReadHDDStatus( BOOL bPrimary )
{
    if( bPrimary == TRUE )
    {
        // 첫 번째 PATA 포트의 상태 레지스터(포트 0x1F7)에서 값을 반환
        return InPortByte( HDD_PORT_PRIMARYBASE + HDD_PORT_INDEX_STATUS );
    }
    // 두 번째 PATA 포트의 상태 레지스터(포트 0x177)에서 값을 반환
    return InPortByte( HDD_PORT_SECONDARYBASE + HDD_PORT_INDEX_STATUS );
}

/**
 *  하드 디스크의 Busy가 해제될 때까지 일정 시간 동안 대기
 */
static BOOL kWaitForHDDNoBusy( BOOL bPrimary )
{
    DWORD dwStartTickCount;
    BYTE bStatus;

    // 대기를 시작한 시간을 저장
    dwStartTickCount = GetTickCount();

    // 일정 시간 동안 하드 디스크의 Busy가 해제될 때까지 대기
    while( ( GetTickCount() - dwStartTickCount ) <= HDD_WAITTIME )
    {
        // HDD의 상태를 반환
        bStatus = kReadHDDStatus( bPrimary );

        // Busy 비트(비트 7)이 설정되어 있지 않으면 Busy가 해제된 것이므로 종료
        if( ( bStatus & HDD_STATUS_BUSY ) != HDD_STATUS_BUSY )
        {
            return TRUE;
        }
		SimpleSleep( 1 );
    }
    return FALSE;
}

/**
 *  하드 디스크가 Ready될 때까지 일정 시간 동안 대기
 */
static BOOL kWaitForHDDReady(BOOL bPrimary)
{
    DWORD qwStartTickCount;
    BYTE bStatus;

    // 대기를 시작한 시간을 저장
    qwStartTickCount = GetTickCount();

    // 일정 시간 동안 하드 디스크가 Ready가 될 때까지 대기
    while( ( GetTickCount() - qwStartTickCount ) <= HDD_WAITTIME )
    {
        // HDD의 상태를 반환
        bStatus = kReadHDDStatus( bPrimary );

        // Ready 비트(비트 6)이 설정되어 있으면 데이터를 받을 준비가 된 것이므로
        // 종료
        if( ( bStatus & HDD_STATUS_READY ) == HDD_STATUS_READY )
        {
            return TRUE;
        }
		SimpleSleep( 1 );
    }
    return FALSE;
}


/**
 *  인터럽트 발생 여부를 설정
 */
void kSetHDDInterruptFlag( BOOL bPrimary, BOOL bFlag )
{
    if( bPrimary == TRUE )
    {
        gs_stHDDManager.bPrimaryInterruptOccur = bFlag;
    }
    else
    {
        gs_stHDDManager.bSecondaryInterruptOccur = bFlag;
    }
}

/**
 *  인터럽트가 발생할 때까지 대기
 */
static BOOL kWaitForHDDInterrupt( BOOL bPrimary )
{
    DWORD dwTickCount;
    
    // 대기를 시작한 시간을 저장
    dwTickCount = GetTickCount();
    
    // 일정 시간 동안 하드 디스크의 인터럽트가 발생할 때까지 대기
    while( GetTickCount() - dwTickCount <= HDD_WAITTIME )
    {
        // 하드 디스크 자료구조에 인터럽트 발생 플래그를 확인
        if( ( bPrimary == TRUE ) && 
            ( gs_stHDDManager.bPrimaryInterruptOccur == TRUE ) )
        {
            return TRUE;
        }
        else if( ( bPrimary == FALSE ) && 
                 ( gs_stHDDManager.bSecondaryInterruptOccur == TRUE ) )
        {
            return TRUE;
        }
    }
    return FALSE;
}

/**
 *  하드 디스크의 정보를 읽음
 */
BOOL kReadHDDInformation( BOOL bPrimary, BOOL bMaster, HDDINFORMATION* pstHDDInformation )
{
    WORD wPortBase;
    DWORD dwLastTickCount;
    BYTE bStatus;
    BYTE bDriveFlag;
    int i;
    WORD wTemp;
    BOOL bWaitResult;

    // PATA 포트에 따라서 I/O 포트의 기본 어드레스를 설정
    if( bPrimary == TRUE )
    {
        // 첫 번째 PATA 포트이면 포트 0x1F0을 저장
        wPortBase = HDD_PORT_PRIMARYBASE;
    }
    else
    {
        // 두 번째 PATA 포트이면 포트 0x170을 저장
        wPortBase = HDD_PORT_SECONDARYBASE;
    }

    // 동기화 처리
    kLock( &(gs_stHDDManager.stMutex ) );
    
    // 아직 수행 중인 커맨드가 있다면 일정 시간 동안 끝날 때까지 대기
    if( kWaitForHDDNoBusy( bPrimary ) == FALSE )
    {
        // 동기화 처리
        kUnlock( &(gs_stHDDManager.stMutex ) );
        return FALSE;
    }
    
    //==========================================================================
    //  LBA 어드레스와 드라이브 및 헤드에 관련된 레지스터 설정
    //      드라이브와 헤드 정보만 있으면 됨
    //==========================================================================
    // 드라이브와 헤드 데이터 설정
    if( bMaster == TRUE )
    {
        // 마스터이면 LBA 플래그만 설정
        bDriveFlag = HDD_DRIVEANDHEAD_LBA;
    }
    else
    {
        // 슬레이브이면 LBA 플래그에 슬레이브 플래그도 같이 설정
        bDriveFlag = HDD_DRIVEANDHEAD_LBA | HDD_DRIVEANDHEAD_SLAVE;
    }
    // 드라이브/헤드 레지스터(포트 0x1F6 또는 0x176)에 설정된 값을 전송
    OutPortByte( wPortBase + HDD_PORT_INDEX_DRIVEANDHEAD, bDriveFlag );

    //==========================================================================
    //  커맨드 전송 후, 인터럽트 대기
    //==========================================================================
    // 커맨드를 받아들일 준비가 될 때까지 일정 시간 동안 대기
    if( kWaitForHDDReady( bPrimary ) == FALSE )
    {
        // 동기화 처리
        kUnlock( &(gs_stHDDManager.stMutex ) );
        return FALSE;
    }

    // 인터럽트 플래그를 초기화
    kSetHDDInterruptFlag( bPrimary, FALSE );
    
    // 커맨드 레지스터(포트 0x1F7 또는 0x177)에 드라이브 인식 커맨드(0xEC)를 전송
    OutPortByte( wPortBase + HDD_PORT_INDEX_COMMAND, HDD_COMMAND_IDENTIFY );

    // 처리가 완료될 때까지 인터럽트 발생을 기다림
    bWaitResult = kWaitForHDDInterrupt( bPrimary );
    // 에러가 발생하거나 인터럽트가 발생하지 않으면 문제가 발생한 것이므로 종료
    bStatus = kReadHDDStatus( bPrimary );
    if( ( bWaitResult == FALSE ) || 
        ( ( bStatus & HDD_STATUS_ERROR ) == HDD_STATUS_ERROR ) )
    {
        // 동기화 처리
        kUnlock( &( gs_stHDDManager.stMutex ) );
        return FALSE;
    }

    //==========================================================================
    //  데이터 수신
    //==========================================================================
    // 한 섹터를 읽음
    for( i = 0; i < 512 / 2; i++ )
    {
        ( ( WORD* ) pstHDDInformation )[ i ] =
        InPortWord( wPortBase + HDD_PORT_INDEX_DATA );
    }

    // 문자열은 바이트 순서로 다시 변환
    kSwapByteInWord( pstHDDInformation->vwModelNumber,
            sizeof( pstHDDInformation->vwModelNumber ) / 2 );
    kSwapByteInWord( pstHDDInformation->vwSerialNumber,
            sizeof( pstHDDInformation->vwSerialNumber ) / 2 );

    // 동기화 처리
    kUnlock( &(gs_stHDDManager.stMutex ) );
    return TRUE;
}

/**
 *  WORD 내의 바이트 순서를 바꿈
 */
static void kSwapByteInWord(WORD* pwData, int iWordCount)
{
    int i;
    WORD wTemp;

    for (i = 0; i < iWordCount; i++)
    {
        wTemp = pwData[ i ];
        pwData[ i ] = (wTemp >> 8 ) | (wTemp << 8 );
    }
}

/**
 *  하드 디스크의 섹터를 읽음
 *      최대 256개의 섹터를 읽을 수 있음
 *      실제로 읽은 섹터 수를 반환
 */
int kReadHDDSector( BOOL bPrimary, BOOL bMaster, DWORD dwLBA, int iSectorCount,
        char* pcBuffer )
{
    WORD wPortBase;
    int i, j;
    BYTE bDriveFlag;
    BYTE bStatus;
    long lReadCount = 0;
    BOOL bWaitResult;

    // 범위 검사. 최대 256 섹터를 처리할 수 있음
    if( ( gs_stHDDManager.bHDDDetected == FALSE ) ||
        ( iSectorCount <= 0 ) || ( 256 < iSectorCount ) ||
        ( ( dwLBA + iSectorCount ) >= gs_stHDDManager.stHDDInformation.dwTotalSectors ) )
    {
        return 0;
    }

    // PATA 포트에 따라서 I/O 포트의 기본 어드레스를 설정
    if( bPrimary == TRUE )
    {
        // 첫 번째 PATA 포트이면 포트 0x1F0을 저장
        wPortBase = HDD_PORT_PRIMARYBASE;
    }
    else
    {
        // 두 번째 PATA 포트이면 포트 0x170을 저장
        wPortBase = HDD_PORT_SECONDARYBASE;
    }

    // 동기화 처리
    kLock( &( gs_stHDDManager.stMutex ) );
    
    // 아직 수행 중인 커맨드가 있다면 일정 시간 동안 끝날 때까지 대기
    if( kWaitForHDDNoBusy( bPrimary ) == FALSE )
    {
        // 동기화 처리
        kUnlock( &( gs_stHDDManager.stMutex ) );
        return FALSE;
    }

    //==========================================================================
    //  데이터 레지스터 설정
    //      LBA 모드의 경우, 섹터 번호 -> 실린더 번호 -> 헤드 번호의 순으로 
    //      LBA 어드레스를 대입
    //==========================================================================
    // 섹터 수 레지스터(포트 0x1F2 또는 0x172)에 읽을 섹터 수를 전송
    OutPortByte( wPortBase + HDD_PORT_INDEX_SECTORCOUNT, iSectorCount );
    // 섹터 번호 레지스터(포트 0x1F3 또는 0x173)에 읽을 섹터 위치(LBA 0~7비트)를 전송
    OutPortByte( wPortBase + HDD_PORT_INDEX_SECTORNUMBER, dwLBA );
    // 실린더 LSB 레지스터(포트 0x1F4 또는 0x174)에 읽을 섹터 위치(LBA 8~15비트)를 전송
    OutPortByte( wPortBase + HDD_PORT_INDEX_CYLINDERLSB, dwLBA >> 8 );
    // 실린더 MSB 레지스터(포트 0x1F5 또는 0x175)에 읽을 섹터 위치(LBA 16~23비트)를 전송
    OutPortByte( wPortBase + HDD_PORT_INDEX_CYLINDERMSB, dwLBA >> 16 );
    // 드라이브와 헤드 데이터 설정
    if( bMaster == TRUE )
    {
        bDriveFlag = HDD_DRIVEANDHEAD_LBA;
    }
    else
    {
        bDriveFlag = HDD_DRIVEANDHEAD_LBA | HDD_DRIVEANDHEAD_SLAVE;
    }
    // 드라이브/헤드 레지스터(포트 0x1F6 또는 0x176)에 읽을 섹터의 위치(LBA 24~27비트)와
    // 설정된 값을 같이 전송
    OutPortByte( wPortBase + HDD_PORT_INDEX_DRIVEANDHEAD, bDriveFlag | ( (dwLBA
            >> 24 ) & 0x0F ) );

    //==========================================================================
    //  커맨드 전송
    //==========================================================================
    // 커맨드를 받아들일 준비가 될 때까지 일정 시간 동안 대기
    if( kWaitForHDDReady( bPrimary ) == FALSE )
    {
        // 동기화 처리
        kUnlock( &( gs_stHDDManager.stMutex ) );
        return FALSE;
    }

    // 인터럽트 플래그를 초기화
    kSetHDDInterruptFlag( bPrimary, FALSE );
    
    // 커맨드 레지스터(포트 0x1F7 또는 0x177)에 읽기(0x20)을 전송
    OutPortByte( wPortBase + HDD_PORT_INDEX_COMMAND, HDD_COMMAND_READ );

    //==========================================================================
    //  인터럽트 대기 후, 데이터 수신
    //==========================================================================
    // 섹터 수만큼 루프를 돌면서 데이터 수신
    for( i = 0 ; i < iSectorCount ; i++ )
    {
        // 에러가 발생하면 종료
        bStatus = kReadHDDStatus( bPrimary );
        if( ( bStatus & HDD_STATUS_ERROR ) == HDD_STATUS_ERROR )
        {
			SkyConsole::Print( "Error Occur\n" );
            // 동기화 처리
            kUnlock( &( gs_stHDDManager.stMutex ) );
            return i;
        }

        // DATAREQUEST 비트가 설정되지 않았으면 데이터가 수신되길 기다림
        if( ( bStatus & HDD_STATUS_DATAREQUEST ) != HDD_STATUS_DATAREQUEST )
        {
            // 처리가 완료될 때까지 일정 시간 동안 인터럽트를 기다림
            bWaitResult = kWaitForHDDInterrupt( bPrimary );
            kSetHDDInterruptFlag( bPrimary, FALSE );
            // 인터럽트가 발생하지 않으면 문제가 발생한 것이므로 종료
            if( bWaitResult == FALSE )
            {
				SkyConsole::Print( "Interrupt Not Occur\n" );
                // 동기화 처리
                kUnlock( &( gs_stHDDManager.stMutex ) );
                return FALSE;
            }
        }
        
        // 한 섹터를 읽음
        for( j = 0 ; j < 512 / 2 ; j++ )
        {
            ( ( WORD* ) pcBuffer )[ lReadCount++ ]
                    = InPortWord( wPortBase + HDD_PORT_INDEX_DATA );
        }
    }

    // 동기화 처리
    kUnlock( &( gs_stHDDManager.stMutex ) );
    return i;
}

/**
 *  하드 디스크에 섹터를 씀
 *      최대 256개의 섹터를 쓸 수 있음
 *      실제로 쓴 섹터 수를 반환
 */
int kWriteHDDSector(BOOL bPrimary, BOOL bMaster, DWORD dwLBA, int iSectorCount,
        char* pcBuffer)
{
    WORD wPortBase;
    WORD wTemp;
    int i, j;
    BYTE bDriveFlag;
    BYTE bStatus;
    long lReadCount = 0;
    BOOL bWaitResult;

    // 범위 검사. 최대 256 섹터를 처리할 수 있음
    if( ( gs_stHDDManager.bCanWrite == FALSE ) || 
        ( iSectorCount <= 0 ) || ( 256 < iSectorCount ) ||
        ( ( dwLBA + iSectorCount ) >= gs_stHDDManager.stHDDInformation.dwTotalSectors ) )

    {
        return 0;
    }

    // PATA 포트에 따라서 I/O 포트의 기본 어드레스를 설정
    if( bPrimary == TRUE )
    {
        // 첫 번째 PATA 포트이면 포트 0x1F0을 저장
        wPortBase = HDD_PORT_PRIMARYBASE;
    }
    else
    {
        // 두 번째 PATA 포트이면 포트 0x170을 저장
        wPortBase = HDD_PORT_SECONDARYBASE;
    }

    // 아직 수행 중인 커맨드가 있다면 일정 시간 동안 끝날 때까지 대기
    if( kWaitForHDDNoBusy( bPrimary ) == FALSE )
    {
        return FALSE;
    }

    // 동기화 처리
    kLock( &(gs_stHDDManager.stMutex ) );

    //==========================================================================
    //  데이터 레지스터 설정
    //      LBA 모드의 경우, 섹터 번호 -> 실린더 번호 -> 헤드 번호의 순으로 
    //      LBA 어드레스를 대입
    //==========================================================================
    // 섹터 수 레지스터(포트 0x1F2 또는 0x172)에 쓸 섹터 수를 전송
    OutPortByte( wPortBase + HDD_PORT_INDEX_SECTORCOUNT, iSectorCount );
    // 섹터 번호 레지스터(포트 0x1F3 또는 0x173)에 쓸 섹터 위치(LBA 0~7비트)를 전송
    OutPortByte( wPortBase + HDD_PORT_INDEX_SECTORNUMBER, dwLBA );
    // 실린더 LSB 레지스터(포트 0x1F4 또는 0x174)에 쓸 섹터 위치(LBA 8~15비트)를 전송
    OutPortByte( wPortBase + HDD_PORT_INDEX_CYLINDERLSB, dwLBA >> 8 );
    // 실린더 MSB 레지스터(포트 0x1F5 또는 0x175)에 쓸 섹터 위치(LBA 16~23비트)를 전송
    OutPortByte( wPortBase + HDD_PORT_INDEX_CYLINDERMSB, dwLBA >> 16 );
    // 드라이브와 헤드 데이터 설정
    if( bMaster == TRUE )
    {
        bDriveFlag = HDD_DRIVEANDHEAD_LBA;
    }
    else
    {
        bDriveFlag = HDD_DRIVEANDHEAD_LBA | HDD_DRIVEANDHEAD_SLAVE;
    }
    // 드라이브/헤드 레지스터(포트 0x1F6 또는 0x176)에 쓸 섹터의 위치(LBA 24~27비트)와
    // 설정된 값을 같이 전송
    OutPortByte(wPortBase + HDD_PORT_INDEX_DRIVEANDHEAD, bDriveFlag | ( (dwLBA
            >> 24 ) & 0x0F ) );

    //==========================================================================
    //  커맨드 전송 후, 데이터 송신이 가능할 때까지 대기
    //==========================================================================
    // 커맨드를 받아들일 준비가 될 때까지 일정 시간 동안 대기
    if( kWaitForHDDReady( bPrimary ) == FALSE )
    {
        // 동기화 처리
        kUnlock( &( gs_stHDDManager.stMutex ) );
        return FALSE;
    }

    // 커맨드 전송
    OutPortByte( wPortBase + HDD_PORT_INDEX_COMMAND, HDD_COMMAND_WRITE );
    
    // 데이터 송신이 가능할 때까지 대기
    while( 1 )
    {
        bStatus = kReadHDDStatus( bPrimary );
        // 에러가 발생하면 종료
        if( ( bStatus & HDD_STATUS_ERROR ) == HDD_STATUS_ERROR )
        {
            // 동기화 처리
            kUnlock( &(gs_stHDDManager.stMutex ) );
            return 0;
        }
        
        // Data Request비트가 설정되었다면 데이터 송신 가능
        if( ( bStatus & HDD_STATUS_DATAREQUEST ) == HDD_STATUS_DATAREQUEST )
        {
            break;
        }
        
		SimpleSleep( 1 );
    }

    //==========================================================================
    //  데이터 송신 후, 인터럽트 대기
    //==========================================================================
    // 섹터 수만큼 루프를 돌면서 데이터 송신
    for( i = 0 ; i < iSectorCount ; i++)
    {
        // 인터럽트 플래그를 초기화하고 한 섹터를 씀
        kSetHDDInterruptFlag( bPrimary, FALSE );
        for( j = 0 ; j < 512 / 2 ; j++ )
        {
            OutPortWord( wPortBase + HDD_PORT_INDEX_DATA, 
                         ( ( WORD* ) pcBuffer )[ lReadCount++ ]);
        }
        
        // 에러가 발생하면 종료
        bStatus = kReadHDDStatus( bPrimary );
        if( ( bStatus & HDD_STATUS_ERROR ) == HDD_STATUS_ERROR )
        {
            // 동기화 처리
            kUnlock( &(gs_stHDDManager.stMutex ) );
            return i;
        }
        
        // DATAREQUEST 비트가 설정되지 않았으면 데이터가 처리가 완료되길 기다림
        if( ( bStatus & HDD_STATUS_DATAREQUEST ) != HDD_STATUS_DATAREQUEST )
        {
            // 처리가 완료될 때까지 일정 시간 동안 인터럽트를 기다림
            bWaitResult = kWaitForHDDInterrupt( bPrimary );
            kSetHDDInterruptFlag( bPrimary, FALSE );
            // 인터럽트가 발생하지 않으면 문제가 발생한 것이므로 종료
            if( bWaitResult == FALSE )
            {
                // 동기화 처리
                kUnlock( &( gs_stHDDManager.stMutex ) );
                return FALSE;
            }
        }        
    }
    
    // 동기화 처리
    kUnlock( &(gs_stHDDManager.stMutex ) );
    return i;
}
