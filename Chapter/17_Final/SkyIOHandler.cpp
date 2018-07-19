#include "SkyIOHandler.h"
#include "windef.h"
#include "memory.h"
#include "Hal.h"
#include "SkyAPI.h"
#include "I_GUIEngine.h"
#include "SkyStartOption.h"
#include "SkyGUISystem.h"

// 스캔 코드를 ASCII 코드로 변환하는 테이블
static KEYMAPPINGENTRY gs_vstKeyMappingTable[KEY_MAPPINGTABLEMAXCOUNT] =
{
	/*  0   */{ KEY_NONE        ,   KEY_NONE },
	/*  1   */{ KEY_ESC         ,   KEY_ESC },
	/*  2   */{ '1'             ,   '!' },
	/*  3   */{ '2'             ,   '@' },
	/*  4   */{ '3'             ,   '#' },
	/*  5   */{ '4'             ,   '$' },
	/*  6   */{ '5'             ,   '%' },
	/*  7   */{ '6'             ,   '^' },
	/*  8   */{ '7'             ,   '&' },
	/*  9   */{ '8'             ,   '*' },
	/*  10  */{ '9'             ,   '(' },
	/*  11  */{ '0'             ,   ')' },
	/*  12  */{ '-'             ,   '_' },
	/*  13  */{ '='             ,   '+' },
	/*  14  */{ KEY_BACKSPACE   ,   KEY_BACKSPACE },
	/*  15  */{ KEY_TAB         ,   KEY_TAB },
	/*  16  */{ 'q'             ,   'Q' },
	/*  17  */{ 'w'             ,   'W' },
	/*  18  */{ 'e'             ,   'E' },
	/*  19  */{ 'r'             ,   'R' },
	/*  20  */{ 't'             ,   'T' },
	/*  21  */{ 'y'             ,   'Y' },
	/*  22  */{ 'u'             ,   'U' },
	/*  23  */{ 'i'             ,   'I' },
	/*  24  */{ 'o'             ,   'O' },
	/*  25  */{ 'p'             ,   'P' },
	/*  26  */{ '['             ,   '{' },
	/*  27  */{ ']'             ,   '}' },
	/*  28  */{ '\n'            ,   '\n' },
	/*  29  */{ KEY_CTRL        ,   KEY_CTRL },
	/*  30  */{ 'a'             ,   'A' },
	/*  31  */{ 's'             ,   'S' },
	/*  32  */{ 'd'             ,   'D' },
	/*  33  */{ 'f'             ,   'F' },
	/*  34  */{ 'g'             ,   'G' },
	/*  35  */{ 'h'             ,   'H' },
	/*  36  */{ 'j'             ,   'J' },
	/*  37  */{ 'k'             ,   'K' },
	/*  38  */{ 'l'             ,   'L' },
	/*  39  */{ ';'             ,   ':' },
	/*  40  */{ '\''            ,   '\"' },
	/*  41  */{ '`'             ,   '~' },
	/*  42  */{ KEY_LSHIFT      ,   KEY_LSHIFT },
	/*  43  */{ '\\'            ,   '|' },
	/*  44  */{ 'z'             ,   'Z' },
	/*  45  */{ 'x'             ,   'X' },
	/*  46  */{ 'c'             ,   'C' },
	/*  47  */{ 'v'             ,   'V' },
	/*  48  */{ 'b'             ,   'B' },
	/*  49  */{ 'n'             ,   'N' },
	/*  50  */{ 'm'             ,   'M' },
	/*  51  */{ ','             ,   '<' },
	/*  52  */{ '.'             ,   '>' },
	/*  53  */{ '/'             ,   '?' },
	/*  54  */{ KEY_RSHIFT      ,   KEY_RSHIFT },
	/*  55  */{ '*'             ,   '*' },
	/*  56  */{ KEY_LALT        ,   KEY_LALT },
	/*  57  */{ ' '             ,   ' ' },
	/*  58  */{ KEY_CAPSLOCK    ,   KEY_CAPSLOCK },
	/*  59  */{ KEY_F1          ,   KEY_F1 },
	/*  60  */{ KEY_F2          ,   KEY_F2 },
	/*  61  */{ KEY_F3          ,   KEY_F3 },
	/*  62  */{ KEY_F4          ,   KEY_F4 },
	/*  63  */{ KEY_F5          ,   KEY_F5 },
	/*  64  */{ KEY_F6          ,   KEY_F6 },
	/*  65  */{ KEY_F7          ,   KEY_F7 },
	/*  66  */{ KEY_F8          ,   KEY_F8 },
	/*  67  */{ KEY_F9          ,   KEY_F9 },
	/*  68  */{ KEY_F10         ,   KEY_F10 },
	/*  69  */{ KEY_NUMLOCK     ,   KEY_NUMLOCK },
	/*  70  */{ KEY_SCROLLLOCK  ,   KEY_SCROLLLOCK },

	/*  71  */{ KEY_HOME        ,   '7' },
	/*  72  */{ KEY_UP          ,   '8' },
	/*  73  */{ KEY_PAGEUP      ,   '9' },
	/*  74  */{ '-'             ,   '-' },
	/*  75  */{ KEY_LEFT        ,   '4' },
	/*  76  */{ KEY_CENTER      ,   '5' },
	/*  77  */{ KEY_RIGHT       ,   '6' },
	/*  78  */{ '+'             ,   '+' },
	/*  79  */{ KEY_END         ,   '1' },
	/*  80  */{ KEY_DOWN        ,   '2' },
	/*  81  */{ KEY_PAGEDOWN    ,   '3' },
	/*  82  */{ KEY_INS         ,   '0' },
	/*  83  */{ KEY_DEL         ,   '.' },
	/*  84  */{ KEY_NONE        ,   KEY_NONE },
	/*  85  */{ KEY_NONE        ,   KEY_NONE },
	/*  86  */{ KEY_NONE        ,   KEY_NONE },
	/*  87  */{ KEY_F11         ,   KEY_F11 },
	/*  88  */{ KEY_F12         ,   KEY_F12 }
};

