#pragma once
#include "windef.h"

// 키 큐에 대한 매크로
// 키 큐의 최대 크기
#define KEY_MAXQUEUECOUNT	100

#pragma pack( push, 1 )

////////////////////////////////////////////////////////////////////////////////
// 마우스 큐에 대한 매크로
#define MOUSE_MAXQUEUECOUNT 100

// 버튼의 상태를 나타내는 매크로
#define MOUSE_LBUTTONDOWN   0x01
#define MOUSE_RBUTTONDOWN   0x02
#define MOUSE_MBUTTONDOWN   0x04

// 스캔 코드 테이블을 구성하는 항목
typedef struct kKeyMappingEntryStruct
{
	// Shift 키나 Caps Lock 키와 조합되지 않는 ASCII 코드
	BYTE bNormalCode;

	// Shift 키나 Caps Lock 키와 조합된 ASCII 코드
	BYTE bCombinedCode;
} KEYMAPPINGENTRY;

// 키보드의 상태를 관리하는 자료구조
typedef struct tag_KEYBOARDSTATE
{
	// 자료구조 동기화를 위한 스핀락
	//SPINLOCK stSpinLock;

	// 조합 키 정보
	bool bShiftDown;
	bool bCapsLockOn;
	bool bNumLockOn;
	bool bScrollLockOn;

	// 확장 키를 처리하기 위한 정보
	bool bExtendedCodeIn;
	int iSkipCountForPause;
} KEYBOARDSTATE;

// 키 큐에 삽입할 데이터 구조체
typedef struct kKeyDataStruct
{
	// 키보드에서 전달된 스캔 코드
	BYTE bScanCode;
	// 스캔 코드를 변환한 ASCII 코드
	BYTE bASCIICode;
	// 키 상태를 저장하는 플래그(눌림/떨어짐/확장 키 여부)
	BYTE bFlags;
} KEYDATA;

// PS/2 마우스 패킷을 저장하는 자료구조, 마우스 큐에 삽입하는 데이터
typedef struct kMousePacketStruct
{
	// 버튼 상태와 X, Y 값에 관련된 플래그
	BYTE bButtonStatusAndFlag;
	// X축 이동거리
	int bXMovement;
	// Y축 이동거리
	int bYMovement;
	// 상대좌표값인지 절대좌표값인지를 나타내는 플래그
	BYTE bAbsoluteCoordinate;
} MOUSEDATA;

// 마우스의 상태를 관리하는 자료구조
typedef struct tag_MouseManagerStruct
{
	// 자료구조 동기화를 위한 스핀락
	//    SPINLOCK stSpinLock;    
	// 현재 수신된 데이터의 개수, 마우스 데이터가 3개이므로 0~2의 범위를 계속 반복함
	int iByteCount;
	// 현재 수신 중인 마우스 데이터
	MOUSEDATA stCurrentData;
} MOUSESTATE;

typedef struct tag_LinearBufferInfo
{
	unsigned long* pBuffer;
	unsigned long width;
	unsigned long height;
	unsigned long depth;
	unsigned type;
	bool isDirectVideoBuffer;

} LinearBufferInfo;

#pragma pack( pop )

class I_GUIEngine
{
public:
	virtual bool Initialize() = 0;
	virtual void Update(float deltaTime) = 0;
	virtual void SetLinearBuffer(LinearBufferInfo& linearBufferInfo) = 0;
	virtual bool PutKeyboardQueue(KEYDATA* pData) = 0;
	virtual bool PutMouseQueue(MOUSEDATA* pData) = 0;

	LinearBufferInfo m_linearBufferInfo;

};