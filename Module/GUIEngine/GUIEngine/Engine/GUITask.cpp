/**
 *  file    GUITask.c
 *  date    2009/10/20
 *  author  kkamagui 
 *          Copyright(c)2008 All rights reserved by kkamagui
 *  brief   GUI 태스크에 관련된 함수를 정의한 소스 파일
 */
#include "GUITask.h"
#include "Window.h"
#include "memory.h"
#include "string.h"
#include "sprintf.h"
#include "fileio.h"
#include "JPEG.h"
#include "MintFont.h"
//#include "Console.h"
#include "ConsoleShell.h"
#include "SkyInterface.h"
#include "Mouse.h"
#define FILESYSTEM_MAXFILENAMELENGTH        24
//#include "MultiProcessor.h"
//#include "MPConfigurationTable.h"
//------------------------------------------------------------------------------
//  기본 GUI 태스크
//------------------------------------------------------------------------------
/**
 *  기본 GUI 태스크의 코드
 *      GUI 태스크를 만들 때 복사하여 기본 코드로 사용
 */
extern void printf(const char* str, ...);

DWORD WINAPI kBaseGUITask(LPVOID parameter)
{
    QWORD qwWindowID;
    int iMouseX, iMouseY;
    int iWindowWidth, iWindowHeight;
    EVENT stReceivedEvent;
    MOUSEEVENT* pstMouseEvent;
    KEYEVENT* pstKeyEvent;
    WINDOWEVENT* pstWindowEvent;

    
    //--------------------------------------------------------------------------
    // 윈도우를 생성
    //--------------------------------------------------------------------------
    // 마우스의 현재 위치를 반환
    kGetCursorPosition( &iMouseX, &iMouseY );

    // 윈도우의 크기와 제목 설정
    iWindowWidth = 500;
    iWindowHeight = 200;
    
    // 윈도우 생성 함수 호출, 마우스가 있던 위치를 기준으로 생성
    qwWindowID = kCreateWindow( iMouseX - 10, iMouseY - WINDOW_TITLEBAR_HEIGHT / 2,
        iWindowWidth, iWindowHeight, WINDOW_FLAGS_DEFAULT | WINDOW_FLAGS_RESIZABLE,
         "Hello World Window" );
    // 윈도우를 생성하지 못했으면 실패
    if( qwWindowID == WINDOW_INVALIDID )
    {
        return 0;
    }
    
    //--------------------------------------------------------------------------
    // GUI 태스크의 이벤트 처리 루프
    //--------------------------------------------------------------------------
    while( 1 )
    {
        // 이벤트 큐에서 이벤트를 수신
        if( kReceiveEventFromWindowQueue( qwWindowID, &stReceivedEvent ) == FALSE )
        {

			//20180628
			ksleep( 0 );
            continue;
        }
        
        // 수신된 이벤트를 타입에 따라 나누어 처리
        switch( stReceivedEvent.qwType )
        {
            // 마우스 이벤트 처리
        case EVENT_MOUSE_MOVE:
        case EVENT_MOUSE_LBUTTONDOWN:
        case EVENT_MOUSE_LBUTTONUP:            
        case EVENT_MOUSE_RBUTTONDOWN:
        case EVENT_MOUSE_RBUTTONUP:
        case EVENT_MOUSE_MBUTTONDOWN:
        case EVENT_MOUSE_MBUTTONUP:
            // 여기에 마우스 이벤트 처리 코드 넣기
            pstMouseEvent = &( stReceivedEvent.stMouseEvent );
            break;

            // 키 이벤트 처리
        case EVENT_KEY_DOWN:
        case EVENT_KEY_UP:
            // 여기에 키보드 이벤트 처리 코드 넣기
            pstKeyEvent = &( stReceivedEvent.stKeyEvent );
            break;

            // 윈도우 이벤트 처리
        case EVENT_WINDOW_SELECT:
        case EVENT_WINDOW_DESELECT:
        case EVENT_WINDOW_MOVE:
        case EVENT_WINDOW_RESIZE:
        case EVENT_WINDOW_CLOSE:
            // 여기에 윈도우 이벤트 처리 코드 넣기
            pstWindowEvent = &( stReceivedEvent.stWindowEvent );

            //------------------------------------------------------------------
            // 윈도우 닫기 이벤트이면 윈도우를 삭제하고 루프를 빠져나가 태스크를 종료
            //------------------------------------------------------------------
            if( stReceivedEvent.qwType == EVENT_WINDOW_CLOSE )
            {
                // 윈도우 삭제
                kDeleteWindow( qwWindowID );
                return 1;
            }
            break;
            
            // 그 외 정보
        default:
            // 여기에 알 수 없는 이벤트 처리 코드 넣기
            break;
        }
    }

	return 0;
}

//------------------------------------------------------------------------------
//  Hello World GUI 태스크
//------------------------------------------------------------------------------
/**
 *  Hello World GUI 태스크
 */
QWORD qwGUIWindowID;
int iWindowWidth, iWindowHeight;
int iY;
MOUSEEVENT* pstMouseEvent;
KEYEVENT* pstKeyEvent;
WINDOWEVENT* pstWindowEvent;
char vcTempBuffer[50];
RECT stButtonArea;
    QWORD qwFindWindowID;
    EVENT stSendEvent;
	static int s_iWindowCount = 0;

	// 이벤트 타입 문자열
	char* vpcEventString[] = {
		"Unknown",
		"MOUSE_MOVE       ",
		"MOUSE_LBUTTONDOWN",
		"MOUSE_LBUTTONUP  ",
		"MOUSE_RBUTTONDOWN",
		"MOUSE_RBUTTONUP  ",
		"MOUSE_MBUTTONDOWN",
		"MOUSE_MBUTTONUP  ",
		"WINDOW_SELECT    ",
		"WINDOW_DESELECT  ",
		"WINDOW_MOVE      ",
		"WINDOW_RESIZE    ",
		"WINDOW_CLOSE     ",
		"KEY_DOWN         ",
		"KEY_UP           " };


DWORD WINAPI kHelloWorldGUITask(LPVOID parameter)
{
    
    int iMouseX, iMouseY;
   
  
   
    
   
    
    
    int i;
    
    //--------------------------------------------------------------------------
    // 윈도우를 생성
    //--------------------------------------------------------------------------
    // 마우스의 현재 위치를 반환
    kGetCursorPosition( &iMouseX, &iMouseY );

    // 윈도우의 크기와 제목 설정
    iWindowWidth = 500;
    iWindowHeight = 200;
    
    // 윈도우 생성 함수 호출, 마우스가 있던 위치를 기준으로 생성하고 번호를 추가하여
    // 윈도우마다 개별적인 이름을 할당
    sprintf( vcTempBuffer, "Hello World Window %d", s_iWindowCount++ );
	qwGUIWindowID = kCreateWindow( iMouseX - 10, iMouseY - WINDOW_TITLEBAR_HEIGHT / 2,
        iWindowWidth, iWindowHeight, WINDOW_FLAGS_DEFAULT, vcTempBuffer );
    // 윈도우를 생성하지 못했으면 실패
    if(qwGUIWindowID == WINDOW_INVALIDID )
    {
        return 0;
    }
    
    //--------------------------------------------------------------------------
    // 윈도우 매니저가 윈도우로 전송하는 이벤트를 표시하는 영역을 그림
    //--------------------------------------------------------------------------
    // 이벤트 정보를 출력할 위치 저장
    iY = WINDOW_TITLEBAR_HEIGHT + 10;
    
    // 이벤트 정보를 표시하는 영역의 테두리와 윈도우 ID를 표시
    kDrawRect(qwGUIWindowID, 10, iY + 8, iWindowWidth - 10, iY + 70, RGB( 0, 0, 0 ),
            FALSE );
    sprintf( vcTempBuffer, "GUI Event Information[Window ID: 0x%Q]", qwGUIWindowID);
    kDrawText(qwGUIWindowID, 20, iY, RGB( 0, 0, 0 ), RGB( 255, 255, 255 ),
               vcTempBuffer, strlen( vcTempBuffer ) );    
    
    //--------------------------------------------------------------------------
    // 화면 아래에 이벤트 전송 버튼을 그림
    //--------------------------------------------------------------------------
    // 버튼 영역을 설정
    kSetRectangleData( 10, iY + 80, iWindowWidth - 10, iWindowHeight - 10, 
            &stButtonArea );
    // 배경은 윈도우의 배경색으로 설정하고 문자는 검은색으로 설정하여 버튼을 그림
    kDrawButton(qwGUIWindowID, &stButtonArea, WINDOW_COLOR_BACKGROUND,
            "User Message Send Button(Up)", RGB( 0, 0, 0 ) );
    // 윈도우를 화면에 표시
    kShowWindow(qwGUIWindowID, TRUE );

	return 1;
       
}