SkyIOHandler* SkyIOHandler::m_inputHandler = nullptr;

void SkyMouseHandler2()
{
	SkyIOHandler::GetInstance()->ProcessMouseInput();	
}

extern void SendEOI();
__declspec(naked) void kSkyMouseHandler2()
{

	_asm {
		PUSHAD
		PUSHFD
		CLI
	}

	_asm
	{
		call SkyMouseHandler2
	}

	SendEOI();

	_asm
	{

		POPFD
		POPAD
		IRETD
	}
}


void SkyKeyboardHandler2()
{
	SkyIOHandler::GetInstance()->ProcessKeyboardInput();
}

__declspec(naked) void kSkyKeyboardHandler2()
{

	_asm {
		PUSHAD
		PUSHFD
		CLI
	}

	_asm
	{
		call SkyKeyboardHandler2
	}

	SendEOI();

	_asm
	{

		POPFD
		POPAD
		IRETD
	}
}

SkyIOHandler::SkyIOHandler()
{
	memset(&m_keyboardState, 0, sizeof(KEYBOARDSTATE));
	memset(&m_mouseState, 0, sizeof(MOUSESTATE));			
	m_pGUIEngine = nullptr;
}

SkyIOHandler::~SkyIOHandler()
{
}

extern void SampleFillRect(ULONG* lfb, int x, int y, int w, int h, int col);
bool SkyIOHandler::Initialize()
{
	SetInterruptVector(0x21, kSkyKeyboardHandler2);
	SetInterruptVector(0x2c, kSkyMouseHandler2);

	if (ActivateMouse() == true)
	{
		
		// 마우스 인터럽트 활성화
		EnableMouseInterrupt();
		return ActivateKeyboard();
	}

	return false;
}

