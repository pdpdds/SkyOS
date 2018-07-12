/**
 *  file    Window.c
 *  date    2009/09/28
 *  author  kkamagui
 *          Copyright(c)2008 All rights reserved by kkamagui
 *  brief   GUI 시스템에 관련된 함수를 정의한 소스 파일
 */
#include "Window.h"
#include "memory.h"
#include "math.h"
#include "string.h"
#include "VBE.h"
//#include "Task.h"
#include "MintFont.h"
//#include "DynamicMemory.h"
//#include "Utility.h"
#include "JPEG.h"
#include "VESA.h"
#include "Mouse.h"
//#include "Console.h"
//#include "AssemblyUtility.h"
//#include "Task.h"
//#include "MultiProcessor.h"

// GUI 시스템 관련 자료구조
static WINDOWPOOLMANAGER gs_stWindowPoolManager;
// 윈도우 매니저 관련 자료구조
static WINDOWMANAGER gs_stWindowManager;

extern void printf(const char* str, ...);

//==============================================================================
//  윈도우 풀 관련
//==============================================================================
/**
 *  윈도우 풀을 초기화
 */
static void kInitializeWindowPool( void )
{
    int i;
    void* pvWindowPoolAddress;
    
    // 자료구조 초기화
	memset( &gs_stWindowPoolManager, 0, sizeof( gs_stWindowPoolManager ) );
    
    // 윈도우 풀의 메모리를 할당
    pvWindowPoolAddress = ( void* ) new char[ sizeof( WINDOW ) * WINDOW_MAXCOUNT ];
    if( pvWindowPoolAddress == NULL )
    {
        printf( "Window Pool Allocate Fail\n" );
        while( 1 )
        {
            ;
        }
    }
    
    // 윈도우 풀의 어드레스를 지정하고 초기화
    gs_stWindowPoolManager.pstStartAddress = ( WINDOW* ) pvWindowPoolAddress;
	memset( pvWindowPoolAddress, 0, sizeof( WINDOW ) * WINDOW_MAXCOUNT );

    // 윈도우 풀에 ID를 할당
    for( i = 0 ; i < WINDOW_MAXCOUNT ; i++ )
    {
        gs_stWindowPoolManager.pstStartAddress[ i ].stLink.qwID = i;
    }
    
    // 윈도우의 최대 개수와 할당된 횟수를 초기화
    gs_stWindowPoolManager.iMaxCount = WINDOW_MAXCOUNT;
    gs_stWindowPoolManager.iAllocatedCount = 1;
    
    // 뮤텍스 초기화
    //kInitializeMutex( &( gs_stWindowPoolManager.stLock ) );
}


/**
 *  윈도우 자료구조를 할당
 */
static WINDOW* kAllocateWindow( void )
{
    WINDOW* pstEmptyWindow;
    int i;

    // 동기화 처리
    //kLock( &( gs_stWindowPoolManager.stLock ) );

	kEnterCriticalSection();

    // 윈도우가 모두 할당되었으면 실패
    if( gs_stWindowPoolManager.iUseCount == gs_stWindowPoolManager.iMaxCount )
    {
        // 동기화 처리
        //kUnlock( &gs_stWindowPoolManager.stLock );
		kLeaveCriticalSection();
        return NULL;
    }

    // 윈도우 풀을 모두 돌면서 빈 영역을 검색
    for( i = 0 ; i < gs_stWindowPoolManager.iMaxCount ; i++ )
    {
        // ID의 상위 32비트가 0이면 비어있는 윈도우 자료구조임
		unsigned int id = gs_stWindowPoolManager.pstStartAddress[i].stLink.qwID >> 32;
        if( id  == 0 )
        {
            pstEmptyWindow = &( gs_stWindowPoolManager.pstStartAddress[ i ] );
            break;
        }
    }

    // 상위 32비트를 0이 아닌 값으로 설정해서 할당된 윈도우로 설정
    pstEmptyWindow->stLink.qwID =
        ( ( QWORD ) gs_stWindowPoolManager.iAllocatedCount << 32 ) | i;

    // 자료구조가 사용 중인 개수와 할당된 횟수를 증가
    gs_stWindowPoolManager.iUseCount++;
    gs_stWindowPoolManager.iAllocatedCount++;
    if( gs_stWindowPoolManager.iAllocatedCount == 0 )
    {
        gs_stWindowPoolManager.iAllocatedCount = 1;
    }

    // 동기화 처리
    //kUnlock( &( gs_stWindowPoolManager.stLock ) );
	kLeaveCriticalSection();
    // 윈도우의 뮤텍스 초기화
   // kInitializeMutex( &( pstEmptyWindow->stLock ) );

    return pstEmptyWindow;
}

/**
 *  윈도우 자료구조를 해제
 */
static void kFreeWindow( QWORD qwID )
{
    int i;

    // 윈도우 ID로 윈도우 풀의 오프셋을 계산, 윈도우 ID의 하위 32비트가 인덱스 역할을 함
    i = GETWINDOWOFFSET( qwID );

    // 동기화 처리
    //kLock( &( gs_stWindowPoolManager.stLock ) );
	kEnterCriticalSection();
    
    // 윈도우 자료구조를 초기화하고 ID 설정
	memset( &( gs_stWindowPoolManager.pstStartAddress[ i ] ), 0, sizeof( WINDOW ) );
    gs_stWindowPoolManager.pstStartAddress[ i ].stLink.qwID = i;

    // 사용 중인 자료구조의 개수를 감소
    gs_stWindowPoolManager.iUseCount--;

	kLeaveCriticalSection();
    // 동기화 처리
    //kUnlock( &( gs_stWindowPoolManager.stLock ) );
}

//==============================================================================
//  윈도우와 윈도우 매니저 관련
//==============================================================================
/**
 *  GUI 시스템을 초기화
 */
void kInitializeGUISystem( void )
{
    VBEMODEINFOBLOCK* pstModeInfo;
    QWORD qwBackgroundWindowID;
    EVENT* pstEventBuffer;

    // 윈도우 풀을 초기화
    kInitializeWindowPool();

    // VBE 모드 정보 블록을 반환
    pstModeInfo = kGetVBEModeInfoBlock();
	pstModeInfo->wXResolution = 1024;
	pstModeInfo->wYResolution = 768;

    // 비디오 메모리 어드레스 설정
    //gs_stWindowManager.pstVideoMemory = ( COLOR* )
      //  ( ( QWORD ) pstModeInfo->dwPhysicalBasePointer & 0xFFFFFFFF );

	gs_stWindowManager.pstVideoMemory = (COLOR*)getlfb();

    // 마우스 커서의 초기 위치 설정
    gs_stWindowManager.iMouseX = pstModeInfo->wXResolution / 2;
    gs_stWindowManager.iMouseY = pstModeInfo->wYResolution / 2;

    // 화면 영역의 범위 설정
    gs_stWindowManager.stScreenArea.iX1 = 0;
    gs_stWindowManager.stScreenArea.iY1 = 0;
    gs_stWindowManager.stScreenArea.iX2 = pstModeInfo->wXResolution - 1;
    gs_stWindowManager.stScreenArea.iY2 = pstModeInfo->wYResolution - 1;

    // 뮤텍스 초기화
   // kInitializeMutex( &( gs_stWindowManager.stLock ) );

    // 윈도우 리스트 초기화
    kInitializeList( &( gs_stWindowManager.stWindowList ) );
    
    // 이벤트 큐에서 사용할 이벤트 자료구조 풀을 생성
    pstEventBuffer = ( EVENT* ) new char[ sizeof( EVENT ) * 
        EVENTQUEUE_WNIDOWMANAGERMAXCOUNT ];
    if( pstEventBuffer == NULL )
    {
		printf( "Window Manager Event Queue Allocate Fail\n" );
        while( 1 )
        {
            ;
        }
    }
    // 이벤트 큐를 초기화
    kInitializeQueue( &( gs_stWindowManager.stEventQueue ), pstEventBuffer, 
            EVENTQUEUE_WNIDOWMANAGERMAXCOUNT, sizeof( EVENT ) );
    
    // 화면을 업데이트할 때 사용할 비트맵 버퍼를 생성
    // 비트맵은 화면 전체 크기로 생성해서 공용으로 사용하도록 함
    gs_stWindowManager.pbDrawBitmap = (BYTE*)new char[ ( pstModeInfo->wXResolution * 
        pstModeInfo->wYResolution + 7 ) / 8 ];
    if( gs_stWindowManager.pbDrawBitmap == NULL )
    {
        printf( "Draw Bitmap Allocate Fail\n" );
        while( 1 )
        {
            ;
        }
    }
    
    // 마우스 버튼의 상태와 윈도우 이동 여부를 초기화
    gs_stWindowManager.bPreviousButtonStatus = 0;
    gs_stWindowManager.bWindowMoveMode = FALSE;
    gs_stWindowManager.qwMovingWindowID = WINDOW_INVALIDID;

    // 윈도우 크기 변경 정보를 초기화
    gs_stWindowManager.bWindowResizeMode = FALSE;
    gs_stWindowManager.qwResizingWindowID = WINDOW_INVALIDID;
	memset( &( gs_stWindowManager.stResizingWindowArea ), 0, sizeof( RECT ) );

    //--------------------------------------------------------------------------
    // 배경 윈도우 생성
    //--------------------------------------------------------------------------
    // 플래그에 0을 넘겨서 화면에 윈도우를 그리지 않도록 함. 배경 윈도우는 윈도우 내에 
    // 배경색을 모두 칠한 뒤 나타냄
    qwBackgroundWindowID = kCreateWindow( 0, 0, pstModeInfo->wXResolution, 
            pstModeInfo->wYResolution, 0, WINDOW_BACKGROUNDWINDOWTITLE );
    gs_stWindowManager.qwBackgoundWindowID = qwBackgroundWindowID; 

    // 배경 윈도우 내부에 배경색을 채움
    kDrawRect( qwBackgroundWindowID, 0, 0, pstModeInfo->wXResolution - 1, 
            pstModeInfo->wYResolution - 1, WINDOW_COLOR_SYSTEMBACKGROUND, TRUE );
    
    // MINT64 OS의 배경 화면 이미지를 표시
    kDrawBackgroundImage();    
        
    // 배경 윈도우를 화면에 나타냄
    kShowWindow( qwBackgroundWindowID, TRUE );
}

/**
 *  윈도우 매니저를 반환
 */
WINDOWMANAGER* kGetWindowManager( void )
{
    return &gs_stWindowManager;
}

/**
 *  배경 윈도우의 ID를 반환
 */
QWORD kGetBackgroundWindowID( void )
{
    return gs_stWindowManager.qwBackgoundWindowID;
}

/**
 *  화면 영역의 크기를 반환
 */
void kGetScreenArea( RECT* pstScreenArea )
{
	memcpy( pstScreenArea, &( gs_stWindowManager.stScreenArea ), sizeof( RECT ) );
}

/**
 *  윈도우를 생성
 *      윈도우 선택과 선택 해제 이벤트도 같이 전송
 */
QWORD kCreateWindow( int iX, int iY, int iWidth, int iHeight, DWORD dwFlags,
        const char* pcTitle )
{
    WINDOW* pstWindow;
//    TCB* pstTask;
    QWORD qwActiveWindowID;
    EVENT stEvent;

    // 크기가 0인 윈도우는 만들 수 없음
    if( ( iWidth <= 0 ) || ( iHeight <= 0 ) )
    {
        return WINDOW_INVALIDID;
    }
    
    // 윈도우에 제목 표시줄이 있을 경우 최소 크기 변경 버튼과 닫기 버튼이 표시될 자리는
    // 필요하므로 최소 크기 이하일 경우 그 이상으로 설정
    if( dwFlags & WINDOW_FLAGS_DRAWTITLE )
    {
        if( iWidth < WINDOW_WIDTH_MIN )
        {
            iWidth = WINDOW_WIDTH_MIN;            
        }
        
        if( iHeight < WINDOW_HEIGHT_MIN )
        {
            iHeight = WINDOW_HEIGHT_MIN;            
        }
    }

    // 윈도우 자료구조를 할당
    pstWindow = kAllocateWindow();
    if( pstWindow == NULL )
    {
        return WINDOW_INVALIDID;
    }

    // 윈도우 영역 설정
    pstWindow->stArea.iX1 = iX;
    pstWindow->stArea.iY1 = iY;
    pstWindow->stArea.iX2 = iX + iWidth - 1;
    pstWindow->stArea.iY2 = iY + iHeight - 1;
    
    // 윈도우 제목 저장
	memcpy( pstWindow->vcWindowTitle, pcTitle, WINDOW_TITLEMAXLENGTH );
    pstWindow->vcWindowTitle[ WINDOW_TITLEMAXLENGTH ] = '\0';

    // 윈도우 화면 버퍼와 이벤트 큐에서 사용할 이벤트 자료구조 풀을 생성
    pstWindow->pstWindowBuffer = ( COLOR* ) new char[ iWidth * iHeight *
            sizeof( COLOR ) ];
    pstWindow->pstEventBuffer = ( EVENT* ) new char[
        EVENTQUEUE_WINDOWMAXCOUNT * sizeof( EVENT ) ];
    if( ( pstWindow->pstWindowBuffer == NULL ) ||
        ( pstWindow->pstEventBuffer == NULL ) )
    {
        // 윈도우 버퍼와 이벤트 자료구조 풀도 모두 반환
        delete ( pstWindow->pstWindowBuffer );
		delete ( pstWindow->pstEventBuffer );

        // 메모리 할당에 실패하면 윈도우 자료구조 반환
        kFreeWindow( pstWindow->stLink.qwID );
        return WINDOW_INVALIDID;
    }

    // 이벤트 큐를 초기화
    kInitializeQueue( &( pstWindow->stEventQueue ), pstWindow->pstEventBuffer,
            EVENTQUEUE_WINDOWMAXCOUNT, sizeof( EVENT ) );

    // 윈도우를 생성한 태스크의 ID를 저장
//    pstTask = kGetRunningTask( kGetAPICID() );
//    pstWindow->qwTaskID =  pstTask->stLink.qwID;

    // 윈도우 속성 설정
    pstWindow->dwFlags = dwFlags;

    // 윈도우 배경 그리기
    kDrawWindowBackground( pstWindow->stLink.qwID );

    // 윈도우 테두리 그리기
    if( dwFlags & WINDOW_FLAGS_DRAWFRAME )
    {
        kDrawWindowFrame( pstWindow->stLink.qwID );
    }

    // 윈도우 제목 표시줄 그리기
    if( dwFlags & WINDOW_FLAGS_DRAWTITLE )
    {
        kDrawWindowTitle( pstWindow->stLink.qwID, pcTitle, TRUE );
    }

    // 동기화 처리
    //kLock( &( gs_stWindowManager.stLock ) );
	kEnterCriticalSection();

    // 현재 최상위에 있는 윈도우를 반환
    qwActiveWindowID = kGetTopWindowID();
    
    // 윈도우 리스트의 가장 앞에 추가하여 최상위 윈도우로 설정
    kAddListToHeader( &gs_stWindowManager.stWindowList, pstWindow );

	kLeaveCriticalSection();
    // 동기화 처리
    //kUnlock( &( gs_stWindowManager.stLock ) );

    //--------------------------------------------------------------------------
    // 윈도우 이벤트 전송
    //--------------------------------------------------------------------------
    // 윈도우 영역만큼 화면에 업데이트하고 선택되었다는 이벤트를 전송
    kUpdateScreenByID( pstWindow->stLink.qwID );
    kSetWindowEvent( pstWindow->stLink.qwID, EVENT_WINDOW_SELECT, &stEvent );
    kSendEventToWindow( pstWindow->stLink.qwID, &stEvent );
    
    // 이전에 최상위 윈도우가 배경 윈도우가 아니면 이전 최상위 윈도우의 제목 표시줄을
    // 선택되지 않은 것으로 업데이트하고 선택 해제되었다는 이벤트를 전송
    if( qwActiveWindowID != gs_stWindowManager.qwBackgoundWindowID )
    {
        kUpdateWindowTitle( qwActiveWindowID, FALSE );
        kSetWindowEvent( qwActiveWindowID, EVENT_WINDOW_DESELECT, &stEvent );
        kSendEventToWindow( qwActiveWindowID, &stEvent );
    }    
    return pstWindow->stLink.qwID;
}

