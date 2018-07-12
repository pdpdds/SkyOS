
#pragma once
#include "windef.h"
#include "I_GUIEngine.h"

// 마우스 큐에 대한 매크로
#define MOUSE_MAXQUEUECOUNT 100

// 버튼의 상태를 나타내는 매크로
#define MOUSE_LBUTTONDOWN   0x01
#define MOUSE_RBUTTONDOWN   0x02
#define MOUSE_MBUTTONDOWN   0x04

// 마우스의 상태를 관리하는 자료구조
typedef struct kMouseManagerStruct
{
	// 자료구조 동기화를 위한 스핀락
	//    SPINLOCK stSpinLock;    
	// 현재 수신된 데이터의 개수, 마우스 데이터가 3개이므로 0~2의 범위를 계속 반복함
	int iByteCount;
	// 현재 수신 중인 마우스 데이터
	MOUSEDATA stCurrentData;
} MOUSEMANAGER;

#define kEnterCriticalSection()	__asm	PUSHFD	__asm CLI
#define kLeaveCriticalSection()		__asm	POPFD

//#define kEnterCriticalSection()	
//#define kLeaveCriticalSection()	

bool kInitializeMouse( void );
bool kAccumulateMouseDataAndPutQueue( BYTE bMouseData );
bool kGetMouseDataFromMouseQueue( BYTE* pbButtonStatus, int* piRelativeX,int* piRelativeY, bool& bAbsoluteCoordinate);