void SkyIOHandler::ProcessKeyboardInput()
{
	BYTE bTemp;

	// 출력 버퍼(포트 0x60)에 수신된 데이터가 있는지 여부를 확인하여 읽은 데이터를 
	// 키 큐 또는 마우스 큐에 삽입
	if (IsOutputBufferFull() == TRUE)
	{
		// 마우스 데이터가 아니면 키 큐에 삽입
		if (IsMouseDataInOutputBuffer() == FALSE)
		{
			// 출력 버퍼(포트 0x60)에서 키 스캔 코드를 읽는 용도의 함수지만 키보드와 마우스
			// 데이터는 출력 버퍼를 공통으로 사용하므로 마우스 데이터를 읽는데도 사용 가능
			bTemp = GetKeyboardScanCode();
			// 키 큐에 삽입
			ConvertScanCodeAndPutQueue(bTemp);
		}
		// 마우스 데이터이면 마우스 큐에 삽입
		else
		{
			// 출력 버퍼(포트 0x60)에서 키 스캔 코드를 읽는 용도의 함수지만 키보드와 마우스
			// 데이터는 출력 버퍼를 공통으로 사용하므로 마우스 데이터를 읽는데도 사용 가능
			bTemp = GetKeyboardScanCode();
			// 마우스 큐에 삽입
			AccumulateMouseDataAndPutQueue(bTemp);
		}
	}

}

void SkyIOHandler::ProcessMouseInput()
{
	BYTE bTemp;

	

	// 출력 버퍼(포트 0x60)에 수신된 데이터가 있는지 여부를 확인하여 읽은 데이터를 
	// 키 큐 또는 마우스 큐에 삽입
	if (IsOutputBufferFull() == TRUE)
	{
		// 마우스 데이터가 아니면 키 큐에 삽입
		if (IsMouseDataInOutputBuffer() == FALSE)
		{
			// 출력 버퍼(포트 0x60)에서 키 스캔 코드를 읽는 용도의 함수지만 키보드와 마우스
			// 데이터는 출력 버퍼를 공통으로 사용하므로 마우스 데이터를 읽는데도 사용 가능
			bTemp = GetKeyboardScanCode();
			// 키 큐에 삽입
			ConvertScanCodeAndPutQueue(bTemp);
		}
		// 마우스 데이터이면 마우스 큐에 삽입
		else
		{
			// 출력 버퍼(포트 0x60)에서 키 스캔 코드를 읽는 용도의 함수지만 키보드와 마우스
			// 데이터는 출력 버퍼를 공통으로 사용하므로 마우스 데이터를 읽는데도 사용 가능
			bTemp = GetKeyboardScanCode();
			// 마우스 큐에 삽입
			AccumulateMouseDataAndPutQueue(bTemp);
		}
	}
}

bool SkyIOHandler::ActivateMouse(void)
{
	int i, j;
	bool bPreviousInterrupt;
	bool bResult;

	// 인터럽트 불가
	bPreviousInterrupt = kSetInterruptFlag(FALSE);

	// 컨트롤 레지스터(포트 0x64)에 마우스 활성화 커맨드(0xA8)을 전달하여 마우스 디바이스 활성화
	OutPortByte(0x64, 0xA8);

	// 컨트롤 레지스터(포트 0x64)에 마우스로 데이터를 전송하는 커맨드(0xD4)를 전달하여
	// 입력 버퍼(포트 0x60)로 전달된 데이터를 마우스로 전송
	OutPortByte(0x64, 0xD4);

	// 입력 버퍼(포트 0x60)가 빌 때까지 기다렸다가 마우스에 활성화 커맨드를 전송
	// 0xFFFF만큼 루프를 수행할 시간이면 충분히 커맨드가 전송될 수 있음
	// 0xFFFF 루프를 수행한 이후에도 입력 버퍼(포트 0x60)가 비지 않으면 무시하고 전송
	for (i = 0; i < 0xFFFF; i++)
	{
		// 입력 버퍼(포트 0x60)가 비어있으면 키보드 커맨드 전송 가능
		if (IsInputBufferFull() == FALSE)
		{
			break;
		}
	}

	// 입력 버퍼(포트 0x60)로 마우스 활성화(0xF4) 커맨드를 전달하여 마우스로 전송
	OutPortByte(0x60, 0xF4);

	// ACK가 올 때까지 대기함
	bResult = WaitForACKAndPutOtherScanCode();

	// 이전 인터럽트 상태 복원
	kSetInterruptFlag(bPreviousInterrupt);
	return bResult;
}