/**
 *  윈도우를 삭제
 *      윈도우 선택 이벤트도 같이 전송
 */
bool kDeleteWindow( QWORD qwWindowID )
{
    WINDOW* pstWindow;
    RECT stArea;
    QWORD qwActiveWindowID;
	bool bActiveWindow;
    EVENT stEvent;

    // 동기화 처리
    //kLock( &( gs_stWindowManager.stLock ) );
	kEnterCriticalSection();

    
    // 윈도우 검색과 동기화 처리
    pstWindow = kGetWindowWithWindowLock( qwWindowID );
    if( pstWindow == NULL )
    {
        // 동기화 처리
		kLeaveCriticalSection();
        //kUnlock( &( gs_stWindowManager.stLock ) );
        return FALSE;
    }

    // 윈도우를 삭제하기 전에 영역을 저장해둠
    memcpy( &stArea, &( pstWindow->stArea ), sizeof( RECT ) );

    // 윈도우 리스트에서 마지막 윈도우, 즉 최상위 윈도우를 반환
    qwActiveWindowID = kGetTopWindowID();

    // 최상위 윈도우가 지워지는 경우 플래그를 설정
    if( qwActiveWindowID == qwWindowID )
    {
        bActiveWindow = TRUE;
    }
    else
    {
        bActiveWindow = FALSE;
    }
    
    // 윈도우 리스트에서 윈도우 삭제
    if( kRemoveList( &( gs_stWindowManager.stWindowList ), qwWindowID ) == NULL )
    {
        // 동기화 처리
        //kUnlock( &( pstWindow->stLock ) );
        //kUnlock( &( gs_stWindowManager.stLock ) );
		
		kLeaveCriticalSection();
        return FALSE;
    }

    //--------------------------------------------------------------------------
    // 윈도우 화면 버퍼와 이벤트 큐 버퍼를 반환
    //--------------------------------------------------------------------------
    // 윈도우 화면 버퍼를 반환
    delete ( pstWindow->pstWindowBuffer );
    pstWindow->pstWindowBuffer = NULL;
    
    // 윈도우 이벤트 큐 버퍼를 반환
    delete ( pstWindow->pstEventBuffer );
    pstWindow->pstEventBuffer = NULL;

    // 동기화 처리
    //kUnlock( &( pstWindow->stLock ) );
	

    // 윈도우 자료구조를 반환
    kFreeWindow( qwWindowID );

    // 동기화 처리
    //kUnlock( &( gs_stWindowManager.stLock ) );
	kLeaveCriticalSection();

    // 삭제되기 전에 윈도우가 있던 영역을 화면에 다시 업데이트
    kUpdateScreenByScreenArea( &stArea );
    
    //--------------------------------------------------------------------------
    // 최상위 윈도우가 지워졌다면 현재 리스트에서 최상위에 있는 윈도우를 활성화하고
    // 선택되었다는 윈도우 이벤트를 전송
    //--------------------------------------------------------------------------
    if( bActiveWindow == TRUE )
    {
        // 윈도우 리스트에서 마지막 윈도우, 즉 최상위 윈도우를 반환
        qwActiveWindowID = kGetTopWindowID();
        
        // 최상위 윈도우의 제목 표시줄을 활성화된 상태로 표시
        if( qwActiveWindowID != WINDOW_INVALIDID )
        {
            kUpdateWindowTitle( qwActiveWindowID, TRUE );
            
            kSetWindowEvent( qwActiveWindowID, EVENT_WINDOW_SELECT, &stEvent );
            kSendEventToWindow( qwActiveWindowID, &stEvent );   
        }
    }
    return TRUE;
}

/**
 *  태스크 ID가 일치하는 모든 윈도우를 삭제
 */
bool kDeleteAllWindowInTaskID( QWORD qwTaskID )
{
    WINDOW* pstWindow;
    WINDOW* pstNextWindow;

    // 동기화 처리
    //kLock( &( gs_stWindowManager.stLock ) );
	kEnterCriticalSection();

    // 리스트에서 첫 번째 윈도우를 반환
    pstWindow = (WINDOW*)kGetHeaderFromList( &( gs_stWindowManager.stWindowList ) );
    while( pstWindow != NULL )
    {
        // 다음 윈도우를 미리 구함
        pstNextWindow = (WINDOW*)kGetNextFromList( &( gs_stWindowManager.stWindowList ),
                pstWindow );

        // 배경 윈도우가 아니고 태스크 ID가 일치하면 윈도우 삭제
        if( ( pstWindow->stLink.qwID != gs_stWindowManager.qwBackgoundWindowID ) &&
            ( pstWindow->qwTaskID == qwTaskID ) )
        {
            kDeleteWindow( pstWindow->stLink.qwID );
        }

        // 미리 구해둔 다음 윈도우의 값을 설정
        pstWindow = pstNextWindow;
    }

    // 동기화 처리
    //kUnlock( &( gs_stWindowManager.stLock ) );
	kLeaveCriticalSection();
	return TRUE;
}

/**
 *  윈도우 ID로 윈도우 포인터를 반환
 */
WINDOW* kGetWindow( QWORD qwWindowID )
{
    WINDOW* pstWindow;

    // 윈도우 ID의 유효 범위 검사
    if( GETWINDOWOFFSET( qwWindowID ) >= WINDOW_MAXCOUNT )
    {
        return NULL;
    }

    // ID로 윈도우 포인터를 찾은 뒤 ID가 일치하면 반환
    pstWindow = &gs_stWindowPoolManager.pstStartAddress[ GETWINDOWOFFSET( qwWindowID )];
    if( pstWindow->stLink.qwID == qwWindowID )
    {
        return pstWindow;
    }

    return NULL;
}

/**
 *  윈도우 ID로 윈도우 포인터를 찾아 윈도우 뮤텍스를 잠근 뒤 반환
 */
WINDOW* kGetWindowWithWindowLock( QWORD qwWindowID )
{
    WINDOW* pstWindow;
	bool bResult;

    // 윈도우를 검색
    pstWindow = kGetWindow( qwWindowID );
    if( pstWindow == NULL )
    {
        return NULL;
    }
    
    // 동기화 처리한 뒤 다시 윈도우 ID로 윈도우 검색
    //kLock( &(pstWindow->stLock ) );
    // 윈도우 동기화를 한 뒤에 윈도우 ID로 윈도우를 검색할 수 없다면 도중에 윈도우가
    // 바뀐 것이므로 NULL 반환

	//kEnterCriticalSection();
    pstWindow = kGetWindow( qwWindowID );
    if( ( pstWindow == NULL ) || ( pstWindow->pstEventBuffer == NULL ) ||
        ( pstWindow->pstWindowBuffer == NULL ) )                
    {
		//kLeaveCriticalSection();
        // 동기화 처리
        //kUnlock( &(pstWindow->stLock ) );
        return NULL;
    }
    
    return pstWindow;
}

/**
 *  윈도우를 화면에 나타내거나 숨김
 */
bool kShowWindow( QWORD qwWindowID, bool bShow )
{
    WINDOW* pstWindow;
    RECT stWindowArea;

	kEnterCriticalSection();
    // 윈도우 검색과 동기화 처리
    pstWindow = kGetWindowWithWindowLock( qwWindowID );
    if( pstWindow == NULL )
    {
		kLeaveCriticalSection();
        return FALSE;
    }
    
    // 윈도우 속성 설정
    if( bShow == TRUE )
    {
        pstWindow->dwFlags |= WINDOW_FLAGS_SHOW;
    }
    else
    {
        pstWindow->dwFlags &= ~WINDOW_FLAGS_SHOW;
    }

    // 동기화 처리
    //kUnlock( &( pstWindow->stLock ) );
	kLeaveCriticalSection();
    
    // 윈도우가 있던 영역을 다시 업데이트함으로써 윈도우를 나타내거나 숨김
    if( bShow == TRUE )
    {
        kUpdateScreenByID( qwWindowID );
    }
    else
    {
        kGetWindowArea( qwWindowID, &stWindowArea );
        kUpdateScreenByScreenArea( &stWindowArea );
    }
    return TRUE;
}

/**
 *  특정 영역을 포함하는 윈도우는 모두 그림
 *      윈도우 매니저가 호출하는 함수, 유저는 kUpdateScreen()류 함수를 사용
 */
bool kRedrawWindowByArea( const RECT* pstArea, QWORD qwDrawWindowID )
{
    WINDOW* pstWindow;
    WINDOW* pstTargetWindow = NULL;
    RECT stOverlappedArea;
    RECT stCursorArea;
    DRAWBITMAP stDrawBitmap;
    RECT stTempOverlappedArea;
    RECT vstLargestOverlappedArea[ WINDOW_OVERLAPPEDAREALOGMAXCOUNT ];
    int viLargestOverlappedAreaSize[ WINDOW_OVERLAPPEDAREALOGMAXCOUNT ];
    int iTempOverlappedAreaSize;
    int iMinAreaSize;
    int iMinAreaIndex;
    int i;

    // 화면 영역과 겹치는 영역이 없으면 그릴 필요가 없음
    if( kGetOverlappedRectangle( &( gs_stWindowManager.stScreenArea ), pstArea,
            &stOverlappedArea ) == FALSE )
    {
        return FALSE;
    }

    //--------------------------------------------------------------------------
    // Z 순서의 최상위, 즉 윈도우 리스트의 첫 번째부터 마지막까지 루프를 돌면서 
    // 업데이트할 영역과 겹치는 윈도우를 찾아 비디오 메모리로 전송
    //--------------------------------------------------------------------------
    // 화면에 업데이트할 영역을 기록할 공간을 초기화
	memset( viLargestOverlappedAreaSize, 0, sizeof( viLargestOverlappedAreaSize ) );
	memset( vstLargestOverlappedArea, 0, sizeof( vstLargestOverlappedArea ) );

    // 동기화 처리
    //kLock( &( gs_stWindowManager.stLock ) );
	kEnterCriticalSection();

    // 화면에 업데이트할 영역을 저장하는 비트맵을 생성
    kCreateDrawBitmap( &stOverlappedArea, &stDrawBitmap );

    // 현재 윈도우 리스트는 Z 순서에 따라서 정렬 되어 있으므로, 윈도우 리스트를 처음부터
    // 따라가면서 업데이트할 영역을 포함하는 윈도우를 찾아 아래로 내려가며 화면에 전송
    pstWindow = (WINDOW*)kGetHeaderFromList( &( gs_stWindowManager.stWindowList ) );
    while( pstWindow != NULL )
    {
        // 윈도우를 화면에 나타내는 옵션이 설정되어있으며,
        // 업데이트할 부분과 윈도우가 차지하는 영역이 겹치면 겹치는 만큼을 화면에 전송
        if( ( pstWindow->dwFlags & WINDOW_FLAGS_SHOW ) &&
            ( kGetOverlappedRectangle( &( pstWindow->stArea ), &stOverlappedArea,
                                       &stTempOverlappedArea ) == TRUE ) )
        {
            // 윈도우와 겹치는 영역의 넓이를 계산
            iTempOverlappedAreaSize = kGetRectangleWidth( &stTempOverlappedArea ) * 
                kGetRectangleHeight( &stTempOverlappedArea );
            
            // 이전에 기록한 윈도우 업데이트 영역을 검색하여 이전 영역에 포함되는지 확인
            for( i = 0 ; i < WINDOW_OVERLAPPEDAREALOGMAXCOUNT ; i++ )
            {
                // 겹치는 영역을 이전에 저장한 영역과 비교하여 화면을 업데이트할지 
                // 여부를 결정
                // 이전에 업데이트한 영역 중에서 큰 영역에 포함되면 업데이트하지 않음
                if( ( iTempOverlappedAreaSize <= viLargestOverlappedAreaSize[ i ] ) &&
                    ( kIsInRectangle( &( vstLargestOverlappedArea[ i ] ), 
                      stTempOverlappedArea.iX1, stTempOverlappedArea.iY1 ) == TRUE ) &&
                    ( kIsInRectangle( &( vstLargestOverlappedArea[ i ] ), 
                      stTempOverlappedArea.iX2, stTempOverlappedArea.iY2 ) == TRUE ) )
                {
                    break;
                }
            }
            
            // 일치하는 업데이트 영역을 찾았다면 이전에 업데이트 되었다는 뜻이므로
            // 다음 윈도우로 이동        
            if( i < WINDOW_OVERLAPPEDAREALOGMAXCOUNT )
            {
                // 다음 윈도우를 찾음
                pstWindow = (WINDOW*)kGetNextFromList( &( gs_stWindowManager.stWindowList ),
                        pstWindow );
                continue;
            }           
            
            // 현재 영역이 이전에 업데이트했던 가장 큰 영역과 완전히 포함되지 않는다면
            // 넓이를 비교해서 이전에 업데이트한 영역 중 가장 작은 영역을 검색
            iMinAreaSize = 0xFFFFFF;
            iMinAreaIndex = 0;
            for( i = 0 ; i < WINDOW_OVERLAPPEDAREALOGMAXCOUNT ; i++ )
            {
                if( viLargestOverlappedAreaSize[ i ] < iMinAreaSize )
                {
                    iMinAreaSize = viLargestOverlappedAreaSize[ i ];
                    iMinAreaIndex = i;
                }
            }
            
            // 저장된 영역 중에 최소 크기보다 현재 겹치는 영역의 넓이가 크면 해당 위치를
            // 교체하여 영역 크기 순으로 10개를 유지
            if( iMinAreaSize < iTempOverlappedAreaSize )
            {
				memcpy( &( vstLargestOverlappedArea[ iMinAreaIndex ] ),
                         &stTempOverlappedArea, sizeof( RECT ) );
                viLargestOverlappedAreaSize[ iMinAreaIndex ] = iTempOverlappedAreaSize;
            }
            
            // 동기화 처리
            //kLock( &( pstWindow->stLock ) );
			kEnterCriticalSection();

            // 윈도우 ID가 유효하다면 그 전까지 윈도우는 화면에 그리지 않고 
            // 업데이트 비트맵만 업데이트 한 것으로 변경
            if( ( qwDrawWindowID != WINDOW_INVALIDID ) &&
                ( qwDrawWindowID != pstWindow->stLink.qwID ) )
            {
                // 비트맵만 업데이트
                kFillDrawBitmap( &stDrawBitmap, &( pstWindow->stArea ), FALSE );
            }
            else
            {
                // 윈도우 화면 버퍼를 비디오 메모리로 전송
                kCopyWindowBufferToFrameBuffer( pstWindow, &stDrawBitmap );
            }
            // 동기화 처리
            //kUnlock( &( pstWindow->stLock ) );
			kLeaveCriticalSection();
        }
        
        // 모든 영역이 다 업데이트 되었으면 더 이상 그릴 필요 없음
        if( kIsDrawBitmapAllOff( &stDrawBitmap ) == TRUE )
        {
            break;
        }
        
        // 다음 윈도우를 찾음
        pstWindow = (WINDOW*)kGetNextFromList( &( gs_stWindowManager.stWindowList ),
                pstWindow );
    }

    // 동기화 처리
    //kUnlock( &( gs_stWindowManager.stLock ) );
	kLeaveCriticalSection();

    //--------------------------------------------------------------------------
    // 마우스 커서 영역이 포함되면 마우스 커서도 같이 그림
    //--------------------------------------------------------------------------
    // 마우스 영역을 RECT 자료구조에 설정
    kSetRectangleData( gs_stWindowManager.iMouseX, gs_stWindowManager.iMouseY,
            gs_stWindowManager.iMouseX + MOUSE_CURSOR_WIDTH,
            gs_stWindowManager.iMouseY + MOUSE_CURSOR_HEIGHT, &stCursorArea );
    
    // 겹치는지 확인하여 겹친다면 마우스 커서도 그림
    if( kIsRectangleOverlapped( &stOverlappedArea, &stCursorArea ) == TRUE )
    {
        kDrawCursor( gs_stWindowManager.iMouseX, gs_stWindowManager.iMouseY );
    }

	return TRUE;
}

/**
 *  윈도우 화면 버퍼의 일부 또는 전체를 프레임 버퍼로 복사
 */
static void kCopyWindowBufferToFrameBuffer( const WINDOW* pstWindow,
        DRAWBITMAP* pstDrawBitmap )
{
    RECT stTempArea;
    RECT stOverlappedArea;
    int iOverlappedWidth;
    int iOverlappedHeight;
    int iScreenWidth;
    int iWindowWidth;
    int i;
    COLOR* pstCurrentVideoMemoryAddress;
    COLOR* pstCurrentWindowBufferAddress;
    BYTE bTempBitmap;
    int iByteOffset;
    int iBitOffset;
    int iOffsetX;
    int iOffsetY;
    int iLastBitOffset;
    int iBulkCount;

    // 전송해야 하는 영역과 화면 영역이 겹치는 부분을 임시로 계산
    if( kGetOverlappedRectangle( &( gs_stWindowManager.stScreenArea ), 
            &( pstDrawBitmap->stArea ), &stTempArea ) == FALSE )
    {
        return ;
    }

    // 윈도우 영역과 임시로 계산한 영역이 겹치는 부분을 다시 계산
    // 두 영역이 겹치지 않는다면 비디오 메모리로 전송할 필요 없음
    if( kGetOverlappedRectangle( &stTempArea, &( pstWindow->stArea ),
            &stOverlappedArea ) == FALSE )
    {
        return ;
    }

    // 각 영역의 너비와 높이를 계산
    iScreenWidth = kGetRectangleWidth( &( gs_stWindowManager.stScreenArea ) );
    iWindowWidth = kGetRectangleWidth( &( pstWindow->stArea ) );
    iOverlappedWidth = kGetRectangleWidth( &stOverlappedArea );
    iOverlappedHeight = kGetRectangleHeight( &stOverlappedArea );

    //--------------------------------------------------------------------------
    // 겹치는 영역의 높이만큼 출력하는 루프를 반복
    //--------------------------------------------------------------------------
    for( iOffsetY = 0 ; iOffsetY < iOverlappedHeight ; iOffsetY++ )
    {
        // 겹치는 영역이 화면 업데이트 비트맵에서 존재하는 위치를 계산
        if( kGetStartPositionInDrawBitmap( pstDrawBitmap, stOverlappedArea.iX1,
                stOverlappedArea.iY1 + iOffsetY, &iByteOffset, &iBitOffset ) == FALSE )
        {
            break ;
        }

        // 전송을 시작할 비디오 메모리 어드레스와 윈도우 화면 버퍼의 어드레스를 계산
        pstCurrentVideoMemoryAddress = gs_stWindowManager.pstVideoMemory +
            ( stOverlappedArea.iY1 + iOffsetY )* iScreenWidth + stOverlappedArea.iX1;

        // 윈도우 화면 버퍼는 화면 전체가 아닌 윈도우를 기준으로 한 좌표이므로,
        // 겹치는 영역을 윈도우 내부 좌표 기준으로 변환
        pstCurrentWindowBufferAddress = pstWindow->pstWindowBuffer +
            ( stOverlappedArea.iY1 - pstWindow->stArea.iY1 + iOffsetY ) * iWindowWidth +
            ( stOverlappedArea.iX1 - pstWindow->stArea.iX1 );
        
        //----------------------------------------------------------------------
        // 겹친 영역의 너비만큼 출력하는 루프를 반복
        //----------------------------------------------------------------------
        for( iOffsetX = 0 ; iOffsetX < iOverlappedWidth ; )
        {        
            // 8개의 픽셀을 한번에 업데이트할 수 있으면 8픽셀 단위로 처리할 수 있는
            // 크기를 계산하여 한번에 처리
            if( ( pstDrawBitmap->pbBitmap[ iByteOffset ] == 0xFF ) && 
                ( iBitOffset == 0x00 ) &&
                ( ( iOverlappedWidth - iOffsetX ) >= 8 ) )
            {
                // 현재 위치에서 8픽셀 단위로 처리할 수 있는 최대 크기를 계산
                for( iBulkCount = 0 ; 
                     ( iBulkCount < ( ( iOverlappedWidth - iOffsetX ) >> 3 ) ) ;
                     iBulkCount++ )
                {
                    if( pstDrawBitmap->pbBitmap[ iByteOffset + iBulkCount ] != 0xFF )
                    {
                        break;
                    }
                }
                
                // 8픽셀 단위로 한번에 처리
                memcpy( pstCurrentVideoMemoryAddress, pstCurrentWindowBufferAddress,
                        ( sizeof( COLOR ) * iBulkCount ) << 3 );                                

                // 메모리 어드레스와 비트맵 정보를 8픽셀 단위로 업데이트                
                pstCurrentVideoMemoryAddress += iBulkCount << 3;
                pstCurrentWindowBufferAddress += iBulkCount << 3;
				memset( pstDrawBitmap->pbBitmap + iByteOffset, 0x00,
                        iBulkCount );
                
                // 전체 개수에서 8픽셀 단위로 전송한 수만큼 값을 더해줌
                iOffsetX += iBulkCount << 3;
                
                // 비트맵의 오프셋을 변경
                iByteOffset += iBulkCount;
                iBitOffset = 0;
            }
            // 현재 영역이 이미 업데이트 되어 8개의 픽셀을 한번에 제외할 수 있으면
            // 8픽셀 단위로 처리할 수 있는 크기를 계산하여 한번에 처리
            else if( ( pstDrawBitmap->pbBitmap[ iByteOffset ] == 0x00 ) && 
                ( iBitOffset == 0x00 ) &&
                ( ( iOverlappedWidth - iOffsetX ) >= 8 ) )
            {
                // 현재 위치에서 8픽셀 단위로 처리할 수 있는 최대 크기를 계산
                for( iBulkCount = 0 ; 
                     ( iBulkCount < ( ( iOverlappedWidth - iOffsetX ) >> 3 ) ) ;
                     iBulkCount++ )
                {
                    if( pstDrawBitmap->pbBitmap[ iByteOffset + iBulkCount ] != 0x00 )
                    {
                        break;
                    }
                }
                
                // 메모리 어드레스를 변경된 것으로 업데이트                
                pstCurrentVideoMemoryAddress += iBulkCount << 3;
                pstCurrentWindowBufferAddress += iBulkCount << 3;
                
                // 전체 개수에서 8픽셀 단위로 제외한 수만큼 값을 더해줌
                iOffsetX += iBulkCount << 3;
                
                // 비트맵의 오프셋을 변경
                iByteOffset += iBulkCount;
                iBitOffset = 0;                
            }
            else
            {
                // 현재 업데이트할 위치의 비트맵
                bTempBitmap = pstDrawBitmap->pbBitmap[ iByteOffset ];

                // 현재 비트맵에서 출력해야 할 마지막 픽셀의 비트 오프셋을 구함
                iLastBitOffset = MIN( 8, iOverlappedWidth - iOffsetX + iBitOffset );
                
                // 한 점씩 이동시킴
                for( i = iBitOffset ; i < iLastBitOffset ; i++ )
                {
                    // 비트맵이 1로 설정되어 있으면 화면에 출력하고 해당 비트를 0으로 변경
                    if( bTempBitmap & ( 0x01 << i ) )
                    {
                        *pstCurrentVideoMemoryAddress = *pstCurrentWindowBufferAddress;
                        
                        // 비트맵 정보를 변경된 것으로 업데이트
                        bTempBitmap &= ~( 0x01 << i );
                    }
                    
                    // 메모리 어드레스를 변경된 것으로 업데이트
                    pstCurrentVideoMemoryAddress++;
                    pstCurrentWindowBufferAddress++;
                }
                
                // 전체 개수에서 1픽셀 단위로 전송한 수만큼을 값을 더해줌
                iOffsetX += ( iLastBitOffset - iBitOffset );
                
                // 비트맵 정보를 변경된 것으로 업데이트
                pstDrawBitmap->pbBitmap[ iByteOffset ] = bTempBitmap;
                iByteOffset++;
                iBitOffset = 0;
            }
        }
    }
}

/**
 *  특정 위치를 포함하는 윈도우 중에서 가장 위에 있는 윈도우를 반환
 */
QWORD kFindWindowByPoint( int iX, int iY )
{
    QWORD qwWindowID;
    WINDOW* pstWindow;
    
    // 마우스는 배경 윈도우를 벗어나지 못하므로, 기본 값을 배경 윈도우로 설정
    qwWindowID = gs_stWindowManager.qwBackgoundWindowID;
    
    // 동기화 처리
    //kLock( &( gs_stWindowManager.stLock ) );
	kEnterCriticalSection();
    
    // 최상위 윈도우부터 검색 시작
    pstWindow = (WINDOW*)kGetHeaderFromList( &( gs_stWindowManager.stWindowList ) );
    do
    {
        // 윈도우가 화면에 보이고 윈도우가 X, Y 좌표를 포함한다면 윈도우 ID 업데이트
        if( ( pstWindow->dwFlags & WINDOW_FLAGS_SHOW ) &&
            ( kIsInRectangle( &( pstWindow->stArea ), iX, iY ) == TRUE ) )
        {
            qwWindowID = pstWindow->stLink.qwID;
            break;
        }
        
        // 다음 윈도우를 반환
        pstWindow = (WINDOW*)kGetNextFromList( &( gs_stWindowManager.stWindowList ), pstWindow );
    } while( pstWindow != NULL );
    
    // 동기화 처리
    //kUnlock( &( gs_stWindowManager.stLock ) );
	kLeaveCriticalSection();
    return qwWindowID;
}

/**
 *  윈도우 제목이 일치하는 윈도우를 반환
 */
QWORD kFindWindowByTitle( const char* pcTitle )
{
    QWORD qwWindowID;
    WINDOW* pstWindow;
    int iTitleLength;

    qwWindowID = WINDOW_INVALIDID;
    iTitleLength = strlen( pcTitle );
    
    // 동기화 처리
    //kLock( &( gs_stWindowManager.stLock ) );
	kEnterCriticalSection();
    
    // 배경 윈도우부터 검색 시작
    pstWindow = (WINDOW*)kGetHeaderFromList( &( gs_stWindowManager.stWindowList ) );
    while( pstWindow != NULL )
    {
        // 윈도우 제목이 일치하면 루프를 나가서 일치하는 윈도우의 ID를 반환
        if( ( strlen( pstWindow->vcWindowTitle ) == iTitleLength ) &&
            ( memcmp( pstWindow->vcWindowTitle, pcTitle, iTitleLength ) == 0 ) )
        {
            qwWindowID = pstWindow->stLink.qwID;
            break;
        }
        
        // 다음 윈도우를 반환
        pstWindow = (WINDOW*)kGetNextFromList( &( gs_stWindowManager.stWindowList ),
                                      pstWindow );
    }
    
    // 동기화 처리
    //kUnlock( &( gs_stWindowManager.stLock ) );
	kLeaveCriticalSection();
    return qwWindowID;
}

/**
 *  윈도우가 존재하는지 여부를 반환
 */
bool kIsWindowExist( QWORD qwWindowID )
{
    // 윈도우를 검색한 결과가 NULL이라면 윈도우가 존재하지 않음
    if( kGetWindow( qwWindowID ) == NULL )
    {
        return FALSE;
    }
    return TRUE;
}

/**
 *  최상위 윈도우의 ID를 반환
 */
QWORD kGetTopWindowID( void )
{
    WINDOW* pstActiveWindow;
    QWORD qwActiveWindowID;
    
    // 동기화 처리
    //kLock( &( gs_stWindowManager.stLock ) );
	kEnterCriticalSection();
    
    // 윈도우 리스트의 가장 앞에 있는 윈도우를 반환
    pstActiveWindow = ( WINDOW* ) kGetHeaderFromList( &( gs_stWindowManager.stWindowList ) );
    if( pstActiveWindow != NULL )
    {
        qwActiveWindowID = pstActiveWindow->stLink.qwID;
    }
    else
    {
        qwActiveWindowID = WINDOW_INVALIDID;
    }
    
    // 동기화 처리
    //kUnlock( &( gs_stWindowManager.stLock ) );
	kLeaveCriticalSection();
    
    return qwActiveWindowID;
}

/**
 *  윈도우의 Z 순서를 최상위로 만듦
 *      윈도우를 최상위로 이동시킴과 동시에 윈도우 선택과 선택 해제 이벤트를 전송
 */
bool kMoveWindowToTop( QWORD qwWindowID )
{
    WINDOW* pstWindow;
    RECT stArea;
    DWORD dwFlags;
    QWORD qwTopWindowID;
    EVENT stEvent;
    
    // 현재 윈도우 리스트에서 최상위 윈도우, 즉 선택된 윈도우의 ID를 반환
    qwTopWindowID = kGetTopWindowID();    
    // 최상위 윈도우가 자신이면 더 수행할 필요 없음
    if( qwTopWindowID == qwWindowID )
    {
        return TRUE;
    }
    
    // 동기화 처리
    //kLock( &( gs_stWindowManager.stLock ) );
	kEnterCriticalSection();
    // 윈도우 리스트에서 제거하여 윈도우 리스트의 가장 앞으로 이동
    pstWindow = (WINDOW*)kRemoveList( &( gs_stWindowManager.stWindowList ), qwWindowID );
    if( pstWindow != NULL )
    {
        kAddListToHeader( &( gs_stWindowManager.stWindowList ), pstWindow );
        
        // 윈도우의 영역을 윈도우 내부 좌표로 변환하여 플래그와 함께 저장해둠
        // 아래에서 윈도우 화면을 업데이트할 때 사용
        kConvertRectScreenToClient( qwWindowID, &( pstWindow->stArea ), &stArea );
        dwFlags = pstWindow->dwFlags;
    }
    
    // 동기화 처리
    //kUnlock( &( gs_stWindowManager.stLock ) );
	kLeaveCriticalSection();
    // 윈도우가 최상위로 이동했다면 윈도우 제목 표시줄을 화면에 업데이트하고
    // 선택/선택 해제 윈도우 이벤트를 각각 전송
    if( pstWindow != NULL )
    {
        //----------------------------------------------------------------------
        // 선택된 윈도우의 화면을 업데이트하고 윈도우 이벤트 전송
        //----------------------------------------------------------------------
        // 선택되었다는 이벤트를 전송
        kSetWindowEvent( qwWindowID, EVENT_WINDOW_SELECT, &stEvent );
        kSendEventToWindow( qwWindowID, &stEvent );   
        // 제목 표시줄이 있다면 현재 윈도우의 제목 표시줄은 선택된 것으로 만들고 
        // 화면 업데이트
        if( dwFlags & WINDOW_FLAGS_DRAWTITLE )
        {
            // 윈도우 제목 표시줄을 선택된 상태로 업데이트
            kUpdateWindowTitle( qwWindowID, TRUE );
            // 제목 표시줄은 위에서 다시 그렸으므로 제목 표시줄을 제외한 나머지 부분만
            // 화면 업데이트 수행
            stArea.iY1 += WINDOW_TITLEBAR_HEIGHT;
            kUpdateScreenByWindowArea( qwWindowID, &stArea );
        }
        // 제목 표시줄이 없다면 윈도우 영역 전체를 업데이트
        else
        {
            kUpdateScreenByID( qwWindowID );
        }
        
        //----------------------------------------------------------------------
        // 이전에 활성화되었던 윈도우는 제목 표시줄을 비활성화로 만들고 선택 해제되었다는
        // 이벤트를 전송
        //----------------------------------------------------------------------
        // 선택 해제되었다는 이벤트를 전송
        kSetWindowEvent( qwTopWindowID, EVENT_WINDOW_DESELECT, &stEvent );
        kSendEventToWindow( qwTopWindowID, &stEvent );   
        // 제목 표시줄을 선택되지 않은 상태로 업데이트
        kUpdateWindowTitle( qwTopWindowID, FALSE );
        return TRUE;
    }
    
    return FALSE;
}

/**
 *  X, Y좌표가 윈도우의 제목 표시줄 위치에 있는지를 반환
 */
bool kIsInTitleBar( QWORD qwWindowID, int iX, int iY )
{
    WINDOW* pstWindow;
    
    // 윈도우 검색
    pstWindow = kGetWindow( qwWindowID );
    
    // 윈도우가 없거나 윈도우가 제목 표시줄을 가지고 있지 않으면 처리할 필요가 없음
    if( ( pstWindow == NULL ) ||
        ( ( pstWindow->dwFlags & WINDOW_FLAGS_DRAWTITLE ) == 0 ) )
    {
        return FALSE;
    }
    
    // 좌표가 제목 표시줄 영역에 있는지를 비교
    if( ( pstWindow->stArea.iX1 <= iX ) && ( iX <= pstWindow->stArea.iX2 ) &&
        ( pstWindow->stArea.iY1 <= iY ) && 
        ( iY <= pstWindow->stArea.iY1 + WINDOW_TITLEBAR_HEIGHT ) )
    {
        return TRUE;
    }
    
    return FALSE;
}

/**
 *  X, Y좌표가 윈도우의 닫기 버튼 위치에 있는지를 반환
 */
bool kIsInCloseButton( QWORD qwWindowID, int iX, int iY )
{
    WINDOW* pstWindow;
    
    // 윈도우를 검색
    pstWindow = kGetWindow( qwWindowID );
    
    // 윈도우가 없거나 윈도우가 제목 표시줄을 가지고 있지 않으면 처리할 필요가 없음
    if( ( pstWindow == NULL ) &&
        ( ( pstWindow->dwFlags & WINDOW_FLAGS_DRAWTITLE ) == 0 ) )
    {
        return FALSE;
    }
    
    // 좌표가 닫기 버튼 영역에 있는지를 비교
    if( ( ( pstWindow->stArea.iX2 - WINDOW_XBUTTON_SIZE - 1 ) <= iX ) && 
          ( iX <= ( pstWindow->stArea.iX2 - 1 ) ) &&
          ( ( pstWindow->stArea.iY1 + 1 ) <= iY ) && 
          ( iY <= ( pstWindow->stArea.iY1 + 1 + WINDOW_XBUTTON_SIZE ) ) )
    {
        return TRUE;
    }
    
    return FALSE;
}

/**
 *  윈도우를 해당 위치로 이동
 */
bool kMoveWindow( QWORD qwWindowID, int iX, int iY )
{
    WINDOW* pstWindow;
    RECT stPreviousArea;
    int iWidth;
    int iHeight;
    EVENT stEvent;
    
	kEnterCriticalSection();
    // 윈도우 검색과 동기화 처리
    pstWindow = kGetWindowWithWindowLock( qwWindowID );
    if( pstWindow == NULL )
    {
		kLeaveCriticalSection();
        return FALSE;
    }
    
    // 이전에 윈도우가 있던 곳을 저장
	memcpy( &stPreviousArea, &( pstWindow->stArea ), sizeof( RECT ) );

    // 높이와 너비 계산하여 현재 윈도우 위치를 변경
    iWidth = kGetRectangleWidth( &stPreviousArea );
    iHeight = kGetRectangleHeight( &stPreviousArea );
    kSetRectangleData( iX, iY, iX + iWidth - 1, iY + iHeight - 1, 
            &( pstWindow->stArea ) );

    // 동기화 처리
    //kUnlock( &( pstWindow->stLock ) );
	kLeaveCriticalSection();
    
    // 이전 윈도우가 있던 화면 영역을 업데이트
    kUpdateScreenByScreenArea( &stPreviousArea );

    // 현재 윈도우 영역을 화면에 업데이트
    kUpdateScreenByID( qwWindowID );
    
    // 윈도우 이동 메시지를 전송
    kSetWindowEvent( qwWindowID, EVENT_WINDOW_MOVE, &stEvent );
    kSendEventToWindow( qwWindowID, &stEvent );

    return TRUE;
}

/**
 *  윈도우 제목 표시줄을 새로 그림
 */
static bool kUpdateWindowTitle( QWORD qwWindowID, bool bSelectedTitle )
{
    WINDOW* pstWindow;
    RECT stTitleBarArea;
    
    // 윈도우를 검색
    pstWindow = kGetWindow( qwWindowID );

    // 윈도우가 존재하면 윈도우의 제목 표시줄을 다시 그려서 화면에 업데이트    
    if( ( pstWindow != NULL ) &&
        ( pstWindow->dwFlags & WINDOW_FLAGS_DRAWTITLE ) )
    {
        // 선택/선택 해제 여부에 따라 윈도우 제목 표시줄을 다시 그림
        kDrawWindowTitle( pstWindow->stLink.qwID, pstWindow->vcWindowTitle,
                          bSelectedTitle );
        // 윈도우 제목 표시줄의 위치를 윈도우 내부 좌표로 저장
        stTitleBarArea.iX1 = 0;
        stTitleBarArea.iY1 = 0;
        stTitleBarArea.iX2 = kGetRectangleWidth( &( pstWindow->stArea ) ) - 1;
        stTitleBarArea.iY2 = WINDOW_TITLEBAR_HEIGHT;

        // 윈도우 영역만큼 화면에 업데이트
        kUpdateScreenByWindowArea( qwWindowID, &stTitleBarArea );
        
        return TRUE;
    }
    
    return FALSE;
}

/**
 *  윈도우의 크기를 변경
 */
bool kResizeWindow( QWORD qwWindowID, int iX, int iY, int iWidth, int iHeight )
{
    WINDOW* pstWindow;
    COLOR* pstNewWindowBuffer;
    COLOR* pstOldWindowBuffer;
    RECT stPreviousArea;

	kEnterCriticalSection();
    // 윈도우 검색과 동기화 처리
    pstWindow = kGetWindowWithWindowLock( qwWindowID );
    if( pstWindow == NULL )
    {
		kLeaveCriticalSection();
        return FALSE;
    }
    
    // 윈도우에 제목 표시줄이 있을 경우 최소 크기 변경 버튼과 닫기 버튼이 표시될 자리는
    // 필요하므로 최소 크기 이하일 경우 그 이상으로 설정
    if( pstWindow->dwFlags & WINDOW_FLAGS_DRAWTITLE )
    {
        if( iWidth < WINDOW_WIDTH_MIN )
        {
            iWidth = WINDOW_WIDTH_MIN;            
        }
        
        if( iHeight < WINDOW_HEIGHT_MIN )
        {
            iHeight = WINDOW_HEIGHT_MIN;            
        }
    }

    // 새로운 크기의 화면 버퍼를 할당
    pstNewWindowBuffer = ( COLOR* ) new char[ iWidth * iHeight * 
                             sizeof( COLOR ) ];
    if( pstNewWindowBuffer == NULL )
    {
        // 메모리 할당에 실패하면 종료
        //kUnlock( &( pstWindow->stLock ) );
		kLeaveCriticalSection();
        return FALSE;
    }

    // 새로운 화면 버퍼를 설정하고 이전 버퍼를 해제
    pstOldWindowBuffer = pstWindow->pstWindowBuffer;
    pstWindow->pstWindowBuffer = pstNewWindowBuffer;
    delete ( pstOldWindowBuffer );
    
    // 윈도우 크기 정보를 저장하고 새로운 크기로 변경
	memcpy( &stPreviousArea, &( pstWindow->stArea ), sizeof( RECT ) );
    pstWindow->stArea.iX1 = iX;
    pstWindow->stArea.iY1 = iY;
    pstWindow->stArea.iX2 = iX + iWidth - 1;
    pstWindow->stArea.iY2 = iY + iHeight - 1;
    
    // 윈도우 배경 그리기
    kDrawWindowBackground( qwWindowID );

    // 윈도우 테두리 그리기
    if( pstWindow->dwFlags & WINDOW_FLAGS_DRAWFRAME )
    {
        kDrawWindowFrame( qwWindowID );
    }

    // 윈도우 제목 표시줄 그리기
    if( pstWindow->dwFlags & WINDOW_FLAGS_DRAWTITLE )
    {
        kDrawWindowTitle( qwWindowID, pstWindow->vcWindowTitle, TRUE );
    }

    // 동기화 처리
    //kUnlock( &( pstWindow->stLock ) );
	kLeaveCriticalSection();

    // 윈도우를 화면에 표시하는 속성이 있으면 윈도우를 화면에 업데이트
    if( pstWindow->dwFlags & WINDOW_FLAGS_SHOW )
    {
        // 이전 윈도우 영역을 다시 그림
        kUpdateScreenByScreenArea( &stPreviousArea );
        
        // 새로운 영역을 다시 그림
        kShowWindow( qwWindowID, TRUE );
    }
    
    return TRUE;
}

/**
 *  X, Y좌표가 윈도우의 크기 변경 버튼 위에 있는지를 반환
 */
bool kIsInResizeButton( QWORD qwWindowID, int iX, int iY )
{
    WINDOW* pstWindow;
    
    // 윈도우를 검색
    pstWindow = kGetWindow( qwWindowID );
    
    // 윈도우 또는 제목 표시줄이 없거나 크기 변경 속성이 없으면 처리할 필요가 없음
    if( ( pstWindow == NULL ) ||
        ( ( pstWindow->dwFlags & WINDOW_FLAGS_DRAWTITLE ) == 0 ) ||
        ( ( pstWindow->dwFlags & WINDOW_FLAGS_RESIZABLE ) == 0 ) )
    {
        return FALSE;
    }
    
    // 좌표가 윈도우 크기 변경 버튼 영역에 있는지를 비교
    if( ( ( pstWindow->stArea.iX2 - ( WINDOW_XBUTTON_SIZE * 2 ) - 2 ) <= iX ) && 
          ( iX <= ( pstWindow->stArea.iX2 - ( WINDOW_XBUTTON_SIZE * 1 ) - 2 ) ) &&
          ( ( pstWindow->stArea.iY1 + 1 ) <= iY ) && 
          ( iY <= ( pstWindow->stArea.iY1 + 1 + WINDOW_XBUTTON_SIZE ) ) )
    {
        return TRUE;
    }
    
    return FALSE;
}


//==============================================================================
//  화면 좌표 <--> 윈도우 좌표 변환 관련 함수
//==============================================================================
/**
 *  윈도우 영역을 반환
 */
bool kGetWindowArea( QWORD qwWindowID, RECT* pstArea )
{
    WINDOW* pstWindow;
    
	kEnterCriticalSection();
    // 윈도우 검색과 동기화 처리
    pstWindow = kGetWindowWithWindowLock( qwWindowID );
    if( pstWindow == NULL )
    {
		kLeaveCriticalSection();
        return FALSE;
    }

    // 윈도우 영역을 복사해서 반환
	memcpy( pstArea, &( pstWindow->stArea ), sizeof( RECT ) );
    
    // 동기화 처리
	kLeaveCriticalSection();
    //kUnlock( &( pstWindow->stLock ) );
    return TRUE;
}

/**
 *  전체 화면을 기준으로 한 X,Y 좌표를 윈도우 내부 좌표로 변환
 */
bool kConvertPointScreenToClient( QWORD qwWindowID, const POINT* pstXY,
        POINT* pstXYInWindow )
{
    RECT stArea;
    
    // 윈도우 영역을 반환
    if( kGetWindowArea( qwWindowID, &stArea ) == FALSE )
    {
        return FALSE;
    }
    
    pstXYInWindow->iX = pstXY->iX - stArea.iX1;
    pstXYInWindow->iY = pstXY->iY - stArea.iY1;
    return TRUE;
}

/**
 *  윈도우 내부를 기준으로 한 X,Y 좌표를 화면 좌표로 변환
 */
bool kConvertPointClientToScreen( QWORD qwWindowID, const POINT* pstXY,
        POINT* pstXYInScreen )
{
    RECT stArea;
    
    // 윈도우 영역을 반환
    if( kGetWindowArea( qwWindowID, &stArea ) == FALSE )
    {
        return FALSE;
    }
    
    pstXYInScreen->iX = pstXY->iX + stArea.iX1;
    pstXYInScreen->iY = pstXY->iY + stArea.iY1;
    return TRUE;
}

/**
 *  전체 화면을 기준으로 한 사각형 좌표를 윈도우 내부 좌표로 변환
 */
bool kConvertRectScreenToClient( QWORD qwWindowID, const RECT* pstArea,
        RECT* pstAreaInWindow )
{
    RECT stWindowArea;
    
    // 윈도우 영역을 반환
    if( kGetWindowArea( qwWindowID, &stWindowArea ) == FALSE )
    {
        return FALSE;
    }
    
    pstAreaInWindow->iX1 = pstArea->iX1 - stWindowArea.iX1;
    pstAreaInWindow->iY1 = pstArea->iY1 - stWindowArea.iY1;
    pstAreaInWindow->iX2 = pstArea->iX2 - stWindowArea.iX1;
    pstAreaInWindow->iY2 = pstArea->iY2 - stWindowArea.iY1;
    return TRUE;
}

/**
 *  윈도우 내부를 기준으로 한 사각형 좌표를 화면 좌표로 변환
 */
bool kConvertRectClientToScreen( QWORD qwWindowID, const RECT* pstArea,
        RECT* pstAreaInScreen )
{
    RECT stWindowArea;
    
    // 윈도우 영역을 반환
    if( kGetWindowArea( qwWindowID, &stWindowArea ) == FALSE )
    {
        return FALSE;
    }
    
    pstAreaInScreen->iX1 = pstArea->iX1 + stWindowArea.iX1;
    pstAreaInScreen->iY1 = pstArea->iY1 + stWindowArea.iY1;
    pstAreaInScreen->iX2 = pstArea->iX2 + stWindowArea.iX1;
    pstAreaInScreen->iY2 = pstArea->iY2 + stWindowArea.iY1;
    return TRUE;
}

//==============================================================================
//  태스크가 사용하는 화면 업데이트 함수
//==============================================================================
/**
 *  윈도우를 화면에 업데이트
 *      태스크에서 사용하는 함수
 */
bool kUpdateScreenByID( QWORD qwWindowID )
{
    EVENT stEvent;
    WINDOW* pstWindow;
    
    // 윈도우를 검색
    pstWindow = kGetWindow( qwWindowID );
    // 윈도우가 없거나 화면에 보이지 않으면 이벤트를 윈도우 매니저로 전달할 필요 없음
    if( ( pstWindow == NULL ) &&
        ( ( pstWindow->dwFlags & WINDOW_FLAGS_SHOW ) == 0 ) )
    {
        return FALSE; 
    }
    
    // 이벤트 자료구조를 채움. 윈도우 ID를 저장
    stEvent.qwType = EVENT_WINDOWMANAGER_UPDATESCREENBYID;
    stEvent.stWindowEvent.qwWindowID = qwWindowID;
    
    return kSendEventToWindowManager( &stEvent );  
}

/**
 *  윈도우의 내부를 화면에 업데이트
 *      태스크에서 사용하는 함수
 */
bool kUpdateScreenByWindowArea( QWORD qwWindowID, const RECT* pstArea )
{
    EVENT stEvent;
    WINDOW* pstWindow;
    
    // 윈도우를 검색
    pstWindow = kGetWindow( qwWindowID );
    // 윈도우가 없거나 화면에 보이지 않으면 이벤트를 윈도우 매니저로 전달할 필요 없음
    if( ( pstWindow == NULL ) &&
        ( ( pstWindow->dwFlags & WINDOW_FLAGS_SHOW ) == 0 ) )
    {
        return FALSE; 
    }
    
    // 이벤트 자료구조를 채움. 윈도우 ID와 윈도우 내부 영역을 저장
    stEvent.qwType = EVENT_WINDOWMANAGER_UPDATESCREENBYWINDOWAREA;
    stEvent.stWindowEvent.qwWindowID = qwWindowID;
	memcpy( &( stEvent.stWindowEvent.stArea ), pstArea, sizeof( RECT ) );
    
    return kSendEventToWindowManager( &stEvent );  
}

/**
 *  화면 좌표로 화면을 업데이트
 *      태스크에서 사용하는 함수
 */
bool kUpdateScreenByScreenArea( const RECT* pstArea )
{
    EVENT stEvent;
    
    // 이벤트 자료구조를 채움. 윈도우 ID와 화면 영역을 저장
    stEvent.qwType = EVENT_WINDOWMANAGER_UPDATESCREENBYSCREENAREA;
    stEvent.stWindowEvent.qwWindowID = WINDOW_INVALIDID;
	memcpy( &( stEvent.stWindowEvent.stArea ), pstArea, sizeof( RECT ) );
    
    return kSendEventToWindowManager( &stEvent );  
}

//==============================================================================
//  이벤트 큐 관련 함수
//==============================================================================
/**
 *  윈도우로 이벤트를 전송
 */
bool kSendEventToWindow( QWORD qwWindowID, const EVENT* pstEvent )
{
    WINDOW* pstWindow;
	bool bResult;
    
	kEnterCriticalSection();
    // 윈도우 검색과 동기화 처리
    pstWindow = kGetWindowWithWindowLock( qwWindowID );
    if( pstWindow == NULL )
    {
		kLeaveCriticalSection();
        return FALSE;
    }

    bResult = kPutQueue( &( pstWindow->stEventQueue ), pstEvent );
    
    // 동기화 처리
    //kUnlock( &( pstWindow->stLock ) );
	kLeaveCriticalSection();

    return bResult;
}

/**
 *  윈도우의 이벤트 큐에 저장된 이벤트를 수신
 */
bool kReceiveEventFromWindowQueue( QWORD qwWindowID, EVENT* pstEvent )
{
    WINDOW* pstWindow;
	bool bResult;

	kEnterCriticalSection();
    // 윈도우 검색과 동기화 처리
    pstWindow = kGetWindowWithWindowLock( qwWindowID );
    if( pstWindow == NULL )
    {
		kLeaveCriticalSection();
        return FALSE;
    }
    	
    bResult = kGetQueue( &( pstWindow->stEventQueue ), pstEvent );
	kLeaveCriticalSection();
    // 동기화 처리
    //kUnlock( &( pstWindow->stLock ) );
    
    return bResult;
}

/**
 *  윈도우 매니저로 이벤트를 전송
 */
bool kSendEventToWindowManager( const EVENT* pstEvent )
{
	bool bResult = FALSE;
    
    // 큐가 가득 차 있지 않으면 데이터를 넣을 수 있음
    if( kIsQueueFull( &( gs_stWindowManager.stEventQueue ) ) == FALSE )
    {
        // 동기화 처리
        //kLock( &( gs_stWindowManager.stLock ) );
		kEnterCriticalSection();
    
        bResult = kPutQueue( &( gs_stWindowManager.stEventQueue ), pstEvent );
        
        // 동기화 처리
        //kUnlock( &( gs_stWindowManager.stLock ) );
		kLeaveCriticalSection();
    }
    return bResult;    
}

/**
 *  윈도우 매니저의 이벤트 큐에 저장된 이벤트를 수신
 */
bool kReceiveEventFromWindowManagerQueue( EVENT* pstEvent )
{
	bool bResult = FALSE;
    
    // 큐가 비어있지 않으면 데이터를 꺼낼 수 있음
    if( kIsQueueEmpty( &( gs_stWindowManager.stEventQueue ) ) == FALSE )
    {
        // 동기화 처리
        //kLock( &( gs_stWindowManager.stLock ) );
		kEnterCriticalSection();
        bResult = kGetQueue( &( gs_stWindowManager.stEventQueue ), pstEvent );
		kLeaveCriticalSection();
        // 동기화 처리
        //kUnlock( &( gs_stWindowManager.stLock ) );
    }    
    return bResult;
}

/**
 *  마우스 이벤트 자료구조를 설정
 */
bool kSetMouseEvent( QWORD qwWindowID, QWORD qwEventType, int iMouseX, int iMouseY,
        BYTE bButtonStatus, EVENT* pstEvent )
{
    POINT stMouseXYInWindow;
    POINT stMouseXY;
    
    // 이벤트 종류를 확인하여 마우스 이벤트 생성
    switch( qwEventType )
    {
        // 마우스 이벤트 처리
    case EVENT_MOUSE_MOVE:
    case EVENT_MOUSE_LBUTTONDOWN:
    case EVENT_MOUSE_LBUTTONUP:            
    case EVENT_MOUSE_RBUTTONDOWN:
    case EVENT_MOUSE_RBUTTONUP:
    case EVENT_MOUSE_MBUTTONDOWN:
    case EVENT_MOUSE_MBUTTONUP:
        // 마우스의 X, Y좌표를 설정
        stMouseXY.iX = iMouseX;
        stMouseXY.iY = iMouseY;
        
        // 마우스 X, Y좌표를 윈도우 내부 좌표로 변환
        if( kConvertPointScreenToClient( qwWindowID, &stMouseXY, &stMouseXYInWindow ) 
                == FALSE )
        {
            return FALSE;
        }

        // 이벤트 타입 설정
        pstEvent->qwType = qwEventType;
        // 윈도우 ID 설정
        pstEvent->stMouseEvent.qwWindowID = qwWindowID;    
        // 마우스 버튼의 상태 설정
        pstEvent->stMouseEvent.bButtonStatus = bButtonStatus;
        // 마우스 커서의 좌표를 윈도우 내부 좌표로 변환한 값을 설정
		memcpy( &( pstEvent->stMouseEvent.stPoint ), &stMouseXYInWindow,
                sizeof( POINT ) );
        break;
        
    default:
        return FALSE;
        break;
    }    
    return TRUE;
}

/**
 *  윈도우 이벤트 자료구조를 설정
 */
bool kSetWindowEvent( QWORD qwWindowID, QWORD qwEventType, EVENT* pstEvent )
{
    RECT stArea;
    
    // 이벤트 종류를 확인하여 윈도우 이벤트 생성
    switch( qwEventType )
    {
        // 윈도우 이벤트 처리
    case EVENT_WINDOW_SELECT:
    case EVENT_WINDOW_DESELECT:
    case EVENT_WINDOW_MOVE:
    case EVENT_WINDOW_RESIZE:
    case EVENT_WINDOW_CLOSE:
        // 이벤트 타입 설정
        pstEvent->qwType = qwEventType;
        // 윈도우 ID 설정
        pstEvent->stWindowEvent.qwWindowID = qwWindowID;
        // 윈도우 영역을 반환
        if( kGetWindowArea( qwWindowID, &stArea ) == FALSE )
        {
            return FALSE;
        }
        
        // 윈도우의 현재 좌표를 설정
		memcpy( &( pstEvent->stWindowEvent.stArea ), &stArea, sizeof( RECT ) );
        break;
        
    default:
        return FALSE;
        break;
    }    
    return TRUE;
}

/**
 *  키 이벤트 자료구조를 설정
 */
void kSetKeyEvent( QWORD qwWindow, const KEYDATA* pstKeyData, EVENT* pstEvent )
{
    // 눌림 또는 떨어짐 처리
    if( pstKeyData->bFlags & KEY_FLAGS_DOWN )
    {
        pstEvent->qwType = EVENT_KEY_DOWN;
    }
    else
    {
        pstEvent->qwType = EVENT_KEY_UP;
    }
    
    // 키의 각 정보를 설정
    pstEvent->stKeyEvent.bASCIICode = pstKeyData->bASCIICode;
    pstEvent->stKeyEvent.bScanCode = pstKeyData->bScanCode;
    pstEvent->stKeyEvent.bFlags = pstKeyData->bFlags;
}

//==============================================================================
//  윈도우 내부에 그리는 함수와 마우스 커서 관련
//==============================================================================
/**
 *  윈도우 화면 버퍼에 윈도우 테두리 그리기
 */
bool kDrawWindowFrame( QWORD qwWindowID )
{
    WINDOW* pstWindow;
    RECT stArea;
    int iWidth;
    int iHeight;

	kEnterCriticalSection();
    // 윈도우 검색과 동기화 처리
    pstWindow = kGetWindowWithWindowLock( qwWindowID );
    if( pstWindow == NULL )
    {
		kLeaveCriticalSection();
        return FALSE;
    }
    
    // 윈도우의 너비와 높이를 계산
    iWidth = kGetRectangleWidth( &( pstWindow->stArea ) );
    iHeight = kGetRectangleHeight( &( pstWindow->stArea ) );
    // 클리핑 영역 설정
    kSetRectangleData( 0, 0, iWidth - 1, iHeight - 1, &stArea );

    // 윈도우 프레임의 가장자리를 그림, 2 픽셀 두께
    kInternalDrawRect( &stArea, pstWindow->pstWindowBuffer,
            0, 0, iWidth - 1, iHeight - 1, WINDOW_COLOR_FRAME, FALSE );

    kInternalDrawRect( &stArea, pstWindow->pstWindowBuffer,
            1, 1, iWidth - 2, iHeight - 2, WINDOW_COLOR_FRAME, FALSE );

    // 동기화 처리
    //kUnlock( &( pstWindow->stLock ) );
	kLeaveCriticalSection();

    return TRUE;
}


/**
 *  윈도우 화면 버퍼에 배경 그리기
 */
bool kDrawWindowBackground( QWORD qwWindowID )
{
    WINDOW* pstWindow;
    int iWidth;
    int iHeight;
    RECT stArea;
    int iX;
    int iY;

	kEnterCriticalSection();
    // 윈도우 검색과 동기화 처리
    pstWindow = kGetWindowWithWindowLock( qwWindowID );
    if( pstWindow == NULL )
    {
		kLeaveCriticalSection();
        return FALSE;
    }

    // 윈도우의 너비와 높이를 계산
    iWidth = kGetRectangleWidth( &( pstWindow->stArea ) );
    iHeight = kGetRectangleHeight( &( pstWindow->stArea ) );
    // 클리핑 영역 설정
    kSetRectangleData( 0, 0, iWidth - 1, iHeight - 1, &stArea );

    // 윈도우에 제목 표시줄이 있으면 그 아래부터 채움
    if( pstWindow->dwFlags & WINDOW_FLAGS_DRAWTITLE )
    {
        iY = WINDOW_TITLEBAR_HEIGHT;
    }
    else
    {
        iY = 0;
    }

    // 윈도우 테두리를 그리는 옵션이 설정되어 있으면 테두리를 제외한 영역을 채움
    if( pstWindow->dwFlags & WINDOW_FLAGS_DRAWFRAME )
    {
        iX = 2;
    }
    else
    {
        iX = 0;
    }

    // 윈도우의 내부를 채움
    kInternalDrawRect( &stArea, pstWindow->pstWindowBuffer,
            iX, iY, iWidth - 1 - iX, iHeight - 1 - iX, WINDOW_COLOR_BACKGROUND, 
            TRUE );
    
    // 동기화 처리
    //kUnlock( &( pstWindow->stLock ) );
	kLeaveCriticalSection();

    return TRUE;
}

/**
 *  윈도우 화면 버퍼에 윈도우 제목 표시줄 그리기
 */
bool kDrawWindowTitle( QWORD qwWindowID, const char* pcTitle, bool bSelectedTitle )
{
    WINDOW* pstWindow;
    int iWidth;
    int iHeight;
    int iX;
    int iY;
    RECT stArea;
    RECT stButtonArea;
    COLOR stTitleBarColor;

	kEnterCriticalSection();
    // 윈도우 검색과 동기화 처리
    pstWindow = kGetWindowWithWindowLock( qwWindowID );
    if( pstWindow == NULL )
    {
		kLeaveCriticalSection();
        return FALSE;
    }
    
    // 윈도우의 너비와 높이를 계산
    iWidth = kGetRectangleWidth( &( pstWindow->stArea ) );
    iHeight = kGetRectangleHeight( &( pstWindow->stArea ) );
    // 클리핑 영역 설정
    kSetRectangleData( 0, 0, iWidth - 1, iHeight - 1, &stArea );

    //--------------------------------------------------------------------------
    // 제목 표시줄 그리기
    //--------------------------------------------------------------------------
    // 제목 표시줄을 채움
    if( bSelectedTitle == TRUE )
    {
        stTitleBarColor = WINDOW_COLOR_TITLEBARACTIVEBACKGROUND;
    }
    else
    {
        stTitleBarColor = WINDOW_COLOR_TITLEBARINACTIVEBACKGROUND;
    }
    kInternalDrawRect( &stArea, pstWindow->pstWindowBuffer,
            0, 3, iWidth - 1, WINDOW_TITLEBAR_HEIGHT - 1, stTitleBarColor, TRUE );

    // 윈도우 제목을 표시
    kInternalDrawText( &stArea, pstWindow->pstWindowBuffer,
            6, 3, WINDOW_COLOR_TITLEBARTEXT, stTitleBarColor, pcTitle, 
            strlen( pcTitle ) );

    // 제목 표시줄을 입체로 보이게 위쪽의 선을 그림, 2 픽셀 두께
    kInternalDrawLine( &stArea, pstWindow->pstWindowBuffer,
            1, 1, iWidth - 1, 1, WINDOW_COLOR_TITLEBARBRIGHT1 );
    kInternalDrawLine( &stArea, pstWindow->pstWindowBuffer,
            1, 2, iWidth - 1, 2, WINDOW_COLOR_TITLEBARBRIGHT2 );

    kInternalDrawLine( &stArea, pstWindow->pstWindowBuffer,
            1, 2, 1, WINDOW_TITLEBAR_HEIGHT - 1, WINDOW_COLOR_TITLEBARBRIGHT1 );
    kInternalDrawLine( &stArea, pstWindow->pstWindowBuffer,
            2, 2, 2, WINDOW_TITLEBAR_HEIGHT - 1, WINDOW_COLOR_TITLEBARBRIGHT2 );

    // 제목 표시줄의 아래쪽에 선을 그림
    kInternalDrawLine( &stArea, pstWindow->pstWindowBuffer,
            2, WINDOW_TITLEBAR_HEIGHT - 2, iWidth - 2, WINDOW_TITLEBAR_HEIGHT - 2,
            WINDOW_COLOR_TITLEBARUNDERLINE );
    kInternalDrawLine( &stArea, pstWindow->pstWindowBuffer,
            2, WINDOW_TITLEBAR_HEIGHT - 1, iWidth - 2, WINDOW_TITLEBAR_HEIGHT - 1,
            WINDOW_COLOR_TITLEBARUNDERLINE );

    // 동기화 처리
    //kUnlock( &( pstWindow->stLock ) );
	kLeaveCriticalSection();

    //--------------------------------------------------------------------------
    // 닫기 버튼 그리기
    //--------------------------------------------------------------------------
    // 닫기 버튼을 그림, 오른쪽 위에 표시
    stButtonArea.iX1 = iWidth - WINDOW_XBUTTON_SIZE - 1;
    stButtonArea.iY1 = 1;
    stButtonArea.iX2 = iWidth - 2;
    stButtonArea.iY2 = WINDOW_XBUTTON_SIZE - 1;
    kDrawButton( qwWindowID, &stButtonArea, WINDOW_COLOR_BACKGROUND, "", 
            WINDOW_COLOR_BACKGROUND );

	kEnterCriticalSection();
    // 윈도우 검색과 동기화 처리
    pstWindow = kGetWindowWithWindowLock( qwWindowID );
    if( pstWindow == NULL )
    {
		kLeaveCriticalSection(;)
        return FALSE;
    }
    
    // 닫기 버튼 내부에 대각선 X를 3 픽셀로 그림
    kInternalDrawLine( &stArea, pstWindow->pstWindowBuffer,
            iWidth - 2 - 18 + 4, 1 + 4, iWidth - 2 - 4,
            WINDOW_TITLEBAR_HEIGHT - 6, WINDOW_COLOR_XBUTTONLINECOLOR );
    kInternalDrawLine( &stArea, pstWindow->pstWindowBuffer,
            iWidth - 2 - 18 + 5, 1 + 4, iWidth - 2 - 4,
            WINDOW_TITLEBAR_HEIGHT - 7, WINDOW_COLOR_XBUTTONLINECOLOR );
    kInternalDrawLine( &stArea, pstWindow->pstWindowBuffer,
            iWidth - 2 - 18 + 4, 1 + 5, iWidth - 2 - 5,
            WINDOW_TITLEBAR_HEIGHT - 6, WINDOW_COLOR_XBUTTONLINECOLOR );

    kInternalDrawLine( &stArea, pstWindow->pstWindowBuffer,
            iWidth - 2 - 18 + 4, 19 - 4, iWidth - 2 - 4, 1 + 4,
            WINDOW_COLOR_XBUTTONLINECOLOR );
    kInternalDrawLine( &stArea, pstWindow->pstWindowBuffer,
            iWidth - 2 - 18 + 5, 19 - 4, iWidth - 2 - 4, 1 + 5,
            WINDOW_COLOR_XBUTTONLINECOLOR );
    kInternalDrawLine( &stArea, pstWindow->pstWindowBuffer,
            iWidth - 2 - 18 + 4, 19 - 5, iWidth - 2 - 5, 1 + 4,
            WINDOW_COLOR_XBUTTONLINECOLOR );

    // 동기화 처리
    //kUnlock( &( pstWindow->stLock ) );
	kLeaveCriticalSection();

    //--------------------------------------------------------------------------
    // 윈도우 크기 변경 버튼 그리기
    //--------------------------------------------------------------------------
    if( pstWindow->dwFlags & WINDOW_FLAGS_RESIZABLE )
    {
        // 크기 변경 버튼을 그림, 오른쪽 위에 있는 닫기 버튼 옆에 그림
        stButtonArea.iX1 = iWidth - ( WINDOW_XBUTTON_SIZE * 2 ) - 2;
        stButtonArea.iY1 = 1;
        stButtonArea.iX2 = iWidth - WINDOW_XBUTTON_SIZE - 2;
        stButtonArea.iY2 = WINDOW_XBUTTON_SIZE - 1;
        kDrawButton( qwWindowID, &stButtonArea, WINDOW_COLOR_BACKGROUND, "", 
                WINDOW_COLOR_BACKGROUND );
    
		kEnterCriticalSection();
        // 윈도우 검색과 동기화 처리
        pstWindow = kGetWindowWithWindowLock( qwWindowID );
        if( pstWindow == NULL )
        {
			kLeaveCriticalSection();
            return FALSE;
        }
        
        // 크기 변경 버튼 내부에 대각선 화살표를 3픽셀로 표시
        // 가운데 선 표시
        kInternalDrawLine( &stArea, pstWindow->pstWindowBuffer,
                stButtonArea.iX1 + 4, stButtonArea.iY2 - 4, stButtonArea.iX2 - 5, 
                stButtonArea.iY1 + 3, WINDOW_COLOR_XBUTTONLINECOLOR );
        kInternalDrawLine( &stArea, pstWindow->pstWindowBuffer,
                stButtonArea.iX1 + 4, stButtonArea.iY2 - 3, stButtonArea.iX2 - 4, 
                stButtonArea.iY1 + 3, WINDOW_COLOR_XBUTTONLINECOLOR );
        kInternalDrawLine( &stArea, pstWindow->pstWindowBuffer,
                stButtonArea.iX1 + 5, stButtonArea.iY2 - 3, stButtonArea.iX2 - 4, 
                stButtonArea.iY1 + 4, WINDOW_COLOR_XBUTTONLINECOLOR );
    
        // 오른쪽 위의 화살표
        kInternalDrawLine( &stArea, pstWindow->pstWindowBuffer,
                stButtonArea.iX1 + 9, stButtonArea.iY1 + 3, stButtonArea.iX2 - 4, 
                stButtonArea.iY1 + 3, WINDOW_COLOR_XBUTTONLINECOLOR );
        kInternalDrawLine( &stArea, pstWindow->pstWindowBuffer,
                stButtonArea.iX1 + 9, stButtonArea.iY1 + 4, stButtonArea.iX2 - 4, 
                stButtonArea.iY1 + 4, WINDOW_COLOR_XBUTTONLINECOLOR );
        kInternalDrawLine( &stArea, pstWindow->pstWindowBuffer,
                stButtonArea.iX2 - 4, stButtonArea.iY1 + 5, stButtonArea.iX2 - 4, 
                stButtonArea.iY1 + 9, WINDOW_COLOR_XBUTTONLINECOLOR );
        kInternalDrawLine( &stArea, pstWindow->pstWindowBuffer,
                stButtonArea.iX2 - 5, stButtonArea.iY1 + 5, stButtonArea.iX2 - 5, 
                stButtonArea.iY1 + 9, WINDOW_COLOR_XBUTTONLINECOLOR );
    
        // 왼쪽 아래의 화살표
        kInternalDrawLine( &stArea, pstWindow->pstWindowBuffer,
                stButtonArea.iX1 + 4, stButtonArea.iY1 + 8, stButtonArea.iX1 + 4, 
                stButtonArea.iY2 - 3, WINDOW_COLOR_XBUTTONLINECOLOR );
        kInternalDrawLine( &stArea, pstWindow->pstWindowBuffer,
                stButtonArea.iX1 + 5, stButtonArea.iY1 + 8, stButtonArea.iX1 + 5, 
                stButtonArea.iY2 - 3, WINDOW_COLOR_XBUTTONLINECOLOR );
        kInternalDrawLine( &stArea, pstWindow->pstWindowBuffer,
                stButtonArea.iX1 + 6, stButtonArea.iY2 - 4, stButtonArea.iX1 + 10, 
                stButtonArea.iY2 - 4, WINDOW_COLOR_XBUTTONLINECOLOR );
        kInternalDrawLine( &stArea, pstWindow->pstWindowBuffer,
                stButtonArea.iX1 + 6, stButtonArea.iY2 - 3, stButtonArea.iX1 + 10, 
                stButtonArea.iY2 - 3, WINDOW_COLOR_XBUTTONLINECOLOR );
        
        // 동기화 처리
        //kUnlock( &( pstWindow->stLock ) );
		kLeaveCriticalSection();
    }
    return TRUE;
}

/**
 *  윈도우 내부에 버튼 그리기
 */
bool kDrawButton( QWORD qwWindowID, RECT* pstButtonArea, COLOR stBackgroundColor,
        const char* pcText, COLOR stTextColor )
{
    WINDOW* pstWindow;
    RECT stArea;
    int iWindowWidth;
    int iWindowHeight;
    int iTextLength;
    int iTextWidth;
    int iButtonWidth;
    int iButtonHeight;
    int iTextX;
    int iTextY;

	kEnterCriticalSection();
    // 윈도우 검색과 동기화 처리
    pstWindow = kGetWindowWithWindowLock( qwWindowID );
    if( pstWindow == NULL )
    {
		kLeaveCriticalSection();
        return FALSE;
    }
    
    // 윈도우의 너비와 높이를 계산
    iWindowWidth = kGetRectangleWidth( &( pstWindow->stArea ) );
    iWindowHeight = kGetRectangleHeight( &( pstWindow->stArea ) );
    // 클리핑 영역 설정
    kSetRectangleData( 0, 0, iWindowWidth - 1, iWindowHeight - 1, &stArea );

    // 버튼의 배경색을 표시
    kInternalDrawRect( &stArea, pstWindow->pstWindowBuffer,
            pstButtonArea->iX1, pstButtonArea->iY1, pstButtonArea->iX2,
            pstButtonArea->iY2, stBackgroundColor, TRUE );

    // 버튼과 텍스트의 너비와 높이를 계산
    iButtonWidth = kGetRectangleWidth( pstButtonArea );
    iButtonHeight = kGetRectangleHeight( pstButtonArea );
    iTextLength = strlen( pcText );
    iTextWidth = iTextLength * FONT_ENGLISHWIDTH;
    
    // 텍스트가 버튼의 가운데에 위치하도록 출력함
    iTextX = ( pstButtonArea->iX1 + iButtonWidth / 2 ) - iTextWidth / 2;
    iTextY = ( pstButtonArea->iY1 + iButtonHeight / 2 ) - FONT_ENGLISHHEIGHT / 2;
    kInternalDrawText( &stArea, pstWindow->pstWindowBuffer, iTextX, iTextY, 
            stTextColor, stBackgroundColor, pcText, iTextLength );      
    
    // 버튼을 입체로 보이게 테두리를 그림, 2 픽셀 두께로 그림
    // 버튼의 왼쪽과 위는 밝게 표시
    kInternalDrawLine( &stArea, pstWindow->pstWindowBuffer,
            pstButtonArea->iX1, pstButtonArea->iY1, pstButtonArea->iX2,
            pstButtonArea->iY1, WINDOW_COLOR_BUTTONBRIGHT );
    kInternalDrawLine( &stArea, pstWindow->pstWindowBuffer,
            pstButtonArea->iX1, pstButtonArea->iY1 + 1, pstButtonArea->iX2 - 1,
            pstButtonArea->iY1 + 1, WINDOW_COLOR_BUTTONBRIGHT );
    kInternalDrawLine( &stArea, pstWindow->pstWindowBuffer,
            pstButtonArea->iX1, pstButtonArea->iY1, pstButtonArea->iX1,
            pstButtonArea->iY2, WINDOW_COLOR_BUTTONBRIGHT );
    kInternalDrawLine( &stArea, pstWindow->pstWindowBuffer,
            pstButtonArea->iX1 + 1, pstButtonArea->iY1, pstButtonArea->iX1 + 1,
            pstButtonArea->iY2 - 1, WINDOW_COLOR_BUTTONBRIGHT );

    // 버튼의 오른쪽과 아래는 어둡게 표시
    kInternalDrawLine( &stArea, pstWindow->pstWindowBuffer,
            pstButtonArea->iX1 + 1, pstButtonArea->iY2, pstButtonArea->iX2,
            pstButtonArea->iY2, WINDOW_COLOR_BUTTONDARK );
    kInternalDrawLine( &stArea, pstWindow->pstWindowBuffer,
            pstButtonArea->iX1 + 2, pstButtonArea->iY2 - 1, pstButtonArea->iX2,
            pstButtonArea->iY2 - 1, WINDOW_COLOR_BUTTONDARK );
    kInternalDrawLine( &stArea, pstWindow->pstWindowBuffer,
            pstButtonArea->iX2, pstButtonArea->iY1 + 1, pstButtonArea->iX2,
            pstButtonArea->iY2, WINDOW_COLOR_BUTTONDARK );
    kInternalDrawLine( &stArea, pstWindow->pstWindowBuffer,
            pstButtonArea->iX2 - 1, pstButtonArea->iY1 + 2, pstButtonArea->iX2 -1,
            pstButtonArea->iY2, WINDOW_COLOR_BUTTONDARK );
    
    // 동기화 처리
    //kUnlock( &( pstWindow->stLock ) );
	kLeaveCriticalSection();

    return TRUE;
}

// 마우스 커서의 이미지를 저장하는 데이터
static BYTE gs_vwMouseBuffer[ MOUSE_CURSOR_WIDTH * MOUSE_CURSOR_HEIGHT ] =
{
    1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    1, 2, 2, 2, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    1, 2, 2, 2, 2, 2, 2, 2, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0,
    1, 2, 2, 3, 3, 3, 3, 2, 2, 2, 2, 2, 1, 1, 1, 1, 0, 0, 0, 0,
    0, 1, 2, 3, 3, 3, 3, 3, 3, 3, 3, 3, 2, 2, 2, 2, 1, 1, 1, 1,
    0, 1, 2, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 2, 2, 2, 1, 1,
    0, 1, 2, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 2, 2, 2, 1, 1, 0, 0,
    0, 1, 2, 2, 3, 3, 3, 3, 3, 3, 3, 3, 2, 2, 1, 1, 0, 0, 0, 0,
    0, 0, 1, 2, 3, 3, 3, 3, 3, 3, 3, 2, 1, 1, 0, 0, 0, 0, 0, 0,
    0, 0, 1, 2, 3, 3, 3, 3, 3, 3, 3, 2, 1, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 1, 2, 3, 3, 3, 3, 3, 3, 3, 3, 2, 1, 0, 0, 0, 0, 0, 0,
    0, 0, 1, 2, 2, 3, 3, 3, 2, 2, 3, 3, 3, 2, 1, 0, 0, 0, 0, 0,
    0, 0, 0, 1, 2, 3, 3, 2, 1, 1, 2, 3, 2, 2, 2, 1, 0, 0, 0, 0,
    0, 0, 0, 1, 2, 3, 2, 2, 1, 0, 1, 2, 2, 2, 2, 2, 1, 0, 0, 0,
    0, 0, 0, 1, 2, 3, 2, 1, 0, 0, 0, 1, 2, 2, 2, 2, 2, 1, 0, 0,
    0, 0, 0, 1, 2, 2, 2, 1, 0, 0, 0, 0, 1, 2, 2, 2, 2, 2, 1, 0,
    0, 0, 0, 0, 1, 2, 1, 0, 0, 0, 0, 0, 0, 1, 2, 2, 2, 2, 2, 1,
    0, 0, 0, 0, 1, 2, 1, 0, 0, 0, 0, 0, 0, 0, 1, 2, 2, 2, 1, 0,
    0, 0, 0, 0, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 2, 1, 0, 0,
    0, 0, 0, 0, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0,
};

/**
 *  X, Y 위치에 마우스 커서를 출력
 */
static void kDrawCursor( int iX, int iY )
{
    int i;
    int j;
    BYTE* pbCurrentPos;

    // 커서 데이터의 시작 위치를 설정
    pbCurrentPos = gs_vwMouseBuffer;

    // 커서의 너비와 높이만큼 루프를 돌면서 픽셀을 화면에 출력
    for( j = 0 ; j < MOUSE_CURSOR_HEIGHT ; j++ )
    {
        for( i = 0 ; i < MOUSE_CURSOR_WIDTH ; i++ )
        {
            switch( *pbCurrentPos )
            {
                // 0은 출력하지 않음
            case 0:
                // nothing
                break;

                // 가장 바깥쪽 테두리, 검은색으로 출력
            case 1:
                kInternalDrawPixel( &( gs_stWindowManager.stScreenArea ),
                        gs_stWindowManager.pstVideoMemory, i + iX, j + iY,
                        MOUSE_CURSOR_OUTERLINE );
                break;

                // 안쪽과 바깥쪽의 경계, 어두운 녹색으로 출력
            case 2:
                kInternalDrawPixel( &( gs_stWindowManager.stScreenArea ),
                        gs_stWindowManager.pstVideoMemory, i + iX, j + iY,
                        MOUSE_CURSOR_OUTER );
                break;

                // 커서의 안, 밝은 색으로 출력
            case 3:
                kInternalDrawPixel( &( gs_stWindowManager.stScreenArea ),
                        gs_stWindowManager.pstVideoMemory, i + iX, j + iY,
                        MOUSE_CURSOR_INNER );
                break;
            }

            // 커서의 픽셀이 표시됨에 따라 커서 데이터의 위치도 같이 이동
            pbCurrentPos++;
        }
    }
}

/**
 *  마우스 커서를 해당 위치로 이동해서 그려줌
 */
void kMoveCursor( int iX, int iY )
{
    RECT stPreviousArea;

    // 마우스 커서가 화면을 벗어나지 못하도록 보정
    if( iX < gs_stWindowManager.stScreenArea.iX1 )
    {
        iX = gs_stWindowManager.stScreenArea.iX1;
    }
    else if( iX > gs_stWindowManager.stScreenArea.iX2 )
    {
        iX = gs_stWindowManager.stScreenArea.iX2;
    }

    if( iY < gs_stWindowManager.stScreenArea.iY1 )
    {
        iY = gs_stWindowManager.stScreenArea.iY1;
    }
    else if( iY > gs_stWindowManager.stScreenArea.iY2 )
    {
        iY = gs_stWindowManager.stScreenArea.iY2;
    }

    // 동기화 처리
    //kLock( &( gs_stWindowManager.stLock ) );
	kEnterCriticalSection();
    
    // 이전에 마우스 커서가 있던 자리를 저장
    stPreviousArea.iX1 = gs_stWindowManager.iMouseX;
    stPreviousArea.iY1 = gs_stWindowManager.iMouseY;
    stPreviousArea.iX2 = gs_stWindowManager.iMouseX + MOUSE_CURSOR_WIDTH - 1;
    stPreviousArea.iY2 = gs_stWindowManager.iMouseY + MOUSE_CURSOR_HEIGHT - 1;
    
    // 마우스 커서의 새 위치를 저장
    gs_stWindowManager.iMouseX = iX;
    gs_stWindowManager.iMouseY = iY;

    // 동기화 처리
    //kUnlock( &( gs_stWindowManager.stLock ) );
	kLeaveCriticalSection();
    
    // 마우스가 이전에 있던 영역을 다시 그림
    kRedrawWindowByArea( &stPreviousArea, WINDOW_INVALIDID );

    // 새로운 위치에 마우스 커서를 출력
    kDrawCursor( iX, iY );
}

/**
 *  현재 마우스 커서의 위치를 반환
 */
void kGetCursorPosition( int* piX, int* piY )
{
    *piX = gs_stWindowManager.iMouseX;
    *piY = gs_stWindowManager.iMouseY;
}

/**
 *  윈도우 내부에 점 그리기
 */
bool kDrawPixel( QWORD qwWindowID, int iX, int iY, COLOR stColor )
{
    WINDOW* pstWindow;
    RECT stArea;

	kEnterCriticalSection();
    // 윈도우 검색과 동기화 처리
    pstWindow = kGetWindowWithWindowLock( qwWindowID );
    if( pstWindow == NULL )
    {
		kLeaveCriticalSection();
        return FALSE;
    }
    
    // 윈도우 시작 좌표를 0,0으로 하는 좌표로 영역을 변환
    kSetRectangleData( 0, 0, pstWindow->stArea.iX2 - pstWindow->stArea.iX1, 
            pstWindow->stArea.iY2 - pstWindow->stArea.iY1, &stArea );

    // 내부 함수를 호출
    kInternalDrawPixel( &stArea, pstWindow->pstWindowBuffer, iX, iY,
            stColor );

    // 동기화 처리
    //kUnlock( &pstWindow->stLock );
	kLeaveCriticalSection();

    return TRUE;
}


/**
 *  윈도우 내부에 직선 그리기
 */
bool kDrawLine( QWORD qwWindowID, int iX1, int iY1, int iX2, int iY2, COLOR stColor )
{
    WINDOW* pstWindow;
    RECT stArea;

	kEnterCriticalSection();
    // 윈도우 검색과 동기화 처리
    pstWindow = kGetWindowWithWindowLock( qwWindowID );
    if( pstWindow == NULL )
    {
		kLeaveCriticalSection();
        return FALSE;
    }
    
    // 윈도우 시작 좌표를 0,0으로 하는 윈도우 기준 좌표로 영역을 변환
    kSetRectangleData( 0, 0, pstWindow->stArea.iX2 - pstWindow->stArea.iX1, 
            pstWindow->stArea.iY2 - pstWindow->stArea.iY1, &stArea );
    
    // 내부 함수를 호출
    kInternalDrawLine( &stArea, pstWindow->pstWindowBuffer, iX1, iY1,
            iX2, iY2, stColor );

    // 동기화 처리
    //kUnlock( &pstWindow->stLock );
	kLeaveCriticalSection();
    return TRUE;
}

/**
 *  윈도우 내부에 사각형 그리기
 */
bool kDrawRect( QWORD qwWindowID, int iX1, int iY1, int iX2, int iY2,
        COLOR stColor, bool bFill )
{
    WINDOW* pstWindow;
    RECT stArea;
    
	kEnterCriticalSection();
    // 윈도우 검색과 동기화 처리
    pstWindow = kGetWindowWithWindowLock( qwWindowID );
    if( pstWindow == NULL )
    {
		kLeaveCriticalSection();
        return FALSE;
    }

    // 윈도우 시작 좌표를 0,0으로 하는 윈도우 기준 좌표로 영역을 변환
    kSetRectangleData( 0, 0, pstWindow->stArea.iX2 - pstWindow->stArea.iX1, 
            pstWindow->stArea.iY2 - pstWindow->stArea.iY1, &stArea );
    
    // 내부 함수를 호출
    kInternalDrawRect( &stArea, pstWindow->pstWindowBuffer, iX1, iY1,
            iX2, iY2, stColor, bFill );

    // 동기화 처리
    //kUnlock( &pstWindow->stLock );
	kLeaveCriticalSection();
    return TRUE;
}

/**
 *  윈도우 내부에 원 그리기
 */
bool kDrawCircle( QWORD qwWindowID, int iX, int iY, int iRadius, COLOR stColor,
	bool bFill )
{
    WINDOW* pstWindow;
    RECT stArea;
    
	kEnterCriticalSection();
    // 윈도우 검색과 동기화 처리
    pstWindow = kGetWindowWithWindowLock( qwWindowID );
    if( pstWindow == NULL )
    {
		kLeaveCriticalSection();
        return FALSE;
    }
    
    // 윈도우 시작 좌표를 0,0으로 하는 윈도우 기준 좌표로 영역을 변환
    kSetRectangleData( 0, 0, pstWindow->stArea.iX2 - pstWindow->stArea.iX1, 
            pstWindow->stArea.iY2 - pstWindow->stArea.iY1, &stArea );
    
    // 내부 함수를 호출
    kInternalDrawCircle( &stArea, pstWindow->pstWindowBuffer,
            iX, iY, iRadius, stColor, bFill );

    // 동기화 처리
    //kUnlock( &pstWindow->stLock );
	kLeaveCriticalSection();
    return TRUE;
}

/**
 *  윈도우 내부에 문자 출력
 */
bool kDrawText( QWORD qwWindowID, int iX, int iY, COLOR stTextColor,
        COLOR stBackgroundColor, const char* pcString, int iLength )
{
    WINDOW* pstWindow;
    RECT stArea;

	kEnterCriticalSection();
    // 윈도우 검색과 동기화 처리
    pstWindow = kGetWindowWithWindowLock( qwWindowID );
    if( pstWindow == NULL )
    {
		kLeaveCriticalSection();
        return FALSE;
    }
    
    // 윈도우 시작 좌표를 0,0으로 하는 윈도우 기준 좌표로 영역을 변환
    kSetRectangleData( 0, 0, pstWindow->stArea.iX2 - pstWindow->stArea.iX1, 
            pstWindow->stArea.iY2 - pstWindow->stArea.iY1, &stArea );
    
    // 내부 함수를 호출

    kInternalDrawText( &stArea, pstWindow->pstWindowBuffer, iX, iY,
            stTextColor, stBackgroundColor, pcString, iLength );

    // 동기화 처리
    //kUnlock( &pstWindow->stLock );
	kLeaveCriticalSection();
    return TRUE;
}

/**
 *  윈도우 화면 버퍼에 버퍼의 내용을 한번에 전송
 *      X, Y 좌표는 윈도우 내부 버퍼 기준
 */
bool kBitBlt( QWORD qwWindowID, int iX, int iY, COLOR* pstBuffer, int iWidth,
        int iHeight )
{
    WINDOW* pstWindow;
    RECT stWindowArea;
    RECT stBufferArea;
    RECT stOverlappedArea;
    int iWindowWdith;
    int iOverlappedWidth;
    int iOverlappedHeight;
    int i;
    int j;
    int iWindowPosition;
    int iBufferPosition;
    int iStartX;
    int iStartY;
    
	kEnterCriticalSection();
    // 윈도우 검색과 동기화 처리
    pstWindow = kGetWindowWithWindowLock( qwWindowID );
    if( pstWindow == NULL )
    {
		kLeaveCriticalSection();
        return FALSE;
    }
    
    // 윈도우 시작 좌표를 0, 0으로 하는 윈도우 기준 좌표로 영역을 변환
    kSetRectangleData( 0, 0, pstWindow->stArea.iX2 - pstWindow->stArea.iX1, 
            pstWindow->stArea.iY2 - pstWindow->stArea.iY1, &stWindowArea );
    
    // 버퍼 영역의 좌표를 설정
    kSetRectangleData( iX, iY, iX + iWidth - 1, iY + iHeight - 1, &stBufferArea );
    
    // 윈도우 영역과 버퍼 영역의 겹치는 좌표를 계산
    if( kGetOverlappedRectangle( &stWindowArea, &stBufferArea, &stOverlappedArea ) 
            == FALSE )
    {
        // 동기화 처리
        //kUnlock( &pstWindow->stLock );
		kLeaveCriticalSection();
        return FALSE;
    }
    
    // 윈도우 영역과 겹치는 영역의 너비와 높이를 계산
    iWindowWdith = kGetRectangleWidth( &stWindowArea );
    iOverlappedWidth = kGetRectangleWidth( &stOverlappedArea );
    iOverlappedHeight = kGetRectangleHeight( &stOverlappedArea );
    
    // 이미지 출력을 시작할 위치를 결정
    // 윈도우의 시작 좌표를 (0, 0)으로 설정했으므로 출력을 시작하는 좌표가 음수이면 
    // 버퍼의 이미지가 그만큼 잘려서 출력됨
    if( iX < 0 )
    {
        iStartX = iX;
    }
    else
    {
        iStartX = 0;
    }
    
    if( iY < 0 )
    {
        iStartY = iY;
    }
    else
    {
        iStartY = 0;
    }
    
    // 너비와 높이 계산
    for( j = 0 ; j < iOverlappedHeight ; j++ )
    {
        // 화면 버퍼와 전송할 버퍼의 시작 오프셋을 계산
        iWindowPosition = ( iWindowWdith * ( stOverlappedArea.iY1 + j ) ) + 
                            stOverlappedArea.iX1;        
        iBufferPosition = ( iWidth * j + iStartY ) + iStartX;

        // 한 라인씩 복사
		memcpy( pstWindow->pstWindowBuffer + iWindowPosition,
                pstBuffer + iBufferPosition, iOverlappedWidth * sizeof( COLOR ) );
    }    
    
    // 동기화 처리
    //kUnlock( &pstWindow->stLock );
	kLeaveCriticalSection();
    return TRUE;
}


// 배경 화면 이미지 파일이 저장된 데이터 버퍼와 버퍼의 크기
extern unsigned char g_vbWallPaper[];
extern unsigned int size_g_vbWallPaper;

/**
 *  배경 화면 윈도우에 배경 화면 이미지를 출력
 */
void kDrawBackgroundImage( void )
{
    JPEG* pstJpeg;
    COLOR* pstOutputBuffer;
    WINDOWMANAGER* pstWindowManager;
    int i;
    int j;
    int iMiddleX;
    int iMiddleY;
    int iScreenWidth;
    int iScreenHeight;
        
    // 윈도우 매니저를 반환
    pstWindowManager = kGetWindowManager();
    
    // JPEG 자료구조를 할당
    pstJpeg = ( JPEG* ) new char[ sizeof( JPEG ) ];
    
    // JPEG 초기화
    if( kJPEGInit( pstJpeg, g_vbWallPaper, size_g_vbWallPaper ) == FALSE )
    {
        return ;
    }
    
    // 디코딩할 메모리 할당
    pstOutputBuffer = ( COLOR* ) new char[ pstJpeg->width * pstJpeg->height *
                                                  sizeof( COLOR ) ];
    if( pstOutputBuffer == NULL )
    {
        delete ( pstJpeg );
        return ;
    }
    
    // 디코딩 처리
    if( kJPEGDecode( pstJpeg, pstOutputBuffer ) == FALSE )
    {
        // 디코딩이 실패하면 할당받았던 버퍼를 모두 반환
        delete ( pstOutputBuffer );
		delete ( pstJpeg );
        return ;
    }
    
    // 디코딩된 이미지를 윈도우 화면 가운데에 표시
    iScreenWidth = kGetRectangleWidth( &( pstWindowManager->stScreenArea ) );
    iScreenHeight = kGetRectangleHeight( &( pstWindowManager->stScreenArea ) );

    iMiddleX = ( iScreenWidth - pstJpeg->width ) / 2;
    iMiddleY = ( iScreenHeight - pstJpeg->height ) / 2;
    
    // 메모리에서 메모리로 한꺼번에 복사
    kBitBlt( pstWindowManager->qwBackgoundWindowID, iMiddleX, iMiddleY, 
            pstOutputBuffer, pstJpeg->width, pstJpeg->height );    
    
    // 할당받았던 버퍼를 모두 반환
	delete ( pstOutputBuffer );
	delete ( pstJpeg );
}

//==============================================================================
//  화면 업데이트에 사용하는 화면 업데이트 비트맵 관련
//==============================================================================
/**
 *  화면 업데이트에 사용할 비트맵을 생성
 *      좌표는 화면 좌표를 사용
 */
bool kCreateDrawBitmap( const RECT* pstArea, DRAWBITMAP* pstDrawBitmap )
{
    // 화면 영역과 겹치는 부분이 없으면 비트맵을 생성할 필요가 없음
    if( kGetOverlappedRectangle( &( gs_stWindowManager.stScreenArea ), pstArea,
            &( pstDrawBitmap->stArea ) ) == FALSE )
    {
        return FALSE;
    }
 
    // 윈도우 매니저에 있는 화면 업데이트 비트맵 버퍼를 설정
    pstDrawBitmap->pbBitmap = gs_stWindowManager.pbDrawBitmap;
    
    return kFillDrawBitmap( pstDrawBitmap, &( pstDrawBitmap->stArea ), TRUE );
}

/**
 *  화면에 업데이트할 비트맵 영역과 현재 영역이 겹치는 부분에 값을 0 또는 1로 채움
 */
static bool kFillDrawBitmap( DRAWBITMAP* pstDrawBitmap, RECT* pstArea, bool bFill )
{
    RECT stOverlappedArea;
    int iByteOffset;
    int iBitOffset;
    int iAreaSize;
    int iOverlappedWidth;
    int iOverlappedHeight;
    BYTE bTempBitmap;
    int i;
    int iOffsetX;
    int iOffsetY;
    int iBulkCount;
    int iLastBitOffset;
    
    // 업데이트할 영역과 겹치는 부분이 없으면 비트맵 버퍼에 값을 채울 필요 없음
    if( kGetOverlappedRectangle( &( pstDrawBitmap->stArea ), pstArea,
            &stOverlappedArea ) == FALSE )
    {
        return FALSE;
    }
    
    // 겹치는 영역의 너비와 높이를 계산
    iOverlappedWidth = kGetRectangleWidth( &stOverlappedArea );
    iOverlappedHeight = kGetRectangleHeight( &stOverlappedArea );
    
    // 겹치는 영역의 높이만큼 출력하는 루프를 반복
    for( iOffsetY = 0 ; iOffsetY < iOverlappedHeight ; iOffsetY++ )
    {
        // 비트맵 버퍼 내에 라인의 시작 위치를 반환
        if( kGetStartPositionInDrawBitmap( pstDrawBitmap, stOverlappedArea.iX1,
                stOverlappedArea.iY1 + iOffsetY, &iByteOffset, &iBitOffset ) == 
                    FALSE )
        {
            break;
        }
    
        //----------------------------------------------------------------------
        // 겹친 영역의 너비만큼 출력하는 루프를 반복
        //----------------------------------------------------------------------
        for( iOffsetX = 0 ; iOffsetX < iOverlappedWidth ; )
        {        
            // 8픽셀 단위로 처리할 수 있는 크기를 계산하여 한번에 처리
            if( ( iBitOffset == 0x00 ) && ( ( iOverlappedWidth - iOffsetX ) >= 8 ) )
            {
                // 현재 위치에서 8픽셀 단위로 처리할 수 있는 최대 크기를 계산
                iBulkCount = ( iOverlappedWidth - iOffsetX ) >> 3;
                
                // 8픽셀 단위로 한번에 처리
                if( bFill == TRUE )
                {
                    memset( pstDrawBitmap->pbBitmap + iByteOffset, 0xFF, 
                            iBulkCount );
                }
                else
                {
					memset( pstDrawBitmap->pbBitmap + iByteOffset, 0x00,
                            iBulkCount );
                }
                
                // 전체 개수에서 개별적으로 설정한 비트맵의 수만큼을 값을 변경해줌
                iOffsetX += iBulkCount << 3;
                
                // 비트맵의 오프셋을 변경
                iByteOffset += iBulkCount;
                iBitOffset = 0;
            }
            else
            {
                // 현재 비트맵에서 출력해야 할 마지막 픽셀의 비트 오프셋을 계산
                iLastBitOffset = MIN( 8, iOverlappedWidth - iOffsetX + iBitOffset );                

                // 비트맵 생성
                bTempBitmap = 0;
                for( i = iBitOffset ; i < iLastBitOffset ; i++ )
                {
                    bTempBitmap |= ( 0x01 << i );
                }
                
                // 전체 개수에서 8픽셀씩 설정한 비트맵의 수만큼을 값을 변경해줌
                iOffsetX += ( iLastBitOffset - iBitOffset );
                
                // 비트맵 정보를 변경된 것으로 업데이트
                if( bFill == TRUE )
                {
                    pstDrawBitmap->pbBitmap[ iByteOffset ] |= bTempBitmap;
                }
                else
                {
                    pstDrawBitmap->pbBitmap[ iByteOffset ] &= ~( bTempBitmap );
                }
                iByteOffset++;
                iBitOffset = 0;                
            }
        }
    }
    return TRUE;
}

/**
 *  화면 좌표가 화면 업데이트 비트맵 내부에서 시작하는 바이트 오프셋과 비트 오프셋을
 *  반환
 *      좌표는 화면 좌표를 사용
 */
inline bool kGetStartPositionInDrawBitmap( const DRAWBITMAP* pstDrawBitmap,
        int iX, int iY, int* piByteOffset, int* piBitOffset )
{       
    int iWidth;
    int iOffsetX;
    int iOffsetY;
    
    // 비트맵 영역 내부에 좌표가 포함되지 않으면 찾을 필요 없음
    if( kIsInRectangle( &( pstDrawBitmap->stArea ), iX, iY ) == FALSE )
    {
        return FALSE;
    }
    
    // 업데이트 영역 내부의 오프셋을 계산
    iOffsetX = iX - pstDrawBitmap->stArea.iX1;
    iOffsetY = iY - pstDrawBitmap->stArea.iY1;
    // 업데이트할 영역의 너비를 계산
    iWidth = kGetRectangleWidth( &( pstDrawBitmap->stArea ) );
    
    // 바이트 오프셋은 X, Y가 그릴 영역에서 위치한 곳을 8(바이트당 8픽셀)로 나누어 계산
    *piByteOffset = ( iOffsetY * iWidth + iOffsetX ) >> 3;
    // 위에서 계산한 바이트 내에 비트 오프셋은 8로 나눈 나머지로 계산
    *piBitOffset = ( iOffsetY * iWidth + iOffsetX ) & 0x7;
    
    return TRUE;
}

/**
 *  화면에 그릴 비트맵이 모두 0으로 설정되어 더이상 업데이트할 것이 없는지를 반환
 */
inline bool kIsDrawBitmapAllOff( const DRAWBITMAP* pstDrawBitmap )
{
    int iByteCount;
    int iLastBitIndex;
    int iWidth;
    int iHeight;
    int i;
    BYTE* pbTempPosition;
    int iSize;
    
    // 업데이트할 영역의 너비와 높이를 계산
    iWidth = kGetRectangleWidth( &( pstDrawBitmap->stArea) );
    iHeight = kGetRectangleHeight( &( pstDrawBitmap->stArea ) );
    
    // 비트맵의 바이트 수를 계산
    iSize = iWidth * iHeight;
    iByteCount = iSize >> 3;
    
    // 8바이트씩 한번에 비교
    pbTempPosition = pstDrawBitmap->pbBitmap;
    for( i = 0 ; i < ( iByteCount >> 3 ) ; i++ )
    {
        if( *( QWORD* ) ( pbTempPosition ) != 0 )
        {
            return FALSE;
        }
        pbTempPosition += 8;
    }
    
    // 8바이트 단위로 떨어지지 않는 나머지를 비교
    for( i = 0 ; i < ( iByteCount & 0x7 ) ; i++ )
    {
        if( *pbTempPosition != 0 )
        {
            return FALSE;
        }
        pbTempPosition++;
    }
    
    // 전체 크기가 8로 나누어 떨어지지 않는다면 한 바이트가 가득 차지 않은 마지막
    // 바이트가 있으므로 이를 검사
    iLastBitIndex = iSize & 0x7;
    for( i = 0 ; i < iLastBitIndex ; i++ )
    {
        if( *pbTempPosition & ( 0x01 << i ) )
        {
            return FALSE;
        }
    }    
    
    return TRUE;
}
