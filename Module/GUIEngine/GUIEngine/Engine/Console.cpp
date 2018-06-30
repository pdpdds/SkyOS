/**
 *  file    Console.c
 *  date    2009/01/31
 *  author  kkamagui 
 *          Copyright(c)2008 All rights reserved by kkamagui
 *  brief   콘솔에 관련된 소스 파일
 */

#include <stdarg.h>
#include "Console.h"
#include "Keyboard.h"
//#include "Utility.h"
#include "Console.h"
#include "memory.h"
#include "sprintf.h"
#include "string.h"
#include "Mouse.h"
//#include "AssemblyUtility.h"

// 콘솔의 정보를 관리하는 자료구조
CONSOLEMANAGER gs_stConsoleManager = { 0, };

// 그래픽 모드로 시작했을 때 사용하는 화면 버퍼 영역
static CHARACTER gs_vstScreenBuffer[ CONSOLE_WIDTH * CONSOLE_HEIGHT ];

// 그래픽 모드로 시작했을 때 GUI 콘솔 셸 윈도우로 전달된 키 이벤트를 콘솔 셸 태스크로 
// 전달하는 큐 버퍼
static KEYDATA gs_vstKeyQueueBuffer[ CONSOLE_GUIKEYQUEUE_MAXCOUNT ];

/**
 *  콘솔 초기화
 */
void kInitializeConsole( int iX, int iY )
{
    // 콘솔 자료구조 초기화
    memset( &gs_stConsoleManager, 0, sizeof( gs_stConsoleManager ) );
    // 화면 버퍼 초기화
	memset( &gs_vstScreenBuffer, 0, sizeof( gs_vstScreenBuffer ) );
    
	//if( kIsGraphicMode() == FALSE )
    if( 0 )
    {
        // 그래픽 모드로 시작한 것이 아니면 비디오 메모리를 화면 버퍼로 설정
        gs_stConsoleManager.pstScreenBuffer = (CHARACTER* ) CONSOLE_VIDEOMEMORYADDRESS;
    }
    else
    {
        // 그래픽 모드로 시작했으면 그래픽 모드용 화면 버퍼를 설정
        gs_stConsoleManager.pstScreenBuffer = (CHARACTER*)gs_vstScreenBuffer;
        
        // 그래픽 모드에서 사용할 키 큐와 뮤텍스를 초기화
        kInitializeQueue( &( gs_stConsoleManager.stKeyQueueForGUI ), gs_vstKeyQueueBuffer, 
                CONSOLE_GUIKEYQUEUE_MAXCOUNT, sizeof( KEYDATA ) );
       // kInitializeMutex( &( gs_stConsoleManager.stLock ) );
    }
    
    // 커서 위치 설정
    kSetCursor( iX, iY );
}

/**
 *  커서의 위치를 설정
 *      문자를 출력할 위치도 같이 설정
 */
void kSetCursor( int iX, int iY ) 
{
    int iLinearValue;
    int iOldX;
    int iOldY;
    int i;
    
    // 커서의 위치를 계산
    iLinearValue = iY * CONSOLE_WIDTH + iX;

    // 텍스트 모드로 시작했으면 CRT 컨트롤러로 커서 위치를 전송
    //if( kIsGraphicMode() == FALSE )
	if(0)
    {/*
        // CRTC 컨트롤 어드레스 레지스터(포트 0x3D4)에 0x0E를 전송하여
        // 상위 커서 위치 레지스터를 선택
        kOutPortByte( VGA_PORT_INDEX, VGA_INDEX_UPPERCURSOR );
        // CRTC 컨트롤 데이터 레지스터(포트 0x3D5)에 커서의 상위 바이트를 출력
        kOutPortByte( VGA_PORT_DATA, iLinearValue >> 8 );
    
        // CRTC 컨트롤 어드레스 레지스터(포트 0x3D4)에 0x0F를 전송하여
        // 하위 커서 위치 레지스터를 선택
        kOutPortByte( VGA_PORT_INDEX, VGA_INDEX_LOWERCURSOR );
        // CRTC 컨트롤 데이터 레지스터(포트 0x3D5)에 커서의 하위 바이트를 출력
        kOutPortByte( VGA_PORT_DATA, iLinearValue & 0xFF );*/
    }
    // 그래픽 모드로 시작했으면 화면 버퍼에 출력한 커서의 위치를 옮겨줌
    else
    {
        // 이전에 커서가 있던 위치가 그대로 커서로 남아있으면 커서를 지움
        for( i = 0 ; i < CONSOLE_WIDTH * CONSOLE_HEIGHT ; i++ )
        {
            // 커서가 있으면 삭제
            if( ( gs_stConsoleManager.pstScreenBuffer[ i ].bCharactor == '_' ) &&
                ( gs_stConsoleManager.pstScreenBuffer[ i ].bAttribute == 0x00 ) )
            {
                gs_stConsoleManager.pstScreenBuffer[ i ].bCharactor = ' ';
                gs_stConsoleManager.pstScreenBuffer[ i ].bAttribute = 
                    CONSOLE_DEFAULTTEXTCOLOR;
                break;
            }
        }
        
        // 새로운 위치에 커서를 출력
        gs_stConsoleManager.pstScreenBuffer[ iLinearValue ].bCharactor = '_';
        gs_stConsoleManager.pstScreenBuffer[ iLinearValue ].bAttribute = 0x00;
    }
    // 문자를 출력할 위치 업데이트
    gs_stConsoleManager.iCurrentPrintOffset = iLinearValue;
}

/**
 *  현재 커서의 위치를 반환
 */
void kGetCursor( int *piX, int *piY )
{
    *piX = gs_stConsoleManager.iCurrentPrintOffset % CONSOLE_WIDTH;
    *piY = gs_stConsoleManager.iCurrentPrintOffset / CONSOLE_WIDTH;
}



/**
*  문자열의 순서를 뒤집음
*/
void kReverseString(char* pcBuffer)
{
	int iLength;
	int i;
	char cTemp;


	// 문자열의 가운데를 중심으로 좌/우를 바꿔서 순서를 뒤집음
	iLength = strlen(pcBuffer);
	for (i = 0; i < iLength / 2; i++)
	{
		cTemp = pcBuffer[i];
		pcBuffer[i] = pcBuffer[iLength - 1 - i];
		pcBuffer[iLength - 1 - i] = cTemp;
	}
}

/**
*  16진수 값을 문자열로 변환
*/
int kHexToString(QWORD qwValue, char* pcBuffer)
{
	QWORD i;
	QWORD qwCurrentValue;

	// 0이 들어오면 바로 처리
	if (qwValue == 0)
	{
		pcBuffer[0] = '0';
		pcBuffer[1] = '\0';
		return 1;
	}

	// 버퍼에 1의 자리부터 16, 256, ...의 자리 순서로 숫자 삽입
	for (i = 0; qwValue > 0; i++)
	{
		qwCurrentValue = qwValue % 16;
		if (qwCurrentValue >= 10)
		{
			pcBuffer[i] = 'A' + (qwCurrentValue - 10);
		}
		else
		{
			pcBuffer[i] = '0' + qwCurrentValue;
		}

		qwValue = qwValue / 16;
	}
	pcBuffer[i] = '\0';

	// 버퍼에 들어있는 문자열을 뒤집어서 ... 256, 16, 1의 자리 순서로 변경
	kReverseString(pcBuffer);
	return i;
}

/**
*  10진수 값을 문자열로 변환
*/
int kDecimalToString(long lValue, char* pcBuffer)
{
	long i;

	// 0이 들어오면 바로 처리
	if (lValue == 0)
	{
		pcBuffer[0] = '0';
		pcBuffer[1] = '\0';
		return 1;
	}

	// 만약 음수이면 출력 버퍼에 '-'를 추가하고 양수로 변환
	if (lValue < 0)
	{
		i = 1;
		pcBuffer[0] = '-';
		lValue = -lValue;
	}
	else
	{
		i = 0;
	}

	// 버퍼에 1의 자리부터 10, 100, 1000 ...의 자리 순서로 숫자 삽입
	for (; lValue > 0; i++)
	{
		pcBuffer[i] = '0' + lValue % 10;
		lValue = lValue / 10;
	}
	pcBuffer[i] = '\0';

	// 버퍼에 들어있는 문자열을 뒤집어서 ... 1000, 100, 10, 1의 자리 순서로 변경
	if (pcBuffer[0] == '-')
	{
		// 음수인 경우는 부호를 제외하고 문자열을 뒤집음
		kReverseString(&(pcBuffer[1]));
	}
	else
	{
		kReverseString(pcBuffer);
	}

	return i;
}

int kIToA(long lValue, char* pcBuffer, int iRadix)
{
	int iReturn;

	switch (iRadix)
	{
		// 16진수
	case 16:
		iReturn = kHexToString(lValue, pcBuffer);
		break;

		// 10진수 또는 기타
	case 10:
	default:
		iReturn = kDecimalToString(lValue, pcBuffer);
		break;
	}

	return iReturn;
}

/**
*  vsprintf() 함수의 내부 구현
*      버퍼에 포맷 문자열에 따라 데이터를 복사
*/
int kVSPrintf(char* pcBuffer, const char* pcFormatString, va_list ap)
{
	QWORD i, j, k;
	int iBufferIndex = 0;
	int iFormatLength, iCopyLength;
	char* pcCopyString;
	QWORD qwValue;
	int iValue;
	double dValue;

	// 포맷 문자열의 길이를 읽어서 문자열의 길이만큼 데이터를 출력 버퍼에 출력
	iFormatLength = strlen(pcFormatString);
	for (i = 0; i < iFormatLength; i++)
	{
		// %로 시작하면 데이터 타입 문자로 처리
		if (pcFormatString[i] == '%')
		{
			// % 다음의 문자로 이동
			i++;
			switch (pcFormatString[i])
			{
				// 문자열 출력  
			case 's':
				// 가변 인자에 들어있는 파라미터를 문자열 타입으로 변환
				pcCopyString = (char*)(va_arg(ap, char*));
				iCopyLength = strlen(pcCopyString);
				// 문자열의 길이만큼을 출력 버퍼로 복사하고 출력한 길이만큼 
				// 버퍼의 인덱스를 이동
				memcpy(pcBuffer + iBufferIndex, pcCopyString, iCopyLength);
				iBufferIndex += iCopyLength;
				break;

				// 문자 출력
			case 'c':
				// 가변 인자에 들어있는 파라미터를 문자 타입으로 변환하여 
				// 출력 버퍼에 복사하고 버퍼의 인덱스를 1만큼 이동
				pcBuffer[iBufferIndex] = (char)(va_arg(ap, int));
				iBufferIndex++;
				break;

				// 정수 출력
			case 'd':
			case 'i':
				// 가변 인자에 들어있는 파라미터를 정수 타입으로 변환하여
				// 출력 버퍼에 복사하고 출력한 길이만큼 버퍼의 인덱스를 이동
				iValue = (int)(va_arg(ap, int));
				iBufferIndex += kIToA(iValue, pcBuffer + iBufferIndex, 10);
				break;

				// 4바이트 Hex 출력
			case 'x':
			case 'X':
				// 가변 인자에 들어있는 파라미터를 DWORD 타입으로 변환하여
				// 출력 버퍼에 복사하고 출력한 길이만큼 버퍼의 인덱스를 이동
				qwValue = (DWORD)(va_arg(ap, DWORD)) & 0xFFFFFFFF;
				iBufferIndex += kIToA(qwValue, pcBuffer + iBufferIndex, 16);
				break;

				// 8바이트 Hex 출력
			case 'q':
			case 'Q':
			case 'p':
				// 가변 인자에 들어있는 파라미터를 QWORD 타입으로 변환하여
				// 출력 버퍼에 복사하고 출력한 길이만큼 버퍼의 인덱스를 이동
				qwValue = (QWORD)(va_arg(ap, QWORD));
				iBufferIndex += kIToA(qwValue, pcBuffer + iBufferIndex, 16);
				break;

				// 소수점 둘째 자리까지 실수를 출력
			case 'f':
				dValue = (double)(va_arg(ap, double));
				// 셋째 자리에서 반올림 처리
				dValue += 0.005;
				// 소수점 둘째 자리부터 차례로 저장하여 버퍼를 뒤집음
				pcBuffer[iBufferIndex] = '0' + (QWORD)(dValue * 100) % 10;
				pcBuffer[iBufferIndex + 1] = '0' + (QWORD)(dValue * 10) % 10;
				pcBuffer[iBufferIndex + 2] = '.';
				for (k = 0; ; k++)
				{
					// 정수 부분이 0이면 종료
					if (((QWORD)dValue == 0) && (k != 0))
					{
						break;
					}
					pcBuffer[iBufferIndex + 3 + k] = '0' + ((QWORD)dValue % 10);
					dValue = dValue / 10;
				}
				pcBuffer[iBufferIndex + 3 + k] = '\0';
				// 값이 저장된 길이만큼 뒤집고 길이를 증가시킴
				kReverseString(pcBuffer + iBufferIndex);
				iBufferIndex += 3 + k;
				break;

				// 위에 해당하지 않으면 문자를 그대로 출력하고 버퍼의 인덱스를
				// 1만큼 이동
			default:
				pcBuffer[iBufferIndex] = pcFormatString[i];
				iBufferIndex++;
				break;
			}
		}
		// 일반 문자열 처리
		else
		{
			// 문자를 그대로 출력하고 버퍼의 인덱스를 1만큼 이동
			pcBuffer[iBufferIndex] = pcFormatString[i];
			iBufferIndex++;
		}
	}

	// NULL을 추가하여 완전한 문자열로 만들고 출력한 문자의 길이를 반환
	pcBuffer[iBufferIndex] = '\0';
	return iBufferIndex;
}

void kPrintf( const char* pcFormatString, ... )
{
    va_list ap;
    char vcBuffer[ 1024 ];
    int iNextPrintOffset;

    // 가변 인자 리스트를 사용해서 vsprintf()로 처리
    va_start( ap, pcFormatString );
	kVSPrintf( vcBuffer, pcFormatString, ap );
    va_end( ap );
    
    // 포맷 문자열을 화면에 출력
    iNextPrintOffset = kConsolePrintString( vcBuffer );
    
    // 커서의 위치를 업데이트
    kSetCursor( iNextPrintOffset % CONSOLE_WIDTH, iNextPrintOffset / CONSOLE_WIDTH );
}

/**
 *  \n, \t와 같은 문자가 포함된 문자열을 출력한 후, 화면상의 다음 출력할 위치를 
 *  반환
 */
int kConsolePrintString( const char* pcBuffer )
{
    CHARACTER* pstScreen;
    int i, j;
    int iLength;
    int iPrintOffset;

    // 화면 버퍼를 설정
    pstScreen = gs_stConsoleManager.pstScreenBuffer;
    
    // 문자열을 출력할 위치를 저장
    iPrintOffset = gs_stConsoleManager.iCurrentPrintOffset;

    // 문자열의 길이만큼 화면에 출력
    iLength = strlen( pcBuffer );    
    for( i = 0 ; i < iLength ; i++ )
    {
        // 개행 처리
        if( pcBuffer[ i ] == '\n' )
        {
            // 출력할 위치를 80의 배수 컬럼으로 옮김
            // 현재 라인의 남은 문자열의 수만큼 더해서 다음 라인으로 위치시킴
            iPrintOffset += ( CONSOLE_WIDTH - ( iPrintOffset % CONSOLE_WIDTH ) );
        }
        // 탭 처리
        else if( pcBuffer[ i ] == '\t' )
        {
            // 출력할 위치를 8의 배수 컬럼으로 옮김
            iPrintOffset += ( 8 - ( iPrintOffset % 8 ) );
        }
        // 일반 문자열 출력
        else
        {
            // 비디오 메모리에 문자와 속성을 설정하여 문자를 출력하고
            // 출력할 위치를 다음으로 이동
            pstScreen[ iPrintOffset ].bCharactor = pcBuffer[ i ];
            pstScreen[ iPrintOffset ].bAttribute = CONSOLE_DEFAULTTEXTCOLOR;
            iPrintOffset++;
        }
        
        // 출력할 위치가 화면의 최댓값(80 * 25)을 벗어났으면 스크롤 처리
        if( iPrintOffset >= ( CONSOLE_HEIGHT * CONSOLE_WIDTH ) )
        {
            // 가장 윗줄을 제외한 나머지를 한 줄 위로 복사
            memcpy( pstScreen, pstScreen + CONSOLE_WIDTH,
                     ( CONSOLE_HEIGHT - 1 ) * CONSOLE_WIDTH * sizeof( CHARACTER ) );

            // 가장 마지막 라인은 공백으로 채움
            for( j = ( CONSOLE_HEIGHT - 1 ) * ( CONSOLE_WIDTH ) ; 
                 j < ( CONSOLE_HEIGHT * CONSOLE_WIDTH ) ; j++ )
            {
                // 공백 출력
                pstScreen[ j ].bCharactor = ' ';
                pstScreen[ j ].bAttribute = CONSOLE_DEFAULTTEXTCOLOR;
            }
            
            // 출력할 위치를 가장 아래쪽 라인의 처음으로 설정
            iPrintOffset = ( CONSOLE_HEIGHT - 1 ) * CONSOLE_WIDTH;
        }
    }
    return iPrintOffset;
}

/**
 *  전체 화면을 삭제
 */
void kClearScreen( void )
{
    CHARACTER* pstScreen;
    int i;
    
    // 화면 버퍼를 설정
    pstScreen = gs_stConsoleManager.pstScreenBuffer;
    
    // 화면 전체를 공백으로 채우고, 커서의 위치를 0, 0으로 옮김
    for( i = 0 ; i < CONSOLE_WIDTH * CONSOLE_HEIGHT ; i++ )
    {
        pstScreen[ i ].bCharactor = ' ';
        pstScreen[ i ].bAttribute = CONSOLE_DEFAULTTEXTCOLOR;
    }
    
    // 커서를 화면 상단으로 이동
    kSetCursor( 0, 0 );
}

/**
 *  getch() 함수의 구현
 */
BYTE kGetCh( void )
{
    KEYDATA stData;
    
    // 키가 눌러질 때까지 대기함
    while( 1 )
    {
        // 그래픽 모드가 아닌 경우는 커널의 키 큐에서 값을 가져옴
       // if( kIsGraphicMode() == FALSE )
		if (0)
        {
            // 키 큐에 데이터가 수신될 때까지 대기
            while( kGetKeyFromKeyQueue( &stData ) == FALSE )
            {
               // kSchedule();
            }
        }
        // 그래픽 모드인 경우는 그래픽 모드용 키 큐에서 값을 가져옴
        else
        {
            while( kGetKeyFromGUIKeyQueue( &stData ) == FALSE )
            {
                // 그래픽 모드에서 동작하는 중에 셸 태스크를 종료해야될 경우 루프를 종료
                if( gs_stConsoleManager.bExit == TRUE )
                {
                    return 0xFF;
                }
                //kSchedule();
            }
        }
        
        // 키가 눌렸다는 데이터가 수신되면 ASCII 코드를 반환
        if( stData.bFlags & KEY_FLAGS_DOWN )
        {
            return stData.bASCIICode;
        }
    }
}

/**
 *  문자열을 X, Y 위치에 출력
 */
void kPrintStringXY( int iX, int iY, const char* pcString )
{
    CHARACTER* pstScreen;
    int i;
    
    // 화면 버퍼를 설정
    pstScreen = gs_stConsoleManager.pstScreenBuffer;
  
    // 현재 출력할 위치를 계산
    pstScreen += ( iY * CONSOLE_WIDTH ) + iX;
    // 문자열의 길이만큼 루프를 돌면서 문자와 속성을 저장
    for( i = 0 ; pcString[ i ] != 0 ; i++ )
    {
        pstScreen[ i ].bCharactor = pcString[ i ];
        pstScreen[ i ].bAttribute = CONSOLE_DEFAULTTEXTCOLOR;
    }
}

/**
 *  콘솔을 관리하는 자료구조를 반환
 */
CONSOLEMANAGER* kGetConsoleManager( void )
{
    return &gs_stConsoleManager;
}

/**
 *  그래픽 모드용 키 큐에서 키 데이터를 제거
 */
bool kGetKeyFromGUIKeyQueue( KEYDATA* pstData )
{
    bool bResult;
    
    // 큐에 데이터가 없으면 실패
    if( kIsQueueEmpty( &( gs_stConsoleManager.stKeyQueueForGUI ) ) == TRUE )
    {
        return FALSE;
    }
    
    // 동기화 처리
   // kLock( &( gs_stConsoleManager.stLock ) );

    // 큐에서 데이터를 제거
	kEnterCriticalSection();
    bResult = kGetQueue( &( gs_stConsoleManager.stKeyQueueForGUI ), pstData );
	kLeaveCriticalSection();
    // 동기화 처리
   // kUnlock( &( gs_stConsoleManager.stLock ) );
    
    return bResult;
}

/**
 *  그래픽 모드용 키 큐에 데이터를 삽입
 */
bool kPutKeyToGUIKeyQueue( KEYDATA* pstData )
{
    bool bResult;
    
    // 큐에 데이터가 가득 찼으면 실패
    if( kIsQueueFull( &( gs_stConsoleManager.stKeyQueueForGUI ) ) == TRUE )
    {
        return FALSE;
    }
    
    // 동기화 처리
    //kLock( &( gs_stConsoleManager.stLock ) );

    // 큐에 데이터를 삽입
	kEnterCriticalSection();
    bResult = kPutQueue( &( gs_stConsoleManager.stKeyQueueForGUI ), pstData );
	kLeaveCriticalSection();
    // 동기화 처리
    //kUnlock( &( gs_stConsoleManager.stLock ) );
    
    return bResult;
}

/**
 *  콘솔 셸 태스크 종료 플래그를 설정
 */
void kSetConsoleShellExitFlag( bool bFlag )
{
    gs_stConsoleManager.bExit = bFlag;
}