void UpdateHelloGUITask()
{
	EVENT stReceivedEvent;
	//--------------------------------------------------------------------------
	// GUI 태스크의 이벤트 처리 루프
	//--------------------------------------------------------------------------
	while (1)
	{
		// 이벤트 큐에서 이벤트를 수신
		if (kReceiveEventFromWindowQueue(qwGUIWindowID, &stReceivedEvent) == FALSE)
		{
			//20180628
			ksleep( 0 );
			continue;
		}

		// 윈도우 이벤트 정보가 표시될 영역을 배경색으로 칠하여 이전에 표시한 데이터를
		// 모두 지움
		kDrawRect(qwGUIWindowID, 11, iY + 20, iWindowWidth - 11, iY + 69,
			WINDOW_COLOR_BACKGROUND, TRUE);

		// 수신된 이벤트를 타입에 따라 나누어 처리
		switch (stReceivedEvent.qwType)
		{
			// 마우스 이벤트 처리
		case EVENT_MOUSE_MOVE:
		case EVENT_MOUSE_LBUTTONDOWN:
		case EVENT_MOUSE_LBUTTONUP:
		case EVENT_MOUSE_RBUTTONDOWN:
		case EVENT_MOUSE_RBUTTONUP:
		case EVENT_MOUSE_MBUTTONDOWN:
		case EVENT_MOUSE_MBUTTONUP:
			// 여기에 마우스 이벤트 처리 코드 넣기
			pstMouseEvent = &(stReceivedEvent.stMouseEvent);

			// 마우스 이벤트의 타입을 출력
			sprintf(vcTempBuffer, "Mouse Event: %s",
				vpcEventString[stReceivedEvent.qwType]);
			kDrawText(qwGUIWindowID, 20, iY + 20, RGB(0, 0, 0),
				WINDOW_COLOR_BACKGROUND, vcTempBuffer, strlen(vcTempBuffer));

			// 마우스 데이터를 출력
			sprintf(vcTempBuffer, "Data: X = %d, Y = %d, Button = %X",
				pstMouseEvent->stPoint.iX, pstMouseEvent->stPoint.iY,
				pstMouseEvent->bButtonStatus);
			kDrawText(qwGUIWindowID, 20, iY + 40, RGB(0, 0, 0),
				WINDOW_COLOR_BACKGROUND, vcTempBuffer, strlen(vcTempBuffer));

			//------------------------------------------------------------------
			// 마우스 눌림 또는 떨어짐 이벤트이면 버튼의 색깔을 다시 그림
			//------------------------------------------------------------------
			// 마우스 왼쪽 버튼이 눌렸을 때 버튼 처리
			if (stReceivedEvent.qwType == EVENT_MOUSE_LBUTTONDOWN)
			{
				// 버튼 영역에 마우스 왼쪽 버튼이 눌렸는지를 판단
				if (kIsInRectangle(&stButtonArea, pstMouseEvent->stPoint.iX,
					pstMouseEvent->stPoint.iY) == TRUE)
				{
					// 버튼의 배경을 밝은 녹색으로 변경하여 눌렸음을 표시
					kDrawButton(qwGUIWindowID, &stButtonArea,
						RGB(79, 204, 11), "User Message Send Button(Down)",
						RGB(255, 255, 255));
					kUpdateScreenByID(qwGUIWindowID);

					//----------------------------------------------------------
					// 다른 윈도우로 유저 이벤트를 전송
					//----------------------------------------------------------
					// 생성된 모든 윈도우를 찾아서 이벤트를 전송
					stSendEvent.qwType = EVENT_USER_TESTMESSAGE;
					stSendEvent.vqwData[0] = qwGUIWindowID;
					stSendEvent.vqwData[1] = 0x1234;
					stSendEvent.vqwData[2] = 0x5678;

					// 생성된 윈도우의 수 만큼 루프를 수행하면서 이벤트를 전송
					for (int i = 0; i < s_iWindowCount; i++)
					{
						// 윈도우 제목으로 윈도우를 검색
						sprintf(vcTempBuffer, "Hello World Window %d", i);
						qwFindWindowID = kFindWindowByTitle(vcTempBuffer);
						// 윈도우가 존재하며 윈도우 자신이 아닌 경우는 이벤트를 전송
						if ((qwFindWindowID != WINDOW_INVALIDID) &&
							(qwFindWindowID != qwGUIWindowID))
						{
							// 윈도우로 이벤트 전송
							kSendEventToWindow(qwFindWindowID, &stSendEvent);
						}
					}
				}
			}
			// 마우스 왼쪽 버튼이 떨어졌을 때 버튼 처리
			else if (stReceivedEvent.qwType == EVENT_MOUSE_LBUTTONUP)
			{
				// 버튼의 배경을 흰색으로 변경하여 눌리지 않았음을 표시
				kDrawButton(qwGUIWindowID, &stButtonArea,
					WINDOW_COLOR_BACKGROUND, "User Message Send Button(Up)",
					RGB(0, 0, 0));
			}
			break;

			// 키 이벤트 처리
		case EVENT_KEY_DOWN:
		case EVENT_KEY_UP:
			// 여기에 키보드 이벤트 처리 코드 넣기
			pstKeyEvent = &(stReceivedEvent.stKeyEvent);

			// 키 이벤트의 타입을 출력
			sprintf(vcTempBuffer, "Key Event: %s",
				vpcEventString[stReceivedEvent.qwType]);
			kDrawText(qwGUIWindowID, 20, iY + 20, RGB(0, 0, 0),
				WINDOW_COLOR_BACKGROUND, vcTempBuffer, strlen(vcTempBuffer));

			// 키 데이터를 출력
			sprintf(vcTempBuffer, "Data: Key = %c, Flag = %X",
				pstKeyEvent->bASCIICode, pstKeyEvent->bFlags);
			kDrawText(qwGUIWindowID, 20, iY + 40, RGB(0, 0, 0),
				WINDOW_COLOR_BACKGROUND, vcTempBuffer, strlen(vcTempBuffer));
			break;

			// 윈도우 이벤트 처리
		case EVENT_WINDOW_SELECT:
		case EVENT_WINDOW_DESELECT:
		case EVENT_WINDOW_MOVE:
		case EVENT_WINDOW_RESIZE:
		case EVENT_WINDOW_CLOSE:
			// 여기에 윈도우 이벤트 처리 코드 넣기
			pstWindowEvent = &(stReceivedEvent.stWindowEvent);

			// 윈도우 이벤트의 타입을 출력
			sprintf(vcTempBuffer, "Window Event: %s",
				vpcEventString[stReceivedEvent.qwType]);
			kDrawText(qwGUIWindowID, 20, iY + 20, RGB(0, 0, 0),
				WINDOW_COLOR_BACKGROUND, vcTempBuffer, strlen(vcTempBuffer));

			// 윈도우 데이터를 출력
			sprintf(vcTempBuffer, "Data: X1 = %d, Y1 = %d, X2 = %d, Y2 = %d",
				pstWindowEvent->stArea.iX1, pstWindowEvent->stArea.iY1,
				pstWindowEvent->stArea.iX2, pstWindowEvent->stArea.iY2);
			kDrawText(qwGUIWindowID, 20, iY + 40, RGB(0, 0, 0),
				WINDOW_COLOR_BACKGROUND, vcTempBuffer, strlen(vcTempBuffer));

			//------------------------------------------------------------------
			// 윈도우 닫기 이벤트이면 윈도우를 삭제하고 루프를 빠져나가 태스크를 종료
			//------------------------------------------------------------------
			if (stReceivedEvent.qwType == EVENT_WINDOW_CLOSE)
			{
				// 윈도우 삭제
				kDeleteWindow(qwGUIWindowID);
				return;
			}
			break;

			// 그 외 정보
		default:
			// 여기에 알 수 없는 이벤트 처리 코드 넣기
			// 알 수 없는 이벤트를 출력
			sprintf(vcTempBuffer, "Unknown Event: 0x%X", stReceivedEvent.qwType);
			kDrawText(qwGUIWindowID, 20, iY + 20, RGB(0, 0, 0), WINDOW_COLOR_BACKGROUND,
				vcTempBuffer, strlen(vcTempBuffer));

			// 데이터를 출력
			sprintf(vcTempBuffer, "Data0 = 0x%x, Data1 = 0x%x, Data2 = 0x%x",
				stReceivedEvent.vqwData[0], stReceivedEvent.vqwData[1],
				stReceivedEvent.vqwData[2]);
			kDrawText(qwGUIWindowID, 20, iY + 40, RGB(0, 0, 0),
				WINDOW_COLOR_BACKGROUND, vcTempBuffer, strlen(vcTempBuffer));
			break;
		}

		// 윈도우를 화면에 업데이트
		kShowWindow(qwGUIWindowID, TRUE);
	}
}

