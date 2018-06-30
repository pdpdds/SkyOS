/**
 *  file    Console.h
 *  date    2009/01/31
 *  author  kkamagui 
 *          Copyright(c)2008 All rights reserved by kkamagui
 *  brief   콘솔에 관련된 헤더 파일
 */

#ifndef __CONSOLE_H__
#define __CONSOLE_H__

#include "windef.h"
//#include "Synchronization.h"
#include "Queue.h"
#include "Keyboard.h"

typedef struct kCharactorStruct
{
	BYTE bCharactor;
	BYTE bAttribute;
} CHARACTER;

////////////////////////////////////////////////////////////////////////////////
//
// 매크로
//
////////////////////////////////////////////////////////////////////////////////
// 비디오 메모리의 속성(Attribute) 값 설정
#define CONSOLE_BACKGROUND_BLACK            0x00
#define CONSOLE_BACKGROUND_BLUE             0x10
#define CONSOLE_BACKGROUND_GREEN            0x20
#define CONSOLE_BACKGROUND_CYAN             0x30
#define CONSOLE_BACKGROUND_RED              0x40
#define CONSOLE_BACKGROUND_MAGENTA          0x50
#define CONSOLE_BACKGROUND_BROWN            0x60
#define CONSOLE_BACKGROUND_WHITE            0x70
#define CONSOLE_BACKGROUND_BLINK            0x80
#define CONSOLE_FOREGROUND_DARKBLACK        0x00
#define CONSOLE_FOREGROUND_DARKBLUE         0x01
#define CONSOLE_FOREGROUND_DARKGREEN        0x02
#define CONSOLE_FOREGROUND_DARKCYAN         0x03
#define CONSOLE_FOREGROUND_DARKRED          0x04
#define CONSOLE_FOREGROUND_DARKMAGENTA      0x05
#define CONSOLE_FOREGROUND_DARKBROWN        0x06
#define CONSOLE_FOREGROUND_DARKWHITE        0x07
#define CONSOLE_FOREGROUND_BRIGHTBLACK      0x08
#define CONSOLE_FOREGROUND_BRIGHTBLUE       0x09
#define CONSOLE_FOREGROUND_BRIGHTGREEN      0x0A
#define CONSOLE_FOREGROUND_BRIGHTCYAN       0x0B
#define CONSOLE_FOREGROUND_BRIGHTRED        0x0C
#define CONSOLE_FOREGROUND_BRIGHTMAGENTA    0x0D
#define CONSOLE_FOREGROUND_BRIGHTYELLOW     0x0E
#define CONSOLE_FOREGROUND_BRIGHTWHITE      0x0F
// 기본 문자 색상
#define CONSOLE_DEFAULTTEXTCOLOR            ( CONSOLE_BACKGROUND_BLACK | \
        CONSOLE_FOREGROUND_BRIGHTGREEN )

// 콘솔의 너비(Width)와 높이(Height),그리고 비디오 메모리의 시작 어드레스 설정
#define CONSOLE_WIDTH                       80
#define CONSOLE_HEIGHT                      25
#define CONSOLE_VIDEOMEMORYADDRESS          0xB8000

// 비디오 컨트롤러의 I/O 포트 어드레스와 레지스터
#define VGA_PORT_INDEX                      0x3D4
#define VGA_PORT_DATA                       0x3D5
#define VGA_INDEX_UPPERCURSOR               0x0E
#define VGA_INDEX_LOWERCURSOR               0x0F

// 그래픽 모드에서 사용하는 키 큐에 저장할 수 있는 최대 개수
#define CONSOLE_GUIKEYQUEUE_MAXCOUNT        100     

////////////////////////////////////////////////////////////////////////////////
//
// 구조체
//
////////////////////////////////////////////////////////////////////////////////
// 1바이트로 정렬
#pragma pack( push, 1 )

// 콘솔에 대한 정보를 저장하는 자료구조
typedef struct kConsoleManagerStruct
{
    // 현재 출력할 곳의 오프셋
    int iCurrentPrintOffset;

    // 출력할 화면 버퍼의 어드레스
    CHARACTER* pstScreenBuffer;
    
    // 그래픽 모드에서 사용할 키 큐와 뮤텍스
    QUEUE stKeyQueueForGUI;
//    MUTEX stLock;

    // 셸 태스크를 종료할지 여부
    volatile bool bExit;
} CONSOLEMANAGER;

#pragma pack( pop )

////////////////////////////////////////////////////////////////////////////////
//
// 함수
//
////////////////////////////////////////////////////////////////////////////////
void kInitializeConsole( int iX, int iY );
void kSetCursor( int iX, int iY );
void kGetCursor( int *piX, int *piY );
void kPrintf( const char* pcFormatString, ... );
int kConsolePrintString( const char* pcBuffer );
void kClearScreen( void );
BYTE kGetCh( void );
void kPrintStringXY( int iX, int iY, const char* pcString );
CONSOLEMANAGER* kGetConsoleManager( void );
bool kGetKeyFromGUIKeyQueue( KEYDATA* pstData );
bool kPutKeyToGUIKeyQueue( KEYDATA* pstData );
void kSetConsoleShellExitFlag( bool bFlag );

#endif /*__CONSOLE_H__*/