bool SkyIOHandler::IsInputBufferFull(void)
{
	// 상태 레지스터(포트 0x64)에서 읽은 값에 입력 버퍼 상태 비트(비트 1)가
	// 1로 설정되어 있으면 아직 키보드가 데이터를 가져가지 않았음
	if (InPortByte(0x64) & 0x02)
	{
		return TRUE;
	}
	return FALSE;
}

bool SkyIOHandler::IsOutputBufferFull(void)
{
	// 상태 레지스터(포트 0x64)에서 읽은 값에 출력 버퍼 상태 비트(비트 0)가
	// 1로 설정되어 있으면 출력 버퍼에 키보드가 전송한 데이터가 존재함
	if (InPortByte(0x64) & 0x01)
	{
		return TRUE;
	}
	return FALSE;
}

void SkyIOHandler::EnableMouseInterrupt(void)
{
	BYTE bOutputPortData;
	int i;

	// 커맨드 바이트 읽기
	// 컨트롤 레지스터(포트 0x64)에 키보드 컨트롤러의 커맨드 바이트를 읽는 커맨드(0x20) 전송
	OutPortByte(0x64, 0x20);

	// 출력 포트의 데이터를 기다렸다가 읽음
	for (i = 0; i < 0xFFFF; i++)
	{
		// 출력 버퍼(포트 0x60)가 차있으면 데이터를 읽을 수 있음
		if (IsOutputBufferFull() == TRUE)
		{
			break;
		}
	}
	// 출력 포트(포트 0x60)에 수신된 커맨드 바이트 값을 읽음
	bOutputPortData = InPortByte(0x60);


	// 마우스 인터럽트 비트 활성화한 뒤 커맨드 바이트 전송
	// 마우스 인터럽트 비트(비트 1) 설정
	bOutputPortData |= 0x02;

	// 커맨드 레지스터(0x64)에 커맨드 바이트를 쓰는 커맨드(0x60)을 전달
	OutPortByte(0x64, 0x60);

	// 입력 버퍼(포트 0x60)에 데이터가 비어있으면 출력 포트에 값을 쓰는 커맨드와 커맨드 바이트 전송
	for (i = 0; i < 0xFFFF; i++)
	{
		// 입력 버퍼(포트 0x60)가 비었으면 커맨드 전송 가능
		if (IsInputBufferFull() == FALSE)
		{
			break;
		}
	}

	// 입력 버퍼(0x60)에 마우스 인터럽트 비트가 1로 설정된 값을 전달
	OutPortByte(0x60, bOutputPortData);
}

/**
*  마우스 데이터가 출력 버퍼에 있는지를 반환
*/
bool SkyIOHandler::IsMouseDataInOutputBuffer(void)
{
	// 출력 버퍼(포트 0x60을 읽기 전에 먼저 상태 레지스터(포트 0x64)를 읽어서
	// 마우스 데이터인가를 확인, 마우스 데이터는 AUXB 비트(비트 5)가 1로 설정됨
	if (InPortByte(0x64) & 0x20)
	{
		return TRUE;
	}

	return false;
}

/**
*  ACK를 기다림
*      ACK가 아닌 다른 코드는 키보드 데이터와 마우스 데이터를 구분하여 큐에 삽입
*/
bool SkyIOHandler::WaitForACKAndPutOtherScanCode(void)
{
	int i, j;
	BYTE bData;
	bool bResult = FALSE;
	bool bMouseData;

	// ACK가 오기 전에 키보드 출력 버퍼(포트 0x60)에 키 데이터가 저장되어 있을 수 있으므로
	// 키보드에서 전달된 데이터를 최대 100개까지 수신하여 ACK를 확인
	for (j = 0; j < 100; j++)
	{
		// 0xFFFF만큼 루프를 수행할 시간이면 충분히 커맨드의 응답이 올 수 있음
		// 0xFFFF 루프를 수행한 이후에도 출력 버퍼(포트 0x60)가 차 있지 않으면 무시하고 읽음
		for (i = 0; i < 0xFFFF; i++)
		{
			// 출력 버퍼(포트 0x60)가 차있으면 데이터를 읽을 수 있음
			if (IsOutputBufferFull() == TRUE)
			{
				break;
			}
		}

		// 출력 버퍼(포트 0x60을 읽기 전에 먼저 상태 레지스터(포트 0x64)를 읽어서
		// 마우스 데이터인지를 확인
		if (IsMouseDataInOutputBuffer() == TRUE)
		{
			bMouseData = TRUE;
		}
		else
		{
			bMouseData = FALSE;
		}

		// 출력 버퍼(포트 0x60)에서 읽은 데이터가 ACK(0xFA)이면 성공
		bData = InPortByte(0x60);
		if (bData == 0xFA)
		{
			bResult = TRUE;
			break;
		}
		// ACK(0xFA)가 아니면 데이터가 수신된 디바이스에 따라 키보드 큐나 마우스 큐에 삽입
		else
		{
			if (bMouseData == FALSE)
			{
				ConvertScanCodeAndPutQueue(bData);
			}
			else
			{
				AccumulateMouseDataAndPutQueue(bData);
			}
		}
	}
	return bResult;
}

/**
*  마우스 데이터를 모아서 큐에 삽입
*/
bool SkyIOHandler::AccumulateMouseDataAndPutQueue(BYTE bMouseData)
{
	bool bResult;

	// 수신된 바이트 수에 따라 마우스 데이터를 설정
	switch (m_mouseState.iByteCount)
	{
		// 바이트 1에 데이터 설정
	case 0:
		m_mouseState.stCurrentData.bButtonStatusAndFlag = bMouseData;
		m_mouseState.iByteCount++;
		break;

		// 바이트 2에 데이터 설정
	case 1:
		m_mouseState.stCurrentData.bXMovement = bMouseData;
		m_mouseState.iByteCount++;
		break;

		// 바이트 3에 데이터 설정
	case 2:
		m_mouseState.stCurrentData.bYMovement = bMouseData;
		m_mouseState.iByteCount++;
		break;

		// 그 외의 경우는 수신된 바이트 수 초기화
	default:
		m_mouseState.iByteCount = 0;
		break;
	}

	// 3바이트가 모두 수신되었으면 마우스 큐에 삽입하고 수신된 횟수를 초기화
	if (m_mouseState.iByteCount >= 3)
	{
		// 임계 영역 시작
		//kLockForSpinLock( &( m_mouseState.stSpinLock ) );

		kEnterCriticalSection();
		// 마우스 큐에 마우스 데이터 삽입 
		//bResult = kPutQueue(&gs_stMouseQueue, &m_mouseState.stCurrentData);
		if (m_pGUIEngine)
		{
			m_pGUIEngine->PutMouseQueue(&m_mouseState.stCurrentData);
		}
		kLeaveCriticalSection();
		// 임계 영역 끝
		// kUnlockForSpinLock( &( m_mouseState.stSpinLock ) );
		// 수신된 바이트 수 초기화
		m_mouseState.iByteCount = 0;
	}
	return bResult;
}


///////
//키보드
/**
*  출력 버퍼(포트 0x60)에서 키를 읽음
*/
BYTE SkyIOHandler::GetKeyboardScanCode(void)
{
	// 출력 버퍼(포트 0x60)에 데이터가 있을 때까지 대기
	while (IsOutputBufferFull() == FALSE)
	{
		;
	}
	return InPortByte(0x60);
}

/**
*  스캔 코드를 내부적으로 사용하는 키 데이터로 바꾼 후 키 큐에 삽입
*/
bool SkyIOHandler::ConvertScanCodeAndPutQueue(BYTE bScanCode)
{
	KEYDATA stData;
	bool bResult = FALSE;

	stData.bScanCode = bScanCode;
	if (ConvertScanCodeToASCIICode(bScanCode, &(stData.bASCIICode), (bool*)&(stData.bFlags)) == TRUE)
	{
		// 임계 영역 시작
		//kLockForSpinLock( &( m_keyboardState.stSpinLock ) );
		
		kEnterCriticalSection();
		//bResult = kPutQueue(&gs_stKeyQueue, &stData);
		if(m_pGUIEngine)
			bResult = m_pGUIEngine->PutKeyboardQueue(&stData);

		kLeaveCriticalSection();

		// 임계 영역 끝
		//kUnlockForSpinLock( &( m_keyboardState.stSpinLock ) );
	}
	return bResult;
}

/**
*  스캔 코드를 ASCII 코드로 변환
*/
bool SkyIOHandler::ConvertScanCodeToASCIICode(BYTE bScanCode, BYTE* pbASCIICode, bool* pbFlags)
{
	bool bUseCombinedKey;

	// 이전에 Pause 키가 수신되었다면, Pause의 남은 스캔 코드를 무시
	if (m_keyboardState.iSkipCountForPause > 0)
	{
		m_keyboardState.iSkipCountForPause--;
		return FALSE;
	}

	// Pause 키는 특별히 처리
	if (bScanCode == 0xE1)
	{
		*pbASCIICode = KEY_PAUSE;
		*pbFlags = KEY_FLAGS_DOWN;
		m_keyboardState.iSkipCountForPause = KEY_SKIPCOUNTFORPAUSE;
		return TRUE;
	}
	// 확장 키 코드가 들어왔을 때, 실제 키 값은 다음에 들어오므로 플래그 설정만 하고 종료
	else if (bScanCode == 0xE0)
	{
		m_keyboardState.bExtendedCodeIn = TRUE;
		return FALSE;
	}

	// 조합된 키를 반환해야 하는가?
	bUseCombinedKey = IsUseCombinedCode(bScanCode);

	// 키 값 설정
	if (bUseCombinedKey == TRUE)
	{
		*pbASCIICode = gs_vstKeyMappingTable[bScanCode & 0x7F].bCombinedCode;
	}
	else
	{
		*pbASCIICode = gs_vstKeyMappingTable[bScanCode & 0x7F].bNormalCode;
	}

	// 확장 키 유무 설정
	if (m_keyboardState.bExtendedCodeIn == TRUE)
	{
		*pbFlags = KEY_FLAGS_EXTENDEDKEY;
		m_keyboardState.bExtendedCodeIn = FALSE;
	}
	else
	{
		*pbFlags = 0;
	}

	// 눌러짐 또는 떨어짐 유무 설정
	if ((bScanCode & 0x80) == 0)
	{
		*pbFlags |= KEY_FLAGS_DOWN;
	}

	// 조합 키 눌림 또는 떨어짐 상태를 갱신
	UpdateCombinationKeyStatusAndLED(bScanCode);
	return TRUE;
}

/**
*  조합 키의 상태를 갱신하고 LED 상태도 동기화 함
*/
void SkyIOHandler::UpdateCombinationKeyStatusAndLED(BYTE bScanCode)
{
	bool bDown;
	BYTE bDownScanCode;
	bool bLEDStatusChanged = FALSE;

	// 눌림 또는 떨어짐 상태처리, 최상위 비트(비트 7)가 1이면 키가 떨어졌음을 의미하고
	// 0이면 떨어졌음을 의미함
	if (bScanCode & 0x80)
	{
		bDown = FALSE;
		bDownScanCode = bScanCode & 0x7F;
	}
	else
	{
		bDown = TRUE;
		bDownScanCode = bScanCode;
	}

	// 조합 키 검색
	// Shift 키의 스캔 코드(42 or 54)이면 Shift 키의 상태 갱신
	if ((bDownScanCode == 42) || (bDownScanCode == 54))
	{
		m_keyboardState.bShiftDown = bDown;
	}
	// Caps Lock 키의 스캔 코드(58)이면 Caps Lock의 상태 갱신하고 LED 상태 변경
	else if ((bDownScanCode == 58) && (bDown == TRUE))
	{
		m_keyboardState.bCapsLockOn ^= TRUE;
		bLEDStatusChanged = TRUE;
	}
	// Num Lock 키의 스캔 코드(69)이면 Num Lock의 상태를 갱신하고 LED 상태 변경
	else if ((bDownScanCode == 69) && (bDown == TRUE))
	{
		m_keyboardState.bNumLockOn ^= TRUE;
		bLEDStatusChanged = TRUE;
	}
	// Scroll Lock 키의 스캔 코드(70)이면 Scroll Lock의 상태를 갱신하고 LED 상태 변경
	else if ((bDownScanCode == 70) && (bDown == TRUE))
	{
		m_keyboardState.bScrollLockOn ^= TRUE;
		bLEDStatusChanged = TRUE;
	}

	// LED 상태가 변했으면 키보드로 커맨드를 전송하여 LED를 변경
	if (bLEDStatusChanged == TRUE)
	{
		ChangeKeyboardLED(m_keyboardState.bCapsLockOn,
			m_keyboardState.bNumLockOn, m_keyboardState.bScrollLockOn);
	}
}

/**
*  키보드 LED의 ON/OFF를 변경
*/
bool SkyIOHandler::ChangeKeyboardLED(bool bCapsLockOn, bool bNumLockOn, bool bScrollLockOn)
{
	int i, j;
	bool bPreviousInterrupt;
	bool bResult;
	BYTE bData;

	// 인터럽트 불가
	bPreviousInterrupt = kSetInterruptFlag(FALSE);

	// 키보드에 LED 변경 커맨드 전송하고 커맨드가 처리될 때까지 대기
	for (i = 0; i < 0xFFFF; i++)
	{
		// 출력 버퍼(포트 0x60)가 비었으면 커맨드 전송 가능
		if (IsInputBufferFull() == FALSE)
		{
			break;
		}
	}

	// 출력 버퍼(포트 0x60)로 LED 상태 변경 커맨드(0xED) 전송
	OutPortByte(0x60, 0xED);
	for (i = 0; i < 0xFFFF; i++)
	{
		// 입력 버퍼(포트 0x60)가 비어있으면 키보드가 커맨드를 가져간 것임
		if (IsInputBufferFull() == FALSE)
		{
			break;
		}
	}

	// ACK가 올때까지 대기함
	bResult = WaitForACKAndPutOtherScanCode();

	if (bResult == FALSE)
	{
		// 이전 인터럽트 상태 복원
		kSetInterruptFlag(bPreviousInterrupt);
		return FALSE;
	}

	// LED 변경 값을 키보드로 전송하고 데이터가 처리가 완료될 때까지 대기
	OutPortByte(0x60, (bCapsLockOn << 2) | (bNumLockOn << 1) | bScrollLockOn);
	for (i = 0; i < 0xFFFF; i++)
	{
		// 입력 버퍼(포트 0x60)가 비어있으면 키보드가 LED 데이터를 가져간 것임
		if (IsInputBufferFull() == FALSE)
		{
			break;
		}
	}

	// ACK가 올 때까지 대기함
	bResult = WaitForACKAndPutOtherScanCode();

	// 이전 인터럽트 상태 복원
	kSetInterruptFlag(bPreviousInterrupt);
	return bResult;
}

