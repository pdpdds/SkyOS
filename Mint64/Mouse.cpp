/**
 *  file    Mouse.c
 *  date    2009/09/26
 *  author  kkamagui 
 *          Copyright(c)2008 All rights reserved by kkamagui
 *  brief   마우스 디바이스 드라이버에 관련된 소스 파일
 */

#include "Mouse.h"
#include "Keyboard.h"
#include "Queue.h"
#include "Hal.h"

// 마우스 상태를 관리하는 마우스 매니저
static MOUSEMANAGER gs_stMouseManager = { 0, };

// 마우스를 저장하는 큐와 버퍼 정의
static QUEUE gs_stMouseQueue;
static MOUSEDATA gs_vstMouseQueueBuffer[ MOUSE_MAXQUEUECOUNT ];

/**
 *  마우스 초기화
 */
BOOL kInitializeMouse( void )
{
    // 큐 초기화
    kInitializeQueue( &gs_stMouseQueue, gs_vstMouseQueueBuffer, MOUSE_MAXQUEUECOUNT, 
            sizeof( MOUSEDATA ) );
    
    // 스핀락 초기화
    kInitializeSpinLock( &( gs_stMouseManager.stSpinLock ) );
    
    // 마우스 활성화
    if( kActivateMouse() == TRUE )
    {
        // 마우스 인터럽트 활성화
        kEnableMouseInterrupt();
        return TRUE;
    }
    return FALSE;
}

/**
 *  마우스를 활성화 함
 */
BOOL kActivateMouse( void )
{
    int i = 0;
    BOOL bPreviousInterrupt;
    BOOL bResult;
    
    // 인터럽트 불가
    bPreviousInterrupt = kSetInterruptFlag( FALSE );
    
    // 컨트롤 레지스터(포트 0x64)에 마우스 활성화 커맨드(0xA8)을 전달하여 마우스 디바이스 활성화
    OutPortByte( 0x64, 0xA8 );
   
    // 컨트롤 레지스터(포트 0x64)에 마우스로 데이터를 전송하는 커맨드(0xD4)를 전달하여 
    // 입력 버퍼(포트 0x60)로 전달된 데이터를 마우스로 전송
    OutPortByte( 0x64, 0xD4 );
    
    // 입력 버퍼(포트 0x60)가 빌 때까지 기다렸다가 마우스에 활성화 커맨드를 전송
    // 0xFFFF만큼 루프를 수행할 시간이면 충분히 커맨드가 전송될 수 있음
    // 0xFFFF 루프를 수행한 이후에도 입력 버퍼(포트 0x60)가 비지 않으면 무시하고 전송
    for( i = 0 ; i < 0xFFFF ; i++ )
    {
        // 입력 버퍼(포트 0x60)가 비어있으면 키보드 커맨드 전송 가능
        if( kIsInputBufferFull() == FALSE )
        {
            break;
        }
    }
    
    // 입력 버퍼(포트 0x60)로 마우스 활성화(0xF4) 커맨드를 전달하여 마우스로 전송
    OutPortByte( 0x60, 0xF4 );
    
    // ACK가 올 때까지 대기함
    bResult = kWaitForACKAndPutOtherScanCode();
    
    // 이전 인터럽트 상태 복원
    kSetInterruptFlag( bPreviousInterrupt );
    return bResult;
}

/**
 *  마우스 인터럽트를 활성화
 */
void kEnableMouseInterrupt( void )
{
    BYTE bOutputPortData;
    int i;
    
    // 커맨드 바이트 읽기
    // 컨트롤 레지스터(포트 0x64)에 키보드 컨트롤러의 커맨드 바이트를 읽는 커맨드(0x20) 전송
    OutPortByte( 0x64, 0x20 );
    
    // 출력 포트의 데이터를 기다렸다가 읽음
    for( i = 0 ; i < 0xFFFF ; i++ )
    {
        // 출력 버퍼(포트 0x60)가 차있으면 데이터를 읽을 수 있음 
        if( kIsOutputBufferFull() == TRUE )
        {
            break;
        }
    }
    // 출력 포트(포트 0x60)에 수신된 커맨드 바이트 값을 읽음
    bOutputPortData = InPortByte( 0x60 );
    

    // 마우스 인터럽트 비트 활성화한 뒤 커맨드 바이트 전송
    // 마우스 인터럽트 비트(비트 1) 설정
    bOutputPortData |= 0x02;

    // 커맨드 레지스터(0x64)에 커맨드 바이트를 쓰는 커맨드(0x60)을 전달
    OutPortByte( 0x64, 0x60 );

    // 입력 버퍼(포트 0x60)에 데이터가 비어있으면 출력 포트에 값을 쓰는 커맨드와 커맨드 바이트 전송
    for( i = 0 ; i < 0xFFFF ; i++ )
    {
        // 입력 버퍼(포트 0x60)가 비었으면 커맨드 전송 가능
        if( kIsInputBufferFull() == FALSE )
        {
            break;
        }
    }
    
    // 입력 버퍼(0x60)에 마우스 인터럽트 비트가 1로 설정된 값을 전달
    OutPortByte( 0x60, bOutputPortData );
}


/**
 *  마우스 데이터를 모아서 큐에 삽입
 */
BOOL kAccumulateMouseDataAndPutQueue( BYTE bMouseData )
{
    BOOL bResult = FALSE;
    
    // 수신된 바이트 수에 따라 마우스 데이터를 설정
    switch( gs_stMouseManager.iByteCount )
    {
        // 바이트 1에 데이터 설정
    case 0:
        gs_stMouseManager.stCurrentData.bButtonStatusAndFlag = bMouseData;
        gs_stMouseManager.iByteCount++;
        break;
        
        // 바이트 2에 데이터 설정
    case 1:
        gs_stMouseManager.stCurrentData.bXMovement = bMouseData;
        gs_stMouseManager.iByteCount++;
        break;
        
        // 바이트 3에 데이터 설정
    case 2:
        gs_stMouseManager.stCurrentData.bYMovement = bMouseData;
        gs_stMouseManager.iByteCount++;
        break;
        
        // 그 외의 경우는 수신된 바이트 수 초기화
    default:
        gs_stMouseManager.iByteCount = 0;
        break;
    }
    
    // 3바이트가 모두 수신되었으면 마우스 큐에 삽입하고 수신된 횟수를 초기화
    if( gs_stMouseManager.iByteCount >= 3 )
    {
        // 임계 영역 시작
        kLockForSpinLock( &( gs_stMouseManager.stSpinLock ) );
        
        // 마우스 큐에 마우스 데이터 삽입 
        bResult = kPutQueue( &gs_stMouseQueue, &gs_stMouseManager.stCurrentData );    
        // 임계 영역 끝
        kUnlockForSpinLock( &( gs_stMouseManager.stSpinLock ) );
        // 수신된 바이트 수 초기화
        gs_stMouseManager.iByteCount = 0;
    }
    return bResult;
}

/**
 *  마우스 큐에서 마우스 데이터를 꺼냄
 */
BOOL kGetMouseDataFromMouseQueue( BYTE* pbButtonStatus, int* piRelativeX, 
        int* piRelativeY )
{
    MOUSEDATA stData;
    BOOL bResult;

    // 큐가 비어있으면 데이터를 꺼낼 수 없음
    if( kIsQueueEmpty( &( gs_stMouseQueue ) ) == TRUE )
    {
        return FALSE;
    }
    
    // 임계 영역 시작
    kLockForSpinLock( &( gs_stMouseManager.stSpinLock ) );
    // 큐에서 데이터를 꺼냄
    bResult = kGetQueue( &( gs_stMouseQueue ), &stData );    
    // 임계 영역 끝
    kUnlockForSpinLock( &( gs_stMouseManager.stSpinLock ) );
    
    // 데이터를 꺼내지 못했으면 실패
    if( bResult == FALSE )
    {
        return FALSE;
    }
    
    // 마우스 데이터 분석
    // 마우스 버튼 플래그는 첫 번째 바이트의 하위 3비트에 존재함
    *pbButtonStatus = stData.bButtonStatusAndFlag & 0x7;

    // X, Y의 이동거리 설정
    // X의 부호 비트는 비트 4에 있으며 1로 설정되어있으면 음수임
    *piRelativeX = stData.bXMovement & 0xFF;
    if( stData.bButtonStatusAndFlag & 0x10 )
    {
        // 음수이므로 아래 8비트에 X 이동거리를 설정한 후 상위 비트를 모두 1로 만들어
        // 부호 비트를 확장함 
        *piRelativeX |= ( 0xFFFFFF00 );
    }
    
    // Y의 부호 비트는 비트 5에 있으며, 1로 설정되었으면 음수임
    // 아래 방향으로 갈수록 Y 값이 증가하는 화면 좌표와 달리 마우스는 위쪽 방향으로 갈수록
    // 값이 증가하므로 계산이 끝난 후 부호를 뒤집음
    *piRelativeY = stData.bYMovement & 0xFF;
    if( stData.bButtonStatusAndFlag & 0x20 )
    {
        // 음수이므로 아래 8비트에 Y 이동거리를 설정한 후 상위 비트를 모두 1로 만들어
        // 부호 비트를 확장함 
        *piRelativeY |= ( 0xFFFFFF00 );
    }

    // 마우스의 Y축 증감 방향은 화면 좌표와 반대이므로 Y 이동거리에 -하여 방향을 바꿈
    *piRelativeY = -*piRelativeY;
    return TRUE;
}

/**
 *  마우스 데이터가 출력 버퍼에 있는지를 반환
 */
BOOL kIsMouseDataInOutputBuffer( void )
{
    // 출력 버퍼(포트 0x60을 읽기 전에 먼저 상태 레지스터(포트 0x64)를 읽어서
    // 마우스 데이터인가를 확인, 마우스 데이터는 AUXB 비트(비트 5)가 1로 설정됨
    if( InPortByte( 0x64 ) & 0x20 )
    {
        return TRUE;
    }
    return FALSE;
}