//------------------------------------------------------------------------------
//  시스템 모니터 태스크
//------------------------------------------------------------------------------
/**
 *  시스템의 상태를 감시하여 화면에 표시하는 태스크
 */
/*void kSystemMonitorTask( void )
{
    QWORD qwWindowID;
    int i;
    int iWindowWidth;
    int iProcessorCount;
    DWORD vdwLastCPULoad[ MAXPROCESSORCOUNT ];
    int viLastTaskCount[ MAXPROCESSORCOUNT ];
    QWORD qwLastTickCount;
    EVENT stReceivedEvent;
    WINDOWEVENT* pstWindowEvent;
    bool bChanged;
    RECT stScreenArea;
    QWORD qwLastDynamicMemoryUsedSize;
    QWORD qwDynamicMemoryUsedSize;
    QWORD qwTemp;
    
    //--------------------------------------------------------------------------
    // 윈도우를 생성
    //--------------------------------------------------------------------------
    // 화면 영역의 크기를 반환
    kGetScreenArea( &stScreenArea );
    
    // 현재 프로세서의 개수로 윈도우의 너비를 계산
    iProcessorCount = kGetProcessorCount();
    iWindowWidth = iProcessorCount * ( SYSTEMMONITOR_PROCESSOR_WIDTH + 
            SYSTEMMONITOR_PROCESSOR_MARGIN ) + SYSTEMMONITOR_PROCESSOR_MARGIN;
    
    // 윈도우를 화면 가운데에 생성한 뒤 화면에 표시하지 않음. 프로세서 정보와 메모리 정보를 
    // 표시하는 영역을 그린 뒤 화면에 표시
    qwWindowID = kCreateWindow( ( stScreenArea.iX2 - iWindowWidth ) / 2, 
        ( stScreenArea.iY2 - SYSTEMMONITOR_WINDOW_HEIGHT ) / 2, 
        iWindowWidth, SYSTEMMONITOR_WINDOW_HEIGHT, WINDOW_FLAGS_DEFAULT & 
        ~WINDOW_FLAGS_SHOW, "System Monitor" );
    // 윈도우를 생성하지 못했으면 실패
    if( qwWindowID == WINDOW_INVALIDID )
    {
        return ;
    }

    // 프로세서 정보를 표시하는 영역을 3픽셀 두께로 표시하고 문자열을 출력
    kDrawLine( qwWindowID, 5, WINDOW_TITLEBAR_HEIGHT + 15, iWindowWidth - 5, 
            WINDOW_TITLEBAR_HEIGHT + 15, RGB( 0, 0, 0 ) );
    kDrawLine( qwWindowID, 5, WINDOW_TITLEBAR_HEIGHT + 16, iWindowWidth - 5, 
            WINDOW_TITLEBAR_HEIGHT + 16, RGB( 0, 0, 0 ) );
    kDrawLine( qwWindowID, 5, WINDOW_TITLEBAR_HEIGHT + 17, iWindowWidth - 5, 
            WINDOW_TITLEBAR_HEIGHT + 17, RGB( 0, 0, 0 ) );
    kDrawText( qwWindowID, 9, WINDOW_TITLEBAR_HEIGHT + 8, RGB( 0, 0, 0 ), 
            WINDOW_COLOR_BACKGROUND, "Processor Information", 21 );


    // 메모리 정보를 표시하는 영역을 3픽셀 두께로 표시하고 문자열을 출력
    kDrawLine( qwWindowID, 5, WINDOW_TITLEBAR_HEIGHT + SYSTEMMONITOR_PROCESSOR_HEIGHT + 50, 
            iWindowWidth - 5, WINDOW_TITLEBAR_HEIGHT + SYSTEMMONITOR_PROCESSOR_HEIGHT + 50, 
            RGB( 0, 0, 0 ) );
    kDrawLine( qwWindowID, 5, WINDOW_TITLEBAR_HEIGHT + SYSTEMMONITOR_PROCESSOR_HEIGHT + 51, 
                iWindowWidth - 5, WINDOW_TITLEBAR_HEIGHT + SYSTEMMONITOR_PROCESSOR_HEIGHT + 51, 
                RGB( 0, 0, 0 ) );
    kDrawLine( qwWindowID, 5, WINDOW_TITLEBAR_HEIGHT + SYSTEMMONITOR_PROCESSOR_HEIGHT + 52, 
                iWindowWidth - 5, WINDOW_TITLEBAR_HEIGHT + SYSTEMMONITOR_PROCESSOR_HEIGHT + 52, 
                RGB( 0, 0, 0 ) );
    kDrawText( qwWindowID, 9, WINDOW_TITLEBAR_HEIGHT + SYSTEMMONITOR_PROCESSOR_HEIGHT + 43, 
            RGB( 0, 0, 0 ), WINDOW_COLOR_BACKGROUND, "Memory Information", 18 );
    // 윈도우를 화면에 표시
    kShowWindow( qwWindowID, TRUE );
    
    // 루프를 돌면서 시스템 정보를 감시하여 화면에 표시
    qwLastTickCount = 0;
    
    // 마지막으로 측정한 프로세서의 부하와 태스크 수, 그리고 메모리 사용량은 모두 0으로 설정
    memset( vdwLastCPULoad, 0, sizeof( vdwLastCPULoad ) );
	memset( viLastTaskCount, 0, sizeof( viLastTaskCount ) );
    qwLastDynamicMemoryUsedSize = 0;
    
    //--------------------------------------------------------------------------
    // GUI 태스크의 이벤트 처리 루프
    //--------------------------------------------------------------------------
    while( 1 )
    {
        //----------------------------------------------------------------------
        // 이벤트 큐의 이벤트 처리
        //----------------------------------------------------------------------
        // 이벤트 큐에서 이벤트를 수신
        if( kReceiveEventFromWindowQueue( qwWindowID, &stReceivedEvent ) == TRUE )
        {
            // 수신된 이벤트를 타입에 따라 나누어 처리
            switch( stReceivedEvent.qwType )
            {
                // 윈도우 이벤트 처리
            case EVENT_WINDOW_CLOSE:
                //--------------------------------------------------------------
                // 윈도우 닫기 이벤트이면 윈도우를 삭제하고 루프를 빠져나가 태스크를 종료
                //--------------------------------------------------------------
                // 윈도우 삭제
                kDeleteWindow( qwWindowID );
                return ;
                break;
                
                // 그 외 정보
            default:
                break;
            }
        }
        
        // 0.5초마다 한번씩 시스템 상태를 확인
        if( ( kGetTickCount() - qwLastTickCount ) < 500 )
        {
            ksleep( 1 );
            continue;
        }

        // 마지막으로 측정한 시간을 최신으로 업데이트
        qwLastTickCount = kGetTickCount();

        //----------------------------------------------------------------------
        // 프로세서 정보 출력
        //----------------------------------------------------------------------
        // 프로세서 수만큼 부하와 태스크 수를 확인하여 달라진 점이 있으면 화면에 업데이트
        for( i = 0 ; i < iProcessorCount ; i++ )
        {
            bChanged = FALSE;
            
            // 프로세서 부하 검사
            if( vdwLastCPULoad[ i ] != kGetProcessorLoad( i ) )
            {
                vdwLastCPULoad [ i ] = kGetProcessorLoad( i );
                bChanged = TRUE;
            }
            // 태스크 수 검사
            else if( viLastTaskCount[ i ] != kGetTaskCount( i ) )
            {
                viLastTaskCount[ i ] = kGetTaskCount( i );
                bChanged = TRUE;
            }
            
            // 이전과 비교해서 변경 사항이 있으면 화면에 업데이트
            if( bChanged == TRUE )
            {
                // 화면에 현재 프로세서의 부하를 표시 
                kDrawProcessorInformation( qwWindowID, i * SYSTEMMONITOR_PROCESSOR_WIDTH + 
                    ( i + 1 ) * SYSTEMMONITOR_PROCESSOR_MARGIN, WINDOW_TITLEBAR_HEIGHT + 28,
                    i );
            }
        }
        
        //----------------------------------------------------------------------
        // 동적 메모리 정보 출력
        //----------------------------------------------------------------------
        // 동적 메모리의 정보를 반환
        kGetDynamicMemoryInformation( &qwTemp, &qwTemp, &qwTemp, 
                &qwDynamicMemoryUsedSize );
        
        // 현재 동적 할당 메모리 사용량이 이전과 다르다면 메모리 정보를 출력
        if( qwDynamicMemoryUsedSize != qwLastDynamicMemoryUsedSize )
        {
            qwLastDynamicMemoryUsedSize = qwDynamicMemoryUsedSize;
            
            // 메모리 정보를 출력
            kDrawMemoryInformation( qwWindowID, WINDOW_TITLEBAR_HEIGHT + 
                    SYSTEMMONITOR_PROCESSOR_HEIGHT + 60, iWindowWidth );
        }
    }
}*/

/**
 *  개별 프로세서의 정보를 화면에 표시
 */
/*static void kDrawProcessorInformation( QWORD qwWindowID, int iX, int iY, 
        BYTE bAPICID )
{
    char vcBuffer[ 100 ];
    RECT stArea;
    QWORD qwProcessorLoad;
    QWORD iUsageBarHeight;
    int iMiddleX;
    
    // 프로세서 ID를 표시
	sprintf( vcBuffer, "Processor ID: %d", bAPICID );
    kDrawText( qwWindowID, iX + 10, iY, RGB( 0, 0, 0 ), WINDOW_COLOR_BACKGROUND, 
            vcBuffer, strlen( vcBuffer ) );
    
    // 프로세서의 태스크 개수를 표시
	sprintf( vcBuffer, "Task Count: %d   ", kGetTaskCount( bAPICID ) );
    kDrawText( qwWindowID, iX + 10, iY + 18, RGB( 0, 0, 0 ), WINDOW_COLOR_BACKGROUND,
            vcBuffer, strlen( vcBuffer ) );

    //--------------------------------------------------------------------------
    // 프로세서 부하를 나타내는 막대를 표시
    //--------------------------------------------------------------------------
    // 프로세서 부하를 표시
    qwProcessorLoad = kGetProcessorLoad( bAPICID );
    if( qwProcessorLoad > 100 )
    {
        qwProcessorLoad = 100;
    }
    
    // 부하를 표시하는 막대의 전체에 테두리를 표시
    kDrawRect( qwWindowID, iX, iY + 36, iX + SYSTEMMONITOR_PROCESSOR_WIDTH, 
            iY + SYSTEMMONITOR_PROCESSOR_HEIGHT, RGB( 0, 0, 0 ), FALSE );

    // 프로세서 사용량을 나타내는 막대의 길이, ( 막대 전체의 길이 * 프로세서 부하 / 100 ) 
    iUsageBarHeight = ( SYSTEMMONITOR_PROCESSOR_HEIGHT - 40 ) * qwProcessorLoad / 100;

    // 부하를 표시하는 영역의 막대 내부를 표시
    // 채워진 막대를 표시, 테두리와 2픽셀 정도 여유 공간을 둠 
    kDrawRect( qwWindowID, iX + 2,
        iY + ( SYSTEMMONITOR_PROCESSOR_HEIGHT - iUsageBarHeight ) - 2, 
        iX + SYSTEMMONITOR_PROCESSOR_WIDTH - 2, 
        iY + SYSTEMMONITOR_PROCESSOR_HEIGHT - 2, SYSTEMMONITOR_BAR_COLOR, TRUE );
    // 빈 막대를 표시, 테두리와 2픽셀 정도 여유 공간을 둠
    kDrawRect( qwWindowID, iX + 2, iY + 38, iX + SYSTEMMONITOR_PROCESSOR_WIDTH - 2,
            iY + ( SYSTEMMONITOR_PROCESSOR_HEIGHT - iUsageBarHeight ) - 1, 
            WINDOW_COLOR_BACKGROUND, TRUE );
    
    // 프로세서의 부하를 표시, 막대의 가운데에 부하가 표시되도록 함
	sprintf( vcBuffer, "Usage: %d%%", qwProcessorLoad );
    iMiddleX = ( SYSTEMMONITOR_PROCESSOR_WIDTH - 
            (strlen( vcBuffer ) * FONT_ENGLISHWIDTH ) ) / 2;
    kDrawText( qwWindowID, iX + iMiddleX, iY + 80, RGB( 0, 0, 0 ), 
            WINDOW_COLOR_BACKGROUND, vcBuffer, strlen( vcBuffer ) );
    
    // 프로세서 정보가 표시된 영역만 다시 화면에 업데이트
    kSetRectangleData( iX, iY, iX + SYSTEMMONITOR_PROCESSOR_WIDTH, 
            iY + SYSTEMMONITOR_PROCESSOR_HEIGHT, &stArea );    
    kUpdateScreenByWindowArea( qwWindowID, &stArea );
}*/

/**
 *  메모리 정보를 출력
 */
/*static void kDrawMemoryInformation( QWORD qwWindowID, int iY, int iWindowWidth )
{
    char vcBuffer[ 100 ];
    QWORD qwTotalRAMKbyteSize;
    QWORD qwDynamicMemoryStartAddress;
    QWORD qwDynamicMemoryUsedSize;
    QWORD qwUsedPercent;
    QWORD qwTemp;
    int iUsageBarWidth;
    RECT stArea;
    int iMiddleX;
    
    // Mbyte 단위의 메모리를 Kbyte 단위로 변환
    qwTotalRAMKbyteSize = kGetTotalRAMSize() * 1024;
    
    // 메모리 정보를 표시
	sprintf( vcBuffer, "Total Size: %d KB        ", qwTotalRAMKbyteSize );
    kDrawText( qwWindowID, SYSTEMMONITOR_PROCESSOR_MARGIN + 10, iY + 3, RGB( 0, 0, 0 ), 
            WINDOW_COLOR_BACKGROUND, vcBuffer, strlen( vcBuffer ) );
    
    // 동적 메모리의 정보를 반환
    kGetDynamicMemoryInformation( &qwDynamicMemoryStartAddress, &qwTemp, 
            &qwTemp, &qwDynamicMemoryUsedSize );
    
	sprintf( vcBuffer, "Used Size: %d KB        ", ( qwDynamicMemoryUsedSize +
            qwDynamicMemoryStartAddress ) / 1024 );
    kDrawText( qwWindowID, SYSTEMMONITOR_PROCESSOR_MARGIN + 10, iY + 21, RGB( 0, 0, 0 ), 
            WINDOW_COLOR_BACKGROUND, vcBuffer, strlen( vcBuffer ) );
    
    //--------------------------------------------------------------------------
    // 메모리 사용량을 나타내는 막대를 표시
    //--------------------------------------------------------------------------
    // 메모리 사용량을 표시하는 막대의 전체에 테두리를 표시
    kDrawRect( qwWindowID, SYSTEMMONITOR_PROCESSOR_MARGIN, iY + 40,
            iWindowWidth - SYSTEMMONITOR_PROCESSOR_MARGIN, 
            iY + SYSTEMMONITOR_MEMORY_HEIGHT - 32, RGB( 0, 0, 0 ), FALSE );
    // 메모리 사용량(%)을 계산
    qwUsedPercent = ( qwDynamicMemoryStartAddress + qwDynamicMemoryUsedSize ) * 
        100 / 1024 / qwTotalRAMKbyteSize;
    if( qwUsedPercent > 100 )
    {
        qwUsedPercent = 100;
    }
    
    // 메모리 사용량을 나타내는 막대의 길이, ( 막대 전체의 길이 * 메모리 사용량 / 100 )     
    iUsageBarWidth = ( iWindowWidth - 2 * SYSTEMMONITOR_PROCESSOR_MARGIN ) * 
        qwUsedPercent / 100;
    
    // 메모리 사용량을 표시하는 영역의 막대 내부를 표시
    // 색칠된 막대를 표시, 테두리와 2픽셀 정도 여유 공간을 둠 
    kDrawRect( qwWindowID, SYSTEMMONITOR_PROCESSOR_MARGIN + 2, iY + 42, 
            SYSTEMMONITOR_PROCESSOR_MARGIN + 2 + iUsageBarWidth, 
            iY + SYSTEMMONITOR_MEMORY_HEIGHT - 34, SYSTEMMONITOR_BAR_COLOR, TRUE );  
    // 빈 막대를 표시, 테두리와 2픽셀 정도 여유 공간을 둠
    kDrawRect( qwWindowID, SYSTEMMONITOR_PROCESSOR_MARGIN + 2 + iUsageBarWidth, 
        iY + 42, iWindowWidth - SYSTEMMONITOR_PROCESSOR_MARGIN - 2,
        iY + SYSTEMMONITOR_MEMORY_HEIGHT - 34, WINDOW_COLOR_BACKGROUND, TRUE );    
    
    // 사용량을 나타내는 텍스트 표시, 막대의 가운데에 사용량이 표시되도록 함
	sprintf( vcBuffer, "Usage: %d%%", qwUsedPercent );
    iMiddleX = ( iWindowWidth - (strlen( vcBuffer ) * FONT_ENGLISHWIDTH ) ) / 2;
    kDrawText( qwWindowID, iMiddleX, iY + 45, RGB( 0, 0, 0 ), WINDOW_COLOR_BACKGROUND, 
            vcBuffer, strlen( vcBuffer ) );
    
    // 메모리 정보가 표시된 영역만 화면에 다시 업데이트
    kSetRectangleData(0, iY, iWindowWidth, iY + SYSTEMMONITOR_MEMORY_HEIGHT, &stArea );
    kUpdateScreenByWindowArea( qwWindowID, &stArea );
}*/

//------------------------------------------------------------------------------
//  GUI 버전의 콘솔 셸 태스크
//------------------------------------------------------------------------------
// 이전 화면 버퍼의 값을 보관하는 영역
#define CONSOLE_WIDTH                       80
#define CONSOLE_HEIGHT                      25
#include "Console.h"
static CHARACTER gs_vstPreviousScreenBuffer[ CONSOLE_WIDTH * CONSOLE_HEIGHT ];

/**
 *  GUI 버전의 콘솔 셸 태스크
 */
#include "SkyInterface.h"
extern SKY_PROCESS_INTERFACE g_processInterface;

DWORD WINAPI kGUIConsoleShellTask(LPVOID parameter)
{
    static QWORD s_qwWindowID = WINDOW_INVALIDID;
    int iWindowWidth, iWindowHeight;
    EVENT stReceivedEvent;
    KEYEVENT* pstKeyEvent;
    RECT stScreenArea;
    KEYDATA stKeyData;
   // TCB* pstConsoleShellTask;
    QWORD qwConsoleShellTaskID;


    // GUI 콘솔 셸 윈도우가 존재하면 생성된 윈도우를 최상위로 만들고 태스크 종료
    if( s_qwWindowID != WINDOW_INVALIDID )
    {
        kMoveWindowToTop( s_qwWindowID );
        return 0;
    }
    
    //--------------------------------------------------------------------------
    // 윈도우를 생성
    //--------------------------------------------------------------------------
    // 전체 화면 영역의 크기를 반환
    kGetScreenArea( &stScreenArea );
    
    // 윈도우의 크기 설정, 화면 버퍼에 들어가는 문자의 최대 너비와 높이를 고려해서 계산
    iWindowWidth = CONSOLE_WIDTH * FONT_ENGLISHWIDTH + 4;
    iWindowHeight = CONSOLE_HEIGHT * FONT_ENGLISHHEIGHT + WINDOW_TITLEBAR_HEIGHT + 2;
    
    // 윈도우 생성 함수 호출, 화면 가운데에 생성
    s_qwWindowID = kCreateWindow( ( stScreenArea.iX2 - iWindowWidth ) / 2, 
        ( stScreenArea.iY2 - iWindowHeight ) / 2, iWindowWidth, iWindowHeight, 
        WINDOW_FLAGS_DEFAULT, "Console Shell for GUI" );
    // 윈도우를 생성하지 못했으면 실패
    if( s_qwWindowID == WINDOW_INVALIDID )
    {
        return 0;
    }

	g_processInterface.sky_kcreate_thread_from_memory(1, kStartConsoleShell, NULL);

    // 셸 커맨드를 처리하는 콘솔 셸 태스크를 생성
   // kSetConsoleShellExitFlag( FALSE );
   // pstConsoleShellTask = kCreateTask( TASK_FLAGS_LOW | TASK_FLAGS_THREAD, 0, 0, 
     //   ( QWORD ) kStartConsoleShell, TASK_LOADBALANCINGID );
    //if( pstConsoleShellTask == NULL )
    {
        // 콘솔 셸 태스크 생성에 실패하면 윈도우를 삭제하고 종료
//        kDeleteWindow( s_qwWindowID );
      //  return ;
    }
    // 콘솔 셸 태스크의 ID를 저장
    //qwConsoleShellTaskID = pstConsoleShellTask->stLink.qwID;

    // 이전 화면 버퍼를 초기화
    memset( gs_vstPreviousScreenBuffer, 0xFF, sizeof( gs_vstPreviousScreenBuffer ) );
    
    //--------------------------------------------------------------------------
    // GUI 태스크의 이벤트 처리 루프
    //--------------------------------------------------------------------------
    while( 1 )
    {
        // 화면 버퍼의 변경된 내용을 윈도우에 업데이트
        kProcessConsoleBuffer( s_qwWindowID );
        
        // 이벤트 큐에서 이벤트를 수신
        if( kReceiveEventFromWindowQueue( s_qwWindowID, &stReceivedEvent ) == FALSE )
        {
			//20180628
            ksleep( 0 );
            continue;
        }
        
        // 수신된 이벤트를 타입에 따라 나누어 처리
        switch( stReceivedEvent.qwType )
        {
            // 키 이벤트 처리
        case EVENT_KEY_DOWN:
        case EVENT_KEY_UP:
            // 여기에 키보드 이벤트 처리 코드 넣기
            pstKeyEvent = &( stReceivedEvent.stKeyEvent );
            stKeyData.bASCIICode = pstKeyEvent->bASCIICode;
            stKeyData.bFlags = pstKeyEvent->bFlags;
            stKeyData.bScanCode = pstKeyEvent->bScanCode;

            // 키를 그래픽 모드용 키 큐로 삽입하여 셸 태스크의 입력으로 전달
            kPutKeyToGUIKeyQueue( &stKeyData );
            break;

            // 윈도우 이벤트 처리
        case EVENT_WINDOW_CLOSE:
            // 생성한 셸 태스크가 종료되도록 종료 플래그를 설정하고 종료될 때까지 대기
           // kSetConsoleShellExitFlag( TRUE );
            //while( kIsTaskExist( qwConsoleShellTaskID ) == TRUE )
            {
				//20180628
			ksleep( 1 );
            }
            
            // 윈도우를 삭제하고 윈도우 ID를 초기화
            kDeleteWindow( s_qwWindowID );
            s_qwWindowID = WINDOW_INVALIDID;            
            return 0;
            
            break;
            
            // 그 외 정보
        default:
            // 여기에 알 수 없는 이벤트 처리 코드 넣기
            break;
        }
    }

	return 0;
}

//
 //  화면 버퍼의 변경된 내용을 GUI 콘솔 셸 윈도우 화면에 업데이트
 //
static void kProcessConsoleBuffer(QWORD qwWindowID)
{
	int i;
	int j;
	CONSOLEMANAGER* pstManager;
	CHARACTER* pstScreenBuffer;
	CHARACTER* pstPreviousScreenBuffer;
	RECT stLineArea;
	bool bChanged;
	static QWORD s_qwLastTickCount = 0;
	bool bFullRedraw;

	// 콘솔을 관리하는 자료구조를 반환 받아 화면 버퍼의 어드레스를 저장하고 
	// 이전 화면 버퍼의 어드레스도 저장
	pstManager = kGetConsoleManager();
	pstScreenBuffer = pstManager->pstScreenBuffer;
	pstPreviousScreenBuffer = gs_vstPreviousScreenBuffer;

	// 화면을 전체를 업데이트 한 지 5초가 지났으면 무조건 화면 전체를 다시 그림
	/*if (kGetTickCount() - s_qwLastTickCount > 5000)
	{
		s_qwLastTickCount = kGetTickCount();
		bFullRedraw = TRUE;
	}
	else*/
	{
		bFullRedraw = FALSE;
	}

	// 화면 버퍼의 높이만큼 반복
	for (j = 0; j < CONSOLE_HEIGHT; j++)
	{
		// 처음에는 변경되지 않은 것으로 플래그 설정
		bChanged = FALSE;

		// 현재 라인에 변화가 있는지 비교하여 변경 여부 플래그를 처리
		for (i = 0; i < CONSOLE_WIDTH; i++)
		{
			// 문자를 비교하여 다르거나 전체를 새로 그려야 하면 이전 화면 버퍼에
			// 업데이트하고 변경 여부 플래그를 설정
			if ((pstScreenBuffer->bCharactor != pstPreviousScreenBuffer->bCharactor) ||
				(bFullRedraw == TRUE))
			{
				// 문자를 화면에 출력
				kDrawText(qwWindowID, i * FONT_ENGLISHWIDTH + 2,
					j * FONT_ENGLISHHEIGHT + WINDOW_TITLEBAR_HEIGHT,
					RGB(0, 255, 0), RGB(0, 0, 0),
					(const char*)&(pstScreenBuffer->bCharactor), 1);

				// 이전 화면 버퍼로 값을 옮겨 놓고 현재 라인에 이전과
				// 다른 데이터가 있음을 표시
				memcpy(pstPreviousScreenBuffer, pstScreenBuffer, sizeof(CHARACTER));
				bChanged = TRUE;
			}

			pstScreenBuffer++;
			pstPreviousScreenBuffer++;
		}

		// 현재 라인에서 변경된 데이터가 있다면 현재 라인만 화면에 업데이트
		if (bChanged == TRUE)
		{
			// 현재 라인의 영역을 계산
			kSetRectangleData(2, j * FONT_ENGLISHHEIGHT + WINDOW_TITLEBAR_HEIGHT,
				5 + FONT_ENGLISHWIDTH * CONSOLE_WIDTH, (j + 1) * FONT_ENGLISHHEIGHT +
				WINDOW_TITLEBAR_HEIGHT - 1, &stLineArea);
			// 윈도우의 일부만 화면 업데이트
			kUpdateScreenByWindowArea(qwWindowID, &stLineArea);
		}
	}
}

//------------------------------------------------------------------------------
//  이미지 뷰어(Image Viewer)
//------------------------------------------------------------------------------
/**
 *  이미지 뷰어 태스크
 */
 // GUI 콘솔 셸 태스크 함수
DWORD WINAPI kImageViewerTask(LPVOID parameter)
{
    QWORD qwWindowID;
    int iMouseX, iMouseY;
    int iWindowWidth, iWindowHeight;
    int iEditBoxWidth;
    RECT stEditBoxArea;
    RECT stButtonArea;
    RECT stScreenArea;
    EVENT stReceivedEvent;
    EVENT stSendEvent;
    char vcFileName[FILESYSTEM_MAXFILENAMELENGTH + 1 ];
    int iFileNameLength;
    MOUSEEVENT* pstMouseEvent;
    KEYEVENT* pstKeyEvent;
    POINT stScreenXY;
    POINT stClientXY;
    
    //--------------------------------------------------------------------------
    // 윈도우를 생성
    //--------------------------------------------------------------------------
    // 전체 화면 영역의 크기를 반환
    kGetScreenArea( &stScreenArea );
    
    // 윈도우의 크기 설정, 화면 버퍼에 들어가는 문자의 최대 너비와 높이를 고려해서 계산
    iWindowWidth = FONT_ENGLISHWIDTH * FILESYSTEM_MAXFILENAMELENGTH + 165;
    iWindowHeight = 35 + WINDOW_TITLEBAR_HEIGHT + 5;
    
    // 윈도우 생성 함수 호출, 화면 가운데에 생성
    qwWindowID = kCreateWindow( ( stScreenArea.iX2 - iWindowWidth ) / 2, 
        ( stScreenArea.iY2 - iWindowHeight ) / 2, iWindowWidth, iWindowHeight, 
        WINDOW_FLAGS_DEFAULT & ~WINDOW_FLAGS_SHOW, "Image Viewer" );
    // 윈도우를 생성하지 못했으면 실패
    if( qwWindowID == WINDOW_INVALIDID )
    {
        return 0;
    }
    
    // 파일 이름을 입력하는 에디트 박스 영역을 표시
    kDrawText( qwWindowID, 5, WINDOW_TITLEBAR_HEIGHT + 6, RGB( 0, 0, 0 ), 
            WINDOW_COLOR_BACKGROUND, "FILE NAME", 9 );
    iEditBoxWidth = FONT_ENGLISHWIDTH * FILESYSTEM_MAXFILENAMELENGTH + 4;
    kSetRectangleData( 85, WINDOW_TITLEBAR_HEIGHT + 5, 85 + iEditBoxWidth, 
                       WINDOW_TITLEBAR_HEIGHT + 25, &stEditBoxArea );
    kDrawRect( qwWindowID, stEditBoxArea.iX1, stEditBoxArea.iY1, 
            stEditBoxArea.iX2, stEditBoxArea.iY2, RGB( 0, 0, 0 ), FALSE );

    // 파일 이름 버퍼를 비우고 에디트 박스에 빈 파일 이름을 표시
    iFileNameLength = 0;
    memset( vcFileName, 0, sizeof( vcFileName ) );
    kDrawFileName( qwWindowID, &stEditBoxArea, vcFileName, iFileNameLength );
    
    // 이미지 출력 버튼 영역을 지정
    kSetRectangleData( stEditBoxArea.iX2 + 10, stEditBoxArea.iY1, 
                       stEditBoxArea.iX2 + 70, stEditBoxArea.iY2, &stButtonArea );
    kDrawButton( qwWindowID, &stButtonArea, WINDOW_COLOR_BACKGROUND, "Show", 
            RGB( 0, 0, 0 ) );
    
    // 윈도우를 표시
    kShowWindow( qwWindowID, TRUE );
    
    //--------------------------------------------------------------------------
    // GUI 태스크의 이벤트 처리 루프
    //--------------------------------------------------------------------------
    while( 1 )
    {
        // 이벤트 큐에서 이벤트를 수신
        if( kReceiveEventFromWindowQueue( qwWindowID, &stReceivedEvent ) == FALSE )
        {
			//20180628
            ksleep( 0 );
            continue;
        }
        
        // 수신된 이벤트를 타입에 따라 나누어 처리
        switch( stReceivedEvent.qwType )
        {
            // 마우스 이벤트 처리
        case EVENT_MOUSE_LBUTTONDOWN:
            pstMouseEvent = &( stReceivedEvent.stMouseEvent );

            // 마우스 왼쪽 버튼이 이미지 출력 버튼 위에서 눌러졌으면 저장된 파일 이름을 
            // 이용하여 이미지를 화면에 표시
            if( kIsInRectangle( &stButtonArea, pstMouseEvent->stPoint.iX, 
                                pstMouseEvent->stPoint.iY ) == TRUE )
            {
                // 버튼을 눌린 것으로 표시
                kDrawButton( qwWindowID, &stButtonArea, RGB( 79, 204, 11 ), "Show", 
                            RGB( 255, 255, 255 ) );
                // 버튼이 있는 영역만 화면 업데이트
                kUpdateScreenByWindowArea( qwWindowID, &( stButtonArea ) );

                // 이미지 출력 윈도우를 생성하고 이벤트를 처리
                if( kCreateImageViewerWindowAndExecute( qwWindowID, vcFileName ) 
                        == FALSE )
                {
                    // 윈도우 생성에 실패하면 버튼이 눌려졌다가 떨어지는 효과를 주려고
                    // 200ms 대기
					//20180628	
					ksleep( 200 );
                }
                
                // 버튼을 떨어진 것으로 표시
                kDrawButton( qwWindowID, &stButtonArea, WINDOW_COLOR_BACKGROUND,
                        "Show", RGB( 0, 0, 0 ) );
                // 버튼이 있는 영역만 화면 업데이트
                kUpdateScreenByWindowArea( qwWindowID, &( stButtonArea ) );
            }
            break;

            // 키 이벤트 처리
        case EVENT_KEY_DOWN:
            pstKeyEvent = &( stReceivedEvent.stKeyEvent );
            
            // 백스페이스(Backspace) 키는 삽입된 문자를 삭제
            if( ( pstKeyEvent->bASCIICode == KEY_BACKSPACE ) &&
                ( iFileNameLength > 0 ) )
            {
                // 버퍼에 삽입된 마지막 문자를 삭제
                vcFileName[ iFileNameLength ] = '\0';
                iFileNameLength--;
                
                // 입력된 내용을 에디트 박스에 표시
                kDrawFileName( qwWindowID, &stEditBoxArea, vcFileName, 
                        iFileNameLength );
            }
            // 엔터(Enter) 키는 이미지 출력 버튼이 눌린 것으로 처리
            else if( ( pstKeyEvent->bASCIICode == KEY_ENTER ) &&
                     ( iFileNameLength > 0 ) )
            {
                // 버튼의 XY 좌표를 화면 좌표로 변환하여 마우스 이벤트의 좌표로 사용
                stClientXY.iX = stButtonArea.iX1 + 1;
                stClientXY.iY = stButtonArea.iY1 + 1;                
                kConvertPointClientToScreen( qwWindowID, &stClientXY, &stScreenXY );
                
                // 이미지 출력 버튼에 마우스 왼쪽 버튼이 눌린 것처럼 마우스 이벤트를 전송
                kSetMouseEvent( qwWindowID, EVENT_MOUSE_LBUTTONDOWN, 
                                stScreenXY.iX + 1, stScreenXY.iY + 1, 0, &stSendEvent );
                kSendEventToWindow( qwWindowID, &stSendEvent );
            }
            // ESC 키는 윈도우 닫힘 버튼이 눌린 것으로 처리
            else if( pstKeyEvent->bASCIICode == KEY_ESC )
            {
                // 윈도우 닫기 이벤트를 윈도우로 전송
                kSetWindowEvent( qwWindowID, EVENT_WINDOW_CLOSE, &stSendEvent );
                kSendEventToWindow( qwWindowID, &stSendEvent );
            }
            // 그 외 키는 파일 이름 버퍼에 공간이 있는 경우만 버퍼에 삽입
            else if( ( pstKeyEvent->bASCIICode <= 128 ) && 
                     ( pstKeyEvent->bASCIICode != KEY_BACKSPACE ) &&
                     ( iFileNameLength < FILESYSTEM_MAXFILENAMELENGTH) )
            {
                // 입력된 키를 파일 이름 버퍼의 마지막에 삽입
                vcFileName[ iFileNameLength ] = pstKeyEvent->bASCIICode;
                iFileNameLength++;
                
                // 입력된 내용을 에디트 박스에 표시
                kDrawFileName( qwWindowID, &stEditBoxArea, vcFileName, 
                        iFileNameLength );
            }            
            break;

            // 윈도우 이벤트 처리
        case EVENT_WINDOW_CLOSE:
            if( stReceivedEvent.qwType == EVENT_WINDOW_CLOSE )
            {
                // 윈도우 삭제
                kDeleteWindow( qwWindowID );
                return 0;
            }
            break;
            
            // 그 외 정보
        default:
            // 여기에 알 수 없는 이벤트 처리 코드 넣기
            break;
        }
    }

	return 1;
}

/**
 *  에디트 박스 영역에 문자를 출력
 */
static void kDrawFileName( QWORD qwWindowID, RECT* pstArea, char *pcFileName, 
        int iNameLength )
{
    // 에디트 박스의 배경을 모두 흰색으로 채움
    kDrawRect( qwWindowID, pstArea->iX1 + 1, pstArea->iY1 + 1, pstArea->iX2 - 1, 
               pstArea-> iY2 - 1, WINDOW_COLOR_BACKGROUND, TRUE );
    
    // 파일 이름을 출력
    kDrawText( qwWindowID, pstArea->iX1 + 2, pstArea->iY1 + 2, RGB( 0, 0, 0 ), 
            WINDOW_COLOR_BACKGROUND, pcFileName, iNameLength );
    
    // 파일 이름의 길이가 파일 시스템이 정의한 최대 길이가 아니면 커서를 출력
    if( iNameLength < FILESYSTEM_MAXFILENAMELENGTH)
    {
        kDrawText( qwWindowID, pstArea->iX1 + 2 + FONT_ENGLISHWIDTH * iNameLength,
            pstArea->iY1 + 2, RGB( 0, 0, 0 ), WINDOW_COLOR_BACKGROUND, "_", 1 );
    }
    
    // 에디트 박스 영역만 화면 업데이트
    kUpdateScreenByWindowArea( qwWindowID, pstArea );
}

/**
 *  JPEG 파일을 읽어서 새로 생성한 윈도우에 표시하고 이벤트를 처리
 */
static bool kCreateImageViewerWindowAndExecute( QWORD qwMainWindowID, 
        const char* pcFileName )
{    
    struct dirent* pstEntry;
    DWORD dwFileSize;
    RECT stScreenArea;
    QWORD qwWindowID;
    WINDOW* pstWindow;
    BYTE* pbFileBuffer;
    COLOR* pstOutputBuffer;
    int iWindowWidth;
    FILE* fp;
    JPEG* pstJpeg;
    EVENT stReceivedEvent;
    KEYEVENT* pstKeyEvent;
    bool bExit;
    
    // 초기화
    fp = NULL;
    pbFileBuffer = NULL;
    pstOutputBuffer = NULL;
    qwWindowID = WINDOW_INVALIDID;
    
    //--------------------------------------------------------------------------
    //  파일을 읽은 후 이미지 디코딩
    //--------------------------------------------------------------------------
    // 파일 읽기
    fp = fopen( pcFileName, "rb" );
    if( fp == NULL )
    {
        printf( "[ImageViewer] %s file open fail\n", pcFileName );
        return FALSE;
    }

	fseek(fp, 0, SEEK_END);
	dwFileSize = ftell(fp);
	fseek(fp, 0, SEEK_SET);

	if (dwFileSize == 0)
	{
		printf("[ImageViewer] %s file doesn't exist or size is zero\n", pcFileName);
		return FALSE;
	}
    
    // 메모리를 파일 크기만큼 할당하고 JPEG 자료구조를 할당
    pbFileBuffer = ( BYTE* ) new char[ dwFileSize ];
    pstJpeg = ( JPEG* ) new char[ sizeof( JPEG ) ];
    if( ( pbFileBuffer == NULL ) || ( pstJpeg == NULL ) )
    {
        printf( "[ImageViewer] Buffer allocation fail\n" );
        delete ( pbFileBuffer );
		delete ( pstJpeg );
        fclose( fp );
        return FALSE;
    }
    
    // 파일을 읽은 후 JPEG 파일 포맷인지 확인
    if( ( fread( pbFileBuffer, 1, dwFileSize, fp ) != dwFileSize ) ||
        ( kJPEGInit( pstJpeg, pbFileBuffer, dwFileSize ) == FALSE ) )
    {
		printf( "[ImageViewer] Read fail or file is not JPEG format\n" );
		delete ( pbFileBuffer );
		delete ( pstJpeg );
        fclose( fp );
        return FALSE;
    }

    // 디코드 결과 출력용 버퍼를 생성
    pstOutputBuffer = (COLOR*)new char[ pstJpeg->width * pstJpeg->height * sizeof( COLOR ) ];
    // 디코드를 수행한 뒤 정상적으로 처리되었다면 윈도우를 생성
    if( ( pstOutputBuffer != NULL ) &&
        ( kJPEGDecode( pstJpeg, pstOutputBuffer ) == TRUE ) )
    {
        // 전체 화면 영역의 크기를 반환
        kGetScreenArea( &stScreenArea );
        // 윈도우를 생성, 이미지의 크기와 제목 표시줄의 크기를 고려
        qwWindowID = kCreateWindow( ( stScreenArea.iX2 - pstJpeg->width ) / 2, 
                    ( stScreenArea.iY2 - pstJpeg->height ) / 2, pstJpeg->width, 
                    pstJpeg->height + WINDOW_TITLEBAR_HEIGHT, 
                    WINDOW_FLAGS_DEFAULT & ~WINDOW_FLAGS_SHOW | WINDOW_FLAGS_RESIZABLE, 
                    pcFileName );
    }
    
    // 윈도우 생성에 실패하거나 출력 버퍼 할당 또는 디코딩에 실패하면 종료
    if( ( qwWindowID == WINDOW_INVALIDID ) || ( pstOutputBuffer == NULL ) )
    {
        printf ( "[ImageViewer] Window create fail or output buffer allocation fail\n" );
		delete ( pbFileBuffer );
		delete ( pstJpeg );
		delete ( pstOutputBuffer );
        kDeleteWindow( qwWindowID );
        return FALSE;
    }

    // 윈도우의 너비를 구하여 제목 표시줄 영역을 제외한 나머지 화면 버퍼 영역에 디코딩된
    // 이미지를 복사
	kEnterCriticalSection();
    pstWindow = kGetWindowWithWindowLock( qwWindowID );
    if( pstWindow != NULL )
    {
        iWindowWidth = kGetRectangleWidth( &( pstWindow->stArea ) );
        memcpy( pstWindow->pstWindowBuffer + ( WINDOW_TITLEBAR_HEIGHT * 
                iWindowWidth ), pstOutputBuffer, pstJpeg->width * 
                pstJpeg->height * sizeof( COLOR ) );

        // 동기화 처리
        //kUnlock( &( pstWindow->stLock ) );
		
    }
	kLeaveCriticalSection();
    
    // 파일 버퍼를 해제하고 윈도우를 화면에 표시
    delete ( pbFileBuffer );
    kShowWindow( qwWindowID, TRUE );
    
    //--------------------------------------------------------------------------
    //  ESC 키와 윈도우 닫기 버튼을 처리하는 간단한 이벤트 루프
    //--------------------------------------------------------------------------
    // 정상적으로 윈도우를 생성하여 표시했으면 파일 이름 입력 윈도우는 숨김
    kShowWindow( qwMainWindowID, FALSE );
    
    bExit = FALSE;
    while( bExit == FALSE )
    {
        // 이벤트 큐에서 이벤트를 수신
        if( kReceiveEventFromWindowQueue( qwWindowID, &stReceivedEvent ) == FALSE )
        {
			//20180628
            ksleep( 0 );
            continue;
        }
        
        // 수신된 이벤트를 타입에 따라 나누어 처리
        switch( stReceivedEvent.qwType )
        {
            // 키 이벤트 처리
        case EVENT_KEY_DOWN:
            pstKeyEvent = &( stReceivedEvent.stKeyEvent );
            // ESC 키가 눌리면 그림을 표시하는 윈도우를 삭제하고 파일 이름 입력 윈도우를
            // 표시한 뒤 종료
            if( pstKeyEvent->bASCIICode == KEY_ESC )
            {
                kDeleteWindow( qwWindowID );
                kShowWindow( qwMainWindowID, TRUE );
                bExit = TRUE;
            }                
            break;

            // 윈도우 이벤트 처리
            // 윈도우 크기 변경 이벤트를 처리
        case EVENT_WINDOW_RESIZE:
            // 변경된 윈도우에 디코딩된 이미지를 전송
            kBitBlt( qwWindowID, 0, WINDOW_TITLEBAR_HEIGHT, pstOutputBuffer, 
                     pstJpeg->width, pstJpeg->height );
            // 윈도우를 한번 더 표시하여 윈도우 내부에 전송된 이미지를 화면에 업데이트
            kShowWindow( qwWindowID, TRUE );
            break;
            
            // 윈도우 닫기 이벤트를 처리
        case EVENT_WINDOW_CLOSE:
            // 닫기 버튼이 눌리면 이미지 출력 윈도우를 삭제하고 파일 이름 입력 윈도우를
            // 표시한 뒤 종료
            if( stReceivedEvent.qwType == EVENT_WINDOW_CLOSE )
            {
                kDeleteWindow( qwWindowID );
                kShowWindow( qwMainWindowID, TRUE );
                bExit = TRUE;
            }
            break;
            
            // 그 외 정보
        default:
            // 여기에 알 수 없는 이벤트 처리 코드 넣기
            break;
        }
    }

    // JPEG 이미지 파일 디코딩에 사용했던 버퍼를 반환
    delete ( pstJpeg );
	delete ( pstOutputBuffer );
    
    return TRUE;
}