/**
*  조합된 키 값을 사용해야 하는지 여부를 반환
*/
bool SkyIOHandler::IsUseCombinedCode(bool bScanCode)
{
	BYTE bDownScanCode;
	bool bUseCombinedKey;

	bDownScanCode = bScanCode & 0x7F;

	// 알파벳 키라면 Shift 키와 Caps Lock의 영향을 받음
	if (IsAlphabetScanCode(bDownScanCode) == TRUE)
	{
		// 만약 Shift 키와 Caps Lock 키 중에 하나만 눌러져있으면 조합된 키를 되돌려 줌
		if (m_keyboardState.bShiftDown ^ m_keyboardState.bCapsLockOn)
		{
			bUseCombinedKey = TRUE;
		}
		else
		{
			bUseCombinedKey = FALSE;
		}
	}
	// 숫자와 기호 키라면 Shift 키의 영향을 받음
	else if (IsNumberOrSymbolScanCode(bDownScanCode) == TRUE)
	{
		// Shift 키가 눌러져있으면 조합된 키를 되돌려 줌
		if (m_keyboardState.bShiftDown == TRUE)
		{
			bUseCombinedKey = TRUE;
		}
		else
		{
			bUseCombinedKey = FALSE;
		}
	}
	// 숫자 패드 키라면 Num Lock 키의 영향을 받음
	// 0xE0만 제외하면 확장 키 코드와 숫자 패드의 코드가 겹치므로,
	// 확장 키 코드가 수신되지 않았을 때만처리 조합된 코드 사용
	else if ((IsNumberPadScanCode(bDownScanCode) == TRUE) &&
		(m_keyboardState.bExtendedCodeIn == FALSE))
	{
		// Num Lock 키가 눌러져있으면, 조합된 키를 되돌려 줌
		if (m_keyboardState.bNumLockOn == TRUE)
		{
			bUseCombinedKey = TRUE;
		}
		else
		{
			bUseCombinedKey = FALSE;
		}
	}

	return bUseCombinedKey;
}

/**
*  스캔 코드가 알파벳 범위인지 여부를 반환
*/
bool SkyIOHandler::IsAlphabetScanCode(BYTE bScanCode)
{
	// 변환 테이블을 값을 직접 읽어서 알파벳 범위인지 확인
	if (('a' <= gs_vstKeyMappingTable[bScanCode].bNormalCode) &&
		(gs_vstKeyMappingTable[bScanCode].bNormalCode <= 'z'))
	{
		return TRUE;
	}
	return FALSE;
}

/**
*  숫자 또는 기호 범위인지 여부를 반환
*/
bool SkyIOHandler::IsNumberOrSymbolScanCode(BYTE bScanCode)
{
	// 숫자 패드나 확장 키 범위를 제외한 범위(스캔 코드 2~53)에서 영문자가 아니면
	// 숫자 또는 기호임
	if ((2 <= bScanCode) && (bScanCode <= 53) &&
		(IsAlphabetScanCode(bScanCode) == FALSE))
	{
		return TRUE;
	}

	return FALSE;
}

/**
* 숫자 패드 범위인지 여부를 반환
*/
bool SkyIOHandler::IsNumberPadScanCode(BYTE bScanCode)
{
	// 숫자 패드는 스캔 코드의 71~83에 있음
	if ((71 <= bScanCode) && (bScanCode <= 83))
	{
		return TRUE;
	}

	return FALSE;
}

/**
*  키보드를 활성화 함
*/
bool SkyIOHandler::ActivateKeyboard(void)
{
	int i, j;
	bool bPreviousInterrupt;
	bool bResult;

	// 인터럽트 불가
	bPreviousInterrupt = kSetInterruptFlag(FALSE);

	// 컨트롤 레지스터(포트 0x64)에 키보드 활성화 커맨드(0xAE)를 전달하여 키보드 디바이스 활성화
	OutPortByte(0x64, 0xAE);

	// 입력 버퍼(포트 0x60)가 빌 때까지 기다렸다가 키보드에 활성화 커맨드를 전송
	// 0xFFFF만큼 루프를 수행할 시간이면 충분히 커맨드가 전송될 수 있음
	// 0xFFFF 루프를 수행한 이후에도 입력 버퍼(포트 0x60)가 비지 않으면 무시하고 전송
	for (i = 0; i < 0xFFFF; i++)
	{
		// 입력 버퍼(포트 0x60)가 비어있으면 키보드 커맨드 전송 가능
		if (IsInputBufferFull() == FALSE)
		{
			break;
		}
	}

	// 입력 버퍼(포트 0x60)로 키보드 활성화(0xF4) 커맨드를 전달하여 키보드로 전송
	OutPortByte(0x60, 0xF4);

	// ACK가 올 때까지 대기함
	bResult = WaitForACKAndPutOtherScanCode();

	// 이전 인터럽트 상태 복원
	kSetInterruptFlag(bPreviousInterrupt);
	return bResult;
}