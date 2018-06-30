/**
 *  file    Window.h
 *  date    2009/09/28
 *  author  kkamagui
 *          Copyright(c)2008 All rights reserved by kkamagui
 *  brief   GUI 시스템에 관련된 함수를 정의한 헤더 파일
 */

#ifndef __WINDOW_H__
#define __WINDOW_H__

#include "windef.h"
#include "2DGraphics.h"
#include "List.h"
#include "Queue.h"
#include "Keyboard.h"

////////////////////////////////////////////////////////////////////////////////
//
// 매크로
//
////////////////////////////////////////////////////////////////////////////////
// 윈도우를 생성할 수 있는 최대 개수
#define WINDOW_MAXCOUNT             2048
// 윈도우 ID로 윈도우 풀 내의 오프셋을 계산하는 매크로
// 하위 32비트가 풀 내의 오프셋을 나타냄
#define GETWINDOWOFFSET( x )        ( ( x ) & 0xFFFFFFFF )
// 윈도우 제목의 최대 길이
#define WINDOW_TITLEMAXLENGTH       40
// 유효하지 않은 윈도우 ID
#define WINDOW_INVALIDID            0xFFFFFFFFFFFFFFFF

// 윈도우의 속성
// 윈도우를 화면에 나타냄
#define WINDOW_FLAGS_SHOW               0x00000001
// 윈도우 테두리 그림
#define WINDOW_FLAGS_DRAWFRAME          0x00000002
// 윈도우 제목 표시줄 그림
#define WINDOW_FLAGS_DRAWTITLE          0x00000004
// 윈도우 크기 변경 버튼을 그림
#define WINDOW_FLAGS_RESIZABLE          0x00000008
// 윈도우 기본 속성, 제목 표시줄과 프레임을 모두 그리고 화면에 윈도우를 보이게 설정
#define WINDOW_FLAGS_DEFAULT        ( WINDOW_FLAGS_SHOW | WINDOW_FLAGS_DRAWFRAME | \
                                      WINDOW_FLAGS_DRAWTITLE )

// 제목 표시줄의 높이
#define WINDOW_TITLEBAR_HEIGHT      21
// 윈도우의 닫기 버튼의 크기
#define WINDOW_XBUTTON_SIZE         19
// 윈도우의 최소 너비, 버튼 2개의 너비에 30픽셀의 여유 공간 확보
#define WINDOW_WIDTH_MIN            ( WINDOW_XBUTTON_SIZE * 2 + 30 )
// 윈도우의 최소 높이, 제목 표시줄의 높이에 30픽셀의 여유 공간 확보
#define WINDOW_HEIGHT_MIN           ( WINDOW_TITLEBAR_HEIGHT + 30 )

// 윈도우의 색깔
#define WINDOW_COLOR_FRAME                      RGB( 109, 218, 22 )
#define WINDOW_COLOR_BACKGROUND                 RGB( 255, 255, 255 )
#define WINDOW_COLOR_TITLEBARTEXT               RGB( 255, 255, 255 )
#define WINDOW_COLOR_TITLEBARACTIVEBACKGROUND   RGB( 79, 204, 11 )
#define WINDOW_COLOR_TITLEBARINACTIVEBACKGROUND RGB( 55, 135, 11 )
#define WINDOW_COLOR_TITLEBARBRIGHT1            RGB( 183, 249, 171 )
#define WINDOW_COLOR_TITLEBARBRIGHT2            RGB( 150, 210, 140 )
#define WINDOW_COLOR_TITLEBARUNDERLINE          RGB( 46, 59, 30 )
#define WINDOW_COLOR_BUTTONBRIGHT               RGB( 229, 229, 229 )
#define WINDOW_COLOR_BUTTONDARK                 RGB( 86, 86, 86 )
#define WINDOW_COLOR_SYSTEMBACKGROUND           RGB( 232, 255, 232 )
#define WINDOW_COLOR_XBUTTONLINECOLOR           RGB( 71, 199, 21 )

// 배경 윈도우의 제목
#define WINDOW_BACKGROUNDWINDOWTITLE            "SYS_BACKGROUND"


// 마우스 커서의 너비와 높이
#define MOUSE_CURSOR_WIDTH                  20
#define MOUSE_CURSOR_HEIGHT                 20

