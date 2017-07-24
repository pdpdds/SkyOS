/**
 *  file    Mouse.h
 *  date    2009/09/26
 *  author  kkamagui 
 *          Copyright(c)2008 All rights reserved by kkamagui
 *  brief   마우스 디바이스 드라이버에 관련된 헤더 파일
 */

#ifndef __MOUSE_H__
#define __MOUSE_H__

#include "windef.h"
#include "Synchronization.h"

////////////////////////////////////////////////////////////////////////////////
//
// 매크로
//
////////////////////////////////////////////////////////////////////////////////
// 마우스 큐에 대한 매크로
#define MOUSE_MAXQUEUECOUNT 100

// 버튼의 상태를 나타내는 매크로
#define MOUSE_LBUTTONDOWN   0x01
#define MOUSE_RBUTTONDOWN   0x02
#define MOUSE_MBUTTONDOWN   0x04

////////////////////////////////////////////////////////////////////////////////
//
// 구조체
//
////////////////////////////////////////////////////////////////////////////////
// 1바이트로 정렬
#pragma pack( push, 1 )

// PS/2 마우스 패킷을 저장하는 자료구조, 마우스 큐에 삽입하는 데이터
typedef struct kMousePacketStruct
{
    // 버튼 상태와 X, Y 값에 관련된 플래그
    BYTE bButtonStatusAndFlag;    
    // X축 이동거리
    BYTE bXMovement;    
    // Y축 이동거리
    BYTE bYMovement;
} MOUSEDATA;

#pragma pack( pop )

// 마우스의 상태를 관리하는 자료구조
typedef struct kMouseManagerStruct
{
    // 자료구조 동기화를 위한 스핀락
    SPINLOCK stSpinLock;    
    // 현재 수신된 데이터의 개수, 마우스 데이터가 3개이므로 0~2의 범위를 계속 반복함
    int iByteCount;
    // 현재 수신 중인 마우스 데이터
    MOUSEDATA stCurrentData;
} MOUSEMANAGER;

////////////////////////////////////////////////////////////////////////////////
//
//  함수
//
////////////////////////////////////////////////////////////////////////////////
BOOL kInitializeMouse( void );
BOOL kAccumulateMouseDataAndPutQueue( BYTE bMouseData );
BOOL kActivateMouse( void );
void kEnableMouseInterrupt( void );
BOOL kIsMouseDataInOutputBuffer( void );
BOOL kGetMouseDataFromMouseQueue( BYTE* pbButtonStatus, int* piRelativeX, 
        int* piRelativeY );

#endif /*__MOUSE_H__*/