// 커서 이미지의 색깔
#define MOUSE_CURSOR_OUTERLINE              RGB(0, 0, 0 )
#define MOUSE_CURSOR_OUTER                  RGB( 79, 204, 11 )
#define MOUSE_CURSOR_INNER                  RGB( 232, 255, 232 )

// 이벤트 큐의 크기
#define EVENTQUEUE_WINDOWMAXCOUNT           100
#define EVENTQUEUE_WNIDOWMANAGERMAXCOUNT    WINDOW_MAXCOUNT

// 윈도우와 윈도우 매니저 태스크 사이에서 전달되는 이벤트의 종류
// 마우스 이벤트
#define EVENT_UNKNOWN                                   0
#define EVENT_MOUSE_MOVE                                1
#define EVENT_MOUSE_LBUTTONDOWN                         2
#define EVENT_MOUSE_LBUTTONUP                           3
#define EVENT_MOUSE_RBUTTONDOWN                         4
#define EVENT_MOUSE_RBUTTONUP                           5
#define EVENT_MOUSE_MBUTTONDOWN                         6
#define EVENT_MOUSE_MBUTTONUP                           7
// 윈도우 이벤트
#define EVENT_WINDOW_SELECT                             8
#define EVENT_WINDOW_DESELECT                           9
#define EVENT_WINDOW_MOVE                               10
#define EVENT_WINDOW_RESIZE                             11
#define EVENT_WINDOW_CLOSE                              12
// 키 이벤트
#define EVENT_KEY_DOWN                                  13
#define EVENT_KEY_UP                                    14
// 화면 업데이트 이벤트
#define EVENT_WINDOWMANAGER_UPDATESCREENBYID            15
#define EVENT_WINDOWMANAGER_UPDATESCREENBYWINDOWAREA    16
#define EVENT_WINDOWMANAGER_UPDATESCREENBYSCREENAREA    17

// 화면에 업데이트할 때 이전에 업데이트한 영역을 저장해둘 개수
#define WINDOW_OVERLAPPEDAREALOGMAXCOUNT                20

////////////////////////////////////////////////////////////////////////////////
//
// 구조체
//
////////////////////////////////////////////////////////////////////////////////
// 마우스 이벤트 자료구조
typedef struct kMouseEventStruct
{
    // 윈도우 ID
    QWORD qwWindowID;

    // 마우스 X,Y좌표와 버튼의 상태
    POINT stPoint;
    BYTE bButtonStatus;
} MOUSEEVENT;

// 키 이벤트 자료구조
typedef struct kKeyEventStruct
{
    // 윈도우 ID
    QWORD qwWindowID;
    
    // 키의 ASCII 코드와 스캔 코드
    BYTE bASCIICode;
    BYTE bScanCode;    
    
    // 키 플래그
    BYTE bFlags;
} KEYEVENT;

// 윈도우 이벤트 자료구조
typedef struct kWindowEventStruct
{
    // 윈도우 ID
    QWORD qwWindowID;
    
    // 영역 정보
    RECT stArea;
} WINDOWEVENT;

// 이벤트 자료구조
typedef struct kEventStruct
{
    // 이벤트 타입
    QWORD qwType;
    
    // 이벤트 데이터 영역을 정의한 공용체
    union
    {
        // 마우스 이벤트 관련 데이터
        MOUSEEVENT stMouseEvent;

        // 키 이벤트 관련 데이터
        KEYEVENT stKeyEvent;

        // 윈도우 이벤트 관련 데이터
        WINDOWEVENT stWindowEvent;

        // 위의 이벤트 외에 유저 이벤트를 위한 데이터
        QWORD vqwData[ 3 ];
    };
} EVENT;

// 윈도우의 정보를 저장하는 자료구조
typedef struct kWindowStruct
{
    // 다음 데이터의 위치와 현재 윈도우의 ID
    LISTLINK stLink;

    // 자료구조 동기화를 위한 뮤텍스
    //MUTEX stLock;

    // 윈도우 영역 정보
    RECT stArea;

    // 윈도우의 화면 버퍼 어드레스
    COLOR* pstWindowBuffer;

    // 윈도우를 가지고 있는 태스크의 ID
    QWORD qwTaskID;

    // 윈도우 속성
    DWORD dwFlags;

    // 이벤트 큐와 큐에서 사용할 버퍼
    QUEUE stEventQueue;
    EVENT* pstEventBuffer;
    
    // 윈도우 제목
    char vcWindowTitle[ WINDOW_TITLEMAXLENGTH + 1 ];
} WINDOW;

// 윈도우 풀의 상태를 관리하는 자료구조
typedef struct kWindowPoolManagerStruct
{
    // 자료구조 동기화를 위한 뮤텍스
    //MUTEX stLock;

    // 윈도우 풀에 대한 정보
    WINDOW* pstStartAddress;
    int iMaxCount;
    int iUseCount;

    // 윈도우가 할당된 횟수
    int iAllocatedCount;
} WINDOWPOOLMANAGER;

// 윈도우 매니저 자료구조
typedef struct kWindowManagerStruct
{
    // 자료구조 동기화를 위한 뮤텍스
   // MUTEX stLock;

    // 윈도우 리스트
    LIST stWindowList;

    // 현재 마우스 커서의 X, Y좌표
    int iMouseX;
    int iMouseY;

    // 화면 영역 정보
    RECT stScreenArea;

    // 비디오 메모리의 어드레스
    COLOR* pstVideoMemory;

    // 배경 윈도우의 ID
    QWORD qwBackgoundWindowID;
    
    // 이벤트 큐와 큐에서 사용할 버퍼
    QUEUE stEventQueue;
    EVENT* pstEventBuffer;

    // 마우스 버튼의 이전 상태
    BYTE bPreviousButtonStatus;
    
    // 이동 중인 윈도우의 ID와 윈도우 이동 모드
    QWORD qwMovingWindowID;
	bool bWindowMoveMode;
    
    // 윈도우 크기 변경 모드와 크기 변경 중인 윈도우의 ID, 변경 중인 윈도우의 영역
    bool bWindowResizeMode;    
    QWORD qwResizingWindowID;
    RECT stResizingWindowArea;
    
    // 화면 업데이트용 비트맵 버퍼의 어드레스
    BYTE* pbDrawBitmap;
} WINDOWMANAGER;


// 화면에 업데이트할 영역의 비트맵 정보을 저장하는 자료구조
typedef struct kDrawBitmapStruct
{
    // 업데이트할 화면 영역
    RECT stArea;
    // 화면 영역의 정보가 저장된 비트맵의 어드레스
    BYTE* pbBitmap;
} DRAWBITMAP;

////////////////////////////////////////////////////////////////////////////////
//
// 함수
//
////////////////////////////////////////////////////////////////////////////////
// 윈도우 풀 관련
static void kInitializeWindowPool( void );
static WINDOW* kAllocateWindow( void );
static void kFreeWindow( QWORD qwID );

// 윈도우와 윈도우 매니저 관련
void kInitializeGUISystem( void );
WINDOWMANAGER* kGetWindowManager( void );
QWORD kGetBackgroundWindowID( void );
void kGetScreenArea( RECT* pstScreenArea );
QWORD kCreateWindow( int iX, int iY, int iWidth, int iHeight, DWORD dwFlags,
        const char* pcTitle );
bool kDeleteWindow( QWORD qwWindowID );
bool kDeleteAllWindowInTaskID( QWORD qwTaskID );
WINDOW* kGetWindow( QWORD qwWindowID );
WINDOW* kGetWindowWithWindowLock( QWORD qwWindowID );
bool kShowWindow( QWORD qwWindowID, bool bShow );
bool kRedrawWindowByArea( const RECT* pstArea, QWORD qwDrawWindowID );
static void kCopyWindowBufferToFrameBuffer( const WINDOW* pstWindow,
        DRAWBITMAP* pstDrawBitmap );
QWORD kFindWindowByPoint( int iX, int iY );
QWORD kFindWindowByTitle( const char* pcTitle );
bool kIsWindowExist( QWORD qwWindowID );
QWORD kGetTopWindowID( void );
bool kMoveWindowToTop( QWORD qwWindowID );
bool kIsInTitleBar( QWORD qwWindowID, int iX, int iY );
bool kIsInCloseButton( QWORD qwWindowID, int iX, int iY );
bool kMoveWindow( QWORD qwWindowID, int iX, int iY );
static bool kUpdateWindowTitle( QWORD qwWindowID, bool bSelectedTitle );
bool kResizeWindow( QWORD qwWindowID, int iX, int iY, int iWidth, int iHeight );
bool kIsInResizeButton(QWORD qwWindowID, int iX, int iY);

// 좌표 변환 관련
bool kGetWindowArea( QWORD qwWindowID, RECT* pstArea );
bool kConvertPointScreenToClient( QWORD qwWindowID, const POINT* pstXY,
        POINT* pstXYInWindow );
bool kConvertPointClientToScreen( QWORD qwWindowID, const POINT* pstXY,
        POINT* pstXYInScreen );
bool kConvertRectScreenToClient( QWORD qwWindowID, const RECT* pstArea,
        RECT* pstAreaInWindow );
bool kConvertRectClientToScreen( QWORD qwWindowID, const RECT* pstArea,
        RECT* pstAreaInScreen );

// 화면 업데이트 관련
bool kUpdateScreenByID( QWORD qwWindowID );
bool kUpdateScreenByWindowArea( QWORD qwWindowID, const RECT* pstArea );
bool kUpdateScreenByScreenArea( const RECT* pstArea );

//  이벤트 큐 관련
bool kSendEventToWindow( QWORD qwWindowID, const EVENT* pstEvent );
bool kReceiveEventFromWindowQueue( QWORD qwWindowID, EVENT* pstEvent );
bool kSendEventToWindowManager( const EVENT* pstEvent );
bool kReceiveEventFromWindowManagerQueue( EVENT* pstEvent );
bool kSetMouseEvent( QWORD qwWindowID, QWORD qwEventType, int iMouseX, int iMouseY,
        BYTE bButtonStatus, EVENT* pstEvent );
bool kSetWindowEvent( QWORD qwWindowID, QWORD qwEventType, EVENT* pstEvent );
void kSetKeyEvent( QWORD qwWindow, const KEYDATA* pstKeyData, EVENT* pstEvent );

// 윈도우 내부에 그리는 함수와 마우스 커서 관련
bool kDrawWindowFrame( QWORD qwWindowID );
bool kDrawWindowBackground( QWORD qwWindowID );
bool kDrawWindowTitle( QWORD qwWindowID, const char* pcTitle, bool bSelectedTitle );
bool kDrawButton( QWORD qwWindowID, RECT* pstButtonArea, COLOR stBackgroundColor,
        const char* pcText, COLOR stTextColor );
bool kDrawPixel( QWORD qwWindowID, int iX, int iY, COLOR stColor );
bool kDrawLine( QWORD qwWindowID, int iX1, int iY1, int iX2, int iY2, COLOR stColor );
bool kDrawRect( QWORD qwWindowID, int iX1, int iY1, int iX2, int iY2,
        COLOR stColor, bool bFill );
bool kDrawCircle( QWORD qwWindowID, int iX, int iY, int iRadius, COLOR stColor,
	bool bFill );
bool kDrawText( QWORD qwWindowID, int iX, int iY, COLOR stTextColor,
        COLOR stBackgroundColor, const char* pcString, int iLength );
static void kDrawCursor( int iX, int iY );
void kMoveCursor( int iX, int iY );
void kGetCursorPosition( int* piX, int* piY );
bool kBitBlt( QWORD qwWindowID, int iX, int iY, COLOR* pstBuffer, int iWidth,
        int iHeight );
void kDrawBackgroundImage( void );

//  화면 업데이트에 사용하는 화면 업데이트 비트맵 관련
bool kCreateDrawBitmap( const RECT* pstArea, DRAWBITMAP* pstDrawBitmap );
static bool kFillDrawBitmap( DRAWBITMAP* pstDrawBitmap, RECT* pstArea, bool bFill );
inline bool kGetStartPositionInDrawBitmap( const DRAWBITMAP* pstDrawBitmap,
        int iX, int iY, int* piByteOffset, int* piBitOffset );
inline bool kIsDrawBitmapAllOff( const DRAWBITMAP* pstDrawBitmap );
#endif /*__WINDOW_H__*/
