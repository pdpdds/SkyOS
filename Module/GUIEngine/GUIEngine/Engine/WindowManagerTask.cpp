/**
 *  file    WindowManager.c
 *  date    2009/10/04
 *  author  kkamagui 
 *          Copyright(c)2008 All rights reserved by kkamagui
 *  brief   윈도우 매니저에 관련된 함수를 정의한 소스 파일
 */
 
#include "windef.h"
#include "memory.h"
#include "WindowManagerTask.h"
#include "Mouse.h"
#include "ApplicationPanelTask.h"
#include "SkyInterface.h"
#include "Console.h"

extern void printf(const char* str, ...);
extern void kHelloWorldGUITask(void);
extern DWORD WINAPI kGUIConsoleShellTask(LPVOID parameter);
extern void kApplicationPanelGUITask(void);
extern SKY_PROCESS_INTERFACE g_processInterface;

/**
 *  윈도우 매니저 태스크
 */
void kStartWindowManager( void )
{
    int iMouseX, iMouseY;
	kInitializeMouse();
	kInitializeKeyboard();
	kInitializeConsole(0, 0);

    // GUI 시스템을 초기화
    kInitializeGUISystem();
    
    // 현재 마우스 위치에 커서를 출력
    kGetCursorPosition( &iMouseX, &iMouseY );
    kMoveCursor( iMouseX, iMouseY );
    
    // 애플리케이션 패널 태스크를 실행
   // kCreateTask( TASK_FLAGS_SYSTEM | TASK_FLAGS_THREAD | TASK_FLAGS_LOW, 0, 0, 
     //       ( QWORD ) kApplicationPanelGUITask, TASK_LOADBALANCINGID );
	//if ((kCreateApplicationPanelWindow() == FALSE) ||
	//	(kCreateApplicationListWindow() == FALSE))
	//{
	//	return;
	//}

	//kHelloWorldGUITask();
	
	g_processInterface.sky_kcreate_thread_from_memory(1, kApplicationPanelGUITask, NULL);
	g_processInterface.sky_kcreate_thread_from_memory(1, kGUIConsoleShellTask, NULL);	
    
}

void kUpdate(bool isDirectBuffer)
{
	bool bMouseDataResult;
	bool bKeyDataResult;
	bool bEventQueueResult;
    WINDOWMANAGER* pstWindowManager;

	// 윈도우 매니저를 반환
	pstWindowManager = kGetWindowManager();

	// 윈도우 매니저 태스크 루프
	if(isDirectBuffer == true)
	{
		while (1)
		{
			// 마우스 데이터를 처리
			bMouseDataResult = kProcessMouseData();

			// 키 데이터를 처리
			bKeyDataResult = kProcessKeyData();

			// 윈도우 매니저의 이벤트 큐에 수신된 데이터를 처리. 수신된 모든 이벤트를 처리함
			bEventQueueResult = FALSE;
			while (kProcessEventQueueData() == TRUE)
			{
				kProcessEventQueueData();
				bEventQueueResult = TRUE;
			}

			// 이벤트 큐에 이벤트를 처리했다면 윈도우 크기 변경 표식이 지워질 수 있으므로 다시 출력
			if ((bEventQueueResult == TRUE) &&
				(pstWindowManager->bWindowResizeMode == TRUE))
			{
				// 새로운 위치에 윈도우 크기 변경 표식을 표시
				kDrawResizeMarker(&(pstWindowManager->stResizingWindowArea), TRUE);
			}

			// 처리한 데이터가 하나도 없다면 Sleep()을 수행하여 프로세서를 양보함
			if ((bMouseDataResult == FALSE) &&
				(bKeyDataResult == FALSE) &&
				(bEventQueueResult == FALSE))
			{
				//20180628
				 ksleep( 0 );
			}
		}
	}
	else
	{
		// 마우스 데이터를 처리
		bMouseDataResult = kProcessMouseData();

		// 키 데이터를 처리
		bKeyDataResult = kProcessKeyData();

		// 윈도우 매니저의 이벤트 큐에 수신된 데이터를 처리. 수신된 모든 이벤트를 처리함
		bEventQueueResult = FALSE;
		while (kProcessEventQueueData() == TRUE)
		{
			kProcessEventQueueData();
			bEventQueueResult = TRUE;
		}

		// 이벤트 큐에 이벤트를 처리했다면 윈도우 크기 변경 표식이 지워질 수 있으므로 다시 출력
		if ((bEventQueueResult == TRUE) &&
			(pstWindowManager->bWindowResizeMode == TRUE))
		{
			// 새로운 위치에 윈도우 크기 변경 표식을 표시
			kDrawResizeMarker(&(pstWindowManager->stResizingWindowArea), TRUE);
		}

		// 처리한 데이터가 하나도 없다면 Sleep()을 수행하여 프로세서를 양보함
		if ((bMouseDataResult == FALSE) &&
			(bKeyDataResult == FALSE) &&
			(bEventQueueResult == FALSE))
		{
			//20180628
			 ksleep( 0 );
		}
	}
}

/**
 *  수신된 마우스 데이터를 처리
 */
bool kProcessMouseData( void )
{
    QWORD qwWindowIDUnderMouse;
    BYTE bButtonStatus;
    int iRelativeX, iRelativeY;
    int iMouseX, iMouseY;
	bool bAbsoluteCoordinate = false;
    int iPreviousMouseX, iPreviousMouseY;
    BYTE bChangedButton;
    RECT stWindowArea;
    EVENT stEvent;
    WINDOWMANAGER* pstWindowManager;
    char vcTempTitle[ WINDOW_TITLEMAXLENGTH ];
    int i;
    int iWidth, iHeight;
    
    // 윈도우 매니저를 반환
    pstWindowManager = kGetWindowManager();

    //--------------------------------------------------------------------------
    // 마우스 이벤트를 통합하는 부분
    //--------------------------------------------------------------------------
    for( i = 0 ; i < WINDOWMANAGER_DATAACCUMULATECOUNT ; i++ )
    {
        // 마우스 데이터가 수신되기를 기다림
        if( kGetMouseDataFromMouseQueue( &bButtonStatus, &iRelativeX, &iRelativeY, bAbsoluteCoordinate) ==
            FALSE )
        {
            // 처음으로 확인했는데 데이터가 없다면 종료
            if( i == 0 )
            {
                return FALSE;
            }
            // 처음이 아닌 경우는 이전 루프에서 마우스 이벤트를 수신했으므로 
            // 통합한 이벤트를 처리
            else
            {
                break;
            }
        }
        
        // 현재 마우스 커서 위치를 반환
        kGetCursorPosition( &iMouseX, &iMouseY );
        
        // 처음 마우스 이벤트가 수신된 것이면 현재 좌표를 이전 마우스의 위치로 저장해둠
        if( i == 0 )
        {
            // 움직이기 이전의 좌표를 저장
            iPreviousMouseX = iMouseX;
            iPreviousMouseY = iMouseY;
        }
        
		if (bAbsoluteCoordinate == true)
		{
			iMouseX = iRelativeX;
			iMouseY = iRelativeY;
		}
		else
		{
			// 마우스가 움직인 거리를 이전 커서 위치에 더해서 현재 좌표를 계산
			iMouseX += iRelativeX;
			iMouseY += iRelativeY;
		}        
        
        // 새로운 위치로 마우스 커서를 이동하고 다시 현재 커서의 위치를 반환
        // 마우스 커서가 화면을 벗어나지 않도록 처리된 커서 좌표를 사용하여 화면을 벗어난
        // 커서로 인해 발생하는 문제를 방지
        kMoveCursor( iMouseX, iMouseY );
        kGetCursorPosition( &iMouseX, &iMouseY );
        
        // 버튼 상태는 이전 버튼 상태와 현재 버튼 상태를 XOR하여 1로 설정됐는지를 확인
        bChangedButton = pstWindowManager->bPreviousButtonStatus ^ bButtonStatus;

        // 마우스가 움직였으나 버튼의 변화가 있다면 바로 이벤트 처리
        if( bChangedButton != 0 )
        {
            break;
        }
    }
    
    //--------------------------------------------------------------------------
    // 마우스 이벤트를 처리하는 부분
    //--------------------------------------------------------------------------
    // 현재 마우스 커서 아래에 있는 윈도우를 검색
    qwWindowIDUnderMouse = kFindWindowByPoint( iMouseX, iMouseY );
    
    //--------------------------------------------------------------------------
    // 버튼 상태가 변했는지 확인하고 버튼 상태에 따라 마우스 메시지와 윈도우 메시지를
    // 전송
    //--------------------------------------------------------------------------
    // 마우스 왼쪽 버튼에 변화가 생긴 경우 처리
    if( bChangedButton & MOUSE_LBUTTONDOWN )
    {
        // 왼쪽 버튼이 눌린 경우 처리
        if( bButtonStatus & MOUSE_LBUTTONDOWN )
        {
            // 마우스로 윈도우를 선택한 것이므로, 마우스 아래에 있는 윈도우가
            // 배경 윈도우가 아닌 경우 최상위로 올려줌
            if( qwWindowIDUnderMouse != pstWindowManager->qwBackgoundWindowID )
            {
                // 선택된 윈도우를 최상위로 만듦
                // 윈도우를 최상위로 만들면서 동시에 윈도우 선택과 선택 해제 이벤트도
                // 같이 전송
                kMoveWindowToTop( qwWindowIDUnderMouse );
            }
            
            //------------------------------------------------------------------
            // 왼쪽 버튼이 눌린 위치가 제목 표시줄의 위치이면 윈도우 이동인지 또는
            // 닫기 버튼 위치에서 눌렸는지 확인하여 처리
            //------------------------------------------------------------------
            if( kIsInTitleBar( qwWindowIDUnderMouse, iMouseX, iMouseY ) == TRUE )
            {
                // 닫기 버튼에서 눌렸으면 윈도우에 닫기 전송
                if( kIsInCloseButton( qwWindowIDUnderMouse, iMouseX, iMouseY ) 
                        == TRUE )
                {
                    // 윈도우 닫기 이벤트를 전송
                    kSetWindowEvent( qwWindowIDUnderMouse, EVENT_WINDOW_CLOSE,
                            &stEvent );
                    kSendEventToWindow( qwWindowIDUnderMouse, &stEvent );
                }
                // 윈도우 크기 변경 버튼에서 눌렸으면 크기 변경 모드로 변경
                else if( kIsInResizeButton( qwWindowIDUnderMouse, iMouseX, iMouseY ) 
                           == TRUE )
                {
                    // 윈도우 크기 변경 모드 설정
                    pstWindowManager->bWindowResizeMode = TRUE;
                    
                    // 현재 윈도우를 크기 변경 윈도우로 설정
                    pstWindowManager->qwResizingWindowID = qwWindowIDUnderMouse;
                    
                    // 현재 윈도우의 크기를 저장
                    kGetWindowArea( qwWindowIDUnderMouse, 
                            &( pstWindowManager->stResizingWindowArea ) );
                    
                    // 윈도우 크기 변경 표식을 표시
                    kDrawResizeMarker( &( pstWindowManager->stResizingWindowArea ), 
                            TRUE );
                }
                // 닫기 버튼이 아니면 윈도우 이동 모드로 변경
                else
                {
                    // 윈도우 이동 모드 설정
                    pstWindowManager->bWindowMoveMode = TRUE;
                    
                    // 현재 윈도우를 이동하는 윈도우로 설정
                    pstWindowManager->qwMovingWindowID = qwWindowIDUnderMouse;
                }
            }
            // 윈도우 내부에서 눌린 것이면 왼쪽 버튼이 눌러졌음을 전송
            else
            {
                // 왼쪽 버튼 눌림 이벤트를 전송
                kSetMouseEvent( qwWindowIDUnderMouse, EVENT_MOUSE_LBUTTONDOWN,
                        iMouseX, iMouseY, bButtonStatus, &stEvent );
                kSendEventToWindow( qwWindowIDUnderMouse, &stEvent );
            }
        }
        // 왼쪽 버튼이 떨어진 경우 처리
        else
        {
            // 윈도우가 이동 중이었으면 모드만 해제
            if( pstWindowManager->bWindowMoveMode == TRUE )
            {
                // 이동 중이라는 플래그를 해제
                pstWindowManager->bWindowMoveMode = FALSE;
                pstWindowManager->qwMovingWindowID = WINDOW_INVALIDID;
            }
            // 윈도우가 크기 변경 중이었으면 결정된 크기로 윈도우 크기 변경
            else if( pstWindowManager->bWindowResizeMode == TRUE )
            {
                // 윈도우 매니저 자료구조에 저장된 영역을 이용하여 윈도우의 크기를 변경
                iWidth = kGetRectangleWidth( &( pstWindowManager->stResizingWindowArea ) );
                iHeight = kGetRectangleHeight( &( pstWindowManager->stResizingWindowArea ) );
                kResizeWindow( pstWindowManager->qwResizingWindowID, 
                        pstWindowManager->stResizingWindowArea.iX1, 
                        pstWindowManager->stResizingWindowArea.iY1,
                        iWidth, iHeight );
                
                // 윈도우 크기 변경 표식 삭제
                kDrawResizeMarker( &( pstWindowManager->stResizingWindowArea ), FALSE );

                // 윈도우로 크기 변경 이벤트 전송
                kSetWindowEvent( pstWindowManager->qwResizingWindowID, EVENT_WINDOW_RESIZE, 
                        &stEvent );
                kSendEventToWindow( pstWindowManager->qwResizingWindowID, &stEvent );
                
                // 크기 변경 중이라는 플래그를 해제
                pstWindowManager->bWindowResizeMode = FALSE;
                pstWindowManager->qwResizingWindowID = WINDOW_INVALIDID;
            }
            // 윈도우가 이동 중이 아니었으면 윈도우로 왼쪽 버튼 떨어짐 이벤트를 전송
            else
            {
                // 왼쪽 버튼 떨어짐 이벤트를 전송
                kSetMouseEvent( qwWindowIDUnderMouse, EVENT_MOUSE_LBUTTONUP,
                        iMouseX, iMouseY, bButtonStatus, &stEvent );
                kSendEventToWindow( qwWindowIDUnderMouse, &stEvent );
            }
        }
    }            
    // 마우스 오른쪽 버튼에 변화가 생긴 경우 처리
    else if( bChangedButton & MOUSE_RBUTTONDOWN )
    {
        // 오른쪽 버튼이 눌린 경우 처리
        if( bButtonStatus & MOUSE_RBUTTONDOWN )
        {
            // 오른쪽 버튼 눌림 이벤트를 전송
            kSetMouseEvent( qwWindowIDUnderMouse, EVENT_MOUSE_RBUTTONDOWN,
                    iMouseX, iMouseY, bButtonStatus, &stEvent );
            kSendEventToWindow( qwWindowIDUnderMouse, &stEvent );
        }
        else
        {
            // 오른쪽 버튼 떨어짐 이벤트를 전송
            kSetMouseEvent( qwWindowIDUnderMouse, EVENT_MOUSE_RBUTTONUP,
                    iMouseX, iMouseY, bButtonStatus, &stEvent );
            kSendEventToWindow( qwWindowIDUnderMouse, &stEvent );
        }
    }
    // 마우스 가운데 버튼에 변화가 생긴 경우 처리
    else if( bChangedButton & MOUSE_MBUTTONDOWN )
    {
        // 가운데 버튼이 눌린 경우 처리
        if( bButtonStatus & MOUSE_MBUTTONDOWN )
        {
            // 가운데 버튼 눌림 이벤트를 전송
            kSetMouseEvent( qwWindowIDUnderMouse, EVENT_MOUSE_MBUTTONDOWN,
                    iMouseX, iMouseY, bButtonStatus, &stEvent );
            kSendEventToWindow( qwWindowIDUnderMouse, &stEvent );
        }
        else
        {
            // 가운데 버튼 떨어짐 이벤트를 전송
            kSetMouseEvent( qwWindowIDUnderMouse, EVENT_MOUSE_MBUTTONUP,
                    iMouseX, iMouseY, bButtonStatus, &stEvent );
            kSendEventToWindow( qwWindowIDUnderMouse, &stEvent );
        }
    }        
    // 마우스 버튼이 변경되지 않았으면 마우스 이동 처리만 수행
    else
    {
        // 마우스 이동 이벤트를 전송
        kSetMouseEvent( qwWindowIDUnderMouse, EVENT_MOUSE_MOVE, iMouseX, iMouseY, 
                    bButtonStatus, &stEvent );
        kSendEventToWindow( qwWindowIDUnderMouse, &stEvent );
    }

    //--------------------------------------------------------------------------
    //  윈도우 이동과 크기 변경 처리
    //--------------------------------------------------------------------------    
    // 윈도우가 이동 중이었으면 윈도우 이동 처리
    if( pstWindowManager->bWindowMoveMode == TRUE )
    {
        // 윈도우의 위치를 얻음
        if( kGetWindowArea( pstWindowManager->qwMovingWindowID, &stWindowArea )
                == TRUE )
        {
            // 윈도우의 현재 위치를 얻어서 마우스가 이동한 만큼 옮겨줌
            // 윈도우 이동 이벤트는 함수 내부에서 전달
            kMoveWindow( pstWindowManager->qwMovingWindowID, 
                         stWindowArea.iX1 + iMouseX - iPreviousMouseX, 
                         stWindowArea.iY1 + iMouseY - iPreviousMouseY ); 
        }
        // 윈도우의 위치를 얻을 수 없으면 윈도우가 존재하지 않는 것이므로
        // 윈도우 이동 모드 해제
        else
        {
            // 이동 중이라는 플래그를 해제
            pstWindowManager->bWindowMoveMode = FALSE;
            pstWindowManager->qwMovingWindowID = WINDOW_INVALIDID;
        }
    }
    // 윈도우가 크기 변경 중이었으면 윈도우 크기 변경 처리
    else if( pstWindowManager->bWindowResizeMode == TRUE )
    {
        // 이전 위치의 윈도우 크기 변경 표식을 삭제
        kDrawResizeMarker( &( pstWindowManager->stResizingWindowArea ), FALSE );
        
        // 마우스 이동 거리를 이용하여 새로운 윈도우 크기를 결정
        pstWindowManager->stResizingWindowArea.iX2 += iMouseX - iPreviousMouseX;
        pstWindowManager->stResizingWindowArea.iY1 += iMouseY - iPreviousMouseY;

        // 윈도우의 크기가 최소 값 이하이면 최솟값으로 다시 설정
        if( ( pstWindowManager->stResizingWindowArea.iX2 < pstWindowManager->stResizingWindowArea.iX1 ) ||
            ( kGetRectangleWidth( &( pstWindowManager->stResizingWindowArea ) ) < WINDOW_WIDTH_MIN ) )
        {
            pstWindowManager->stResizingWindowArea.iX2 = 
                pstWindowManager->stResizingWindowArea.iX1 + WINDOW_WIDTH_MIN - 1;
        }        
        if( ( pstWindowManager->stResizingWindowArea.iY2 < pstWindowManager->stResizingWindowArea.iY1 ) ||
            ( kGetRectangleHeight( &( pstWindowManager->stResizingWindowArea ) ) < WINDOW_HEIGHT_MIN ) )
        {
            pstWindowManager->stResizingWindowArea.iY1 = 
                pstWindowManager->stResizingWindowArea.iY2 - WINDOW_HEIGHT_MIN - 1;
        }
        
        // 새로운 위치에 윈도우 크기 변경 표식을 출력
        kDrawResizeMarker( &( pstWindowManager->stResizingWindowArea ), TRUE );
    }
    
    // 다음 처리에 사용하려고 현재 버튼 상태를 저장함
    pstWindowManager->bPreviousButtonStatus = bButtonStatus;
    return TRUE;
}


/**
 *  수신된 키 데이터를 처리
 */
bool kProcessKeyData( void )
{
    KEYDATA stKeyData;
    EVENT stEvent;
    QWORD qwAcitveWindowID;
    
    // 키보드 데이터가 수신되기를 기다림
    if( kGetKeyFromKeyQueue( &stKeyData ) == FALSE )
    {
        return FALSE;
    }

    // 최상위 윈도우, 즉 선택된 윈도우로 메시지를 전송
    qwAcitveWindowID = kGetTopWindowID();
    kSetKeyEvent( qwAcitveWindowID, &stKeyData, &stEvent );
    return kSendEventToWindow( qwAcitveWindowID, &stEvent );
}    

/**
 *  이벤트 큐에 수신된 이벤트 처리
 */
bool kProcessEventQueueData(void)
{
	EVENT vstEvent[WINDOWMANAGER_DATAACCUMULATECOUNT];
	int iEventCount;
	WINDOWEVENT* pstWindowEvent;
	WINDOWEVENT* pstNextWindowEvent;
	QWORD qwWindowID;
	RECT stArea;
	RECT stOverlappedArea;
	int i;
	int j;

	//--------------------------------------------------------------------------
	// 윈도우 매니저 태스크의 이벤트 큐에 수신된 이벤트를 통합하는 부분
	//--------------------------------------------------------------------------
	for (i = 0; i < WINDOWMANAGER_DATAACCUMULATECOUNT; i++)
	{
		// 이벤트가 수신되기를 기다림
		if (kReceiveEventFromWindowManagerQueue(&(vstEvent[i])) == FALSE)
		{
			// 처음부터 이벤트가 수신되지 않았으면 종료
			if (i == 0)
			{
				return FALSE;
			}
			else
			{
				break;
			}
		}

		pstWindowEvent = &(vstEvent[i].stWindowEvent);
		// 윈도우 ID로 업데이트하는 이벤트가 수신되면 윈도우 영역을 이벤트 데이터에 삽입
		if (vstEvent[i].qwType == EVENT_WINDOWMANAGER_UPDATESCREENBYID)
		{
			// 윈도우의 크기를 이벤트 자료구조에 삽입
			if (kGetWindowArea(pstWindowEvent->qwWindowID, &stArea) == FALSE)
			{
				kSetRectangleData(0, 0, 0, 0, &(pstWindowEvent->stArea));
			}
			else
			{
				kSetRectangleData(0, 0, kGetRectangleWidth(&stArea) - 1,
					kGetRectangleHeight(&stArea) - 1, &(pstWindowEvent->stArea));
			}
		}
	}

	// 저장된 이벤트를 검사하면서 합칠 수 있는 이벤트는 하나로 만듦
	iEventCount = i;

	for (j = 0; j < iEventCount; j++)
	{
		// 수신된 이벤트 중에 이벤트 중에서 이번에 처리할 것과 같은 윈도우에서
		// 발생하는 윈도우 이벤트를 검색
		pstWindowEvent = &(vstEvent[j].stWindowEvent);
		if ((vstEvent[j].qwType != EVENT_WINDOWMANAGER_UPDATESCREENBYID) &&
			(vstEvent[j].qwType != EVENT_WINDOWMANAGER_UPDATESCREENBYWINDOWAREA) &&
			(vstEvent[j].qwType != EVENT_WINDOWMANAGER_UPDATESCREENBYSCREENAREA))
		{
			continue;
		}

		// 수신한 이벤트의 끝까지 루프를 수행하면서 수신된 이벤트를 검사
		for (i = j + 1; i < iEventCount; i++)
		{
			pstNextWindowEvent = &(vstEvent[i].stWindowEvent);
			// 화면 업데이트가 아니거나 윈도우 ID가 일치하지 않으면 제외
			if (((vstEvent[i].qwType != EVENT_WINDOWMANAGER_UPDATESCREENBYID) &&
				(vstEvent[i].qwType != EVENT_WINDOWMANAGER_UPDATESCREENBYWINDOWAREA) &&
				(vstEvent[i].qwType != EVENT_WINDOWMANAGER_UPDATESCREENBYSCREENAREA)) ||
				(pstWindowEvent->qwWindowID != pstNextWindowEvent->qwWindowID))
			{
				continue;
			}

			// 겹치는 영역을 계산하여 겹치지 않으면 제외
			if (kGetOverlappedRectangle(&(pstWindowEvent->stArea),
				&(pstNextWindowEvent->stArea), &stOverlappedArea) == FALSE)
			{
				continue;
			}

			// 두 영역이 일치하거나 어느 한쪽이 포함되면 이벤트를 통합
			if (memcmp(&(pstWindowEvent->stArea), &stOverlappedArea,
				sizeof(RECT)) == 0)
			{
				// 현재 이벤트의 윈도우의 영역이 겹치는 영역과 일치한다면 
				// 다음 이벤트의 윈도우 영역이 현재 윈도우 영역과 같거나 포함함
				// 따라서 현재 이벤트에 다음 이벤트의 윈도우 영역을 복사하고
				// 다음 이벤트는 삭제
				memcpy(&(pstWindowEvent->stArea), &(pstNextWindowEvent->stArea),
					sizeof(RECT));
				vstEvent[i].qwType = EVENT_UNKNOWN;
			}
			else if (memcmp(&(pstNextWindowEvent->stArea), &stOverlappedArea,
				sizeof(RECT)) == 0)
			{
				// 다음 이벤트의 윈도우의 영역이 겹치는 영역과 일치한다면 
				// 현재 이벤트의 윈도우 영역이 다음 윈도우 영역과 같거나 포함함
				// 따라서 윈도우 영역을 복사하지 않고 다음 이벤트를 삭제
				vstEvent[i].qwType = EVENT_UNKNOWN;
			}
		}
	}

	// 통합된 이벤트를 모두 처리
	for (i = 0; i < iEventCount; i++)
	{
		pstWindowEvent = &(vstEvent[i].stWindowEvent);

		// 타입 별로 처리
		switch (vstEvent[i].qwType)
		{
			// 현재 윈도우가 있는 영역을 화면에 업데이트
		case EVENT_WINDOWMANAGER_UPDATESCREENBYID:
			// 윈도우의 내부 영역을 화면에 업데이트
		case EVENT_WINDOWMANAGER_UPDATESCREENBYWINDOWAREA:
			// 윈도우를 기준으로 한 좌표를 화면 좌표로 변환하여 업데이트 처리
			if (kConvertRectClientToScreen(pstWindowEvent->qwWindowID,
				&(pstWindowEvent->stArea), &stArea) == TRUE)
			{
				// 윈도우 영역은 위에서 했으므로 그대로 화면 업데이트 함수를 호출
				kRedrawWindowByArea(&stArea, pstWindowEvent->qwWindowID);
			}
			break;

			// 화면 좌표로 전달된 영역을 화면에 업데이트
		case EVENT_WINDOWMANAGER_UPDATESCREENBYSCREENAREA:
			kRedrawWindowByArea(&(pstWindowEvent->stArea), WINDOW_INVALIDID);
			break;

		default:
			break;
		}
	}

	return TRUE;
}

/**
 *  비디오 메모리에 윈도우 크기 변경 표식을 출력하거나 출력된 표식을 삭제
 */
void kDrawResizeMarker( const RECT* pstArea, bool bShowMarker )
{
    RECT stMarkerArea;
    WINDOWMANAGER* pstWindowManager;
    
    // 윈도우 매니저를 반환
    pstWindowManager = kGetWindowManager();
    
    // 윈도우 크기 변경 표식을 출력하는 경우
    if( bShowMarker == TRUE )
    {
        // 왼쪽 위 표식
        kSetRectangleData( pstArea->iX1, pstArea->iY1, 
            pstArea->iX1 + WINDOWMANAGER_RESIZEMARKERSIZE, 
            pstArea->iY1 + WINDOWMANAGER_RESIZEMARKERSIZE, &stMarkerArea );
        kInternalDrawRect( &( pstWindowManager->stScreenArea ), 
                pstWindowManager->pstVideoMemory, stMarkerArea.iX1, stMarkerArea.iY1,
                stMarkerArea.iX2, stMarkerArea.iY1 + WINDOWMANAGER_THICK_RESIZEMARKER,
                WINDOWMANAGER_COLOR_RESIZEMARKER, TRUE );
        kInternalDrawRect( &( pstWindowManager->stScreenArea ), 
                pstWindowManager->pstVideoMemory, stMarkerArea.iX1, stMarkerArea.iY1,
                stMarkerArea.iX1 + WINDOWMANAGER_THICK_RESIZEMARKER, stMarkerArea.iY2, 
                WINDOWMANAGER_COLOR_RESIZEMARKER, TRUE );
        
        // 오른쪽 위 표식
        kSetRectangleData( pstArea->iX2 - WINDOWMANAGER_RESIZEMARKERSIZE, 
            pstArea->iY1, pstArea->iX2, pstArea->iY1 + WINDOWMANAGER_RESIZEMARKERSIZE,
            &stMarkerArea );
        kInternalDrawRect( &( pstWindowManager->stScreenArea ), 
                pstWindowManager->pstVideoMemory, stMarkerArea.iX1, stMarkerArea.iY1,
                stMarkerArea.iX2, stMarkerArea.iY1 + WINDOWMANAGER_THICK_RESIZEMARKER, 
                WINDOWMANAGER_COLOR_RESIZEMARKER, TRUE );
        kInternalDrawRect( &( pstWindowManager->stScreenArea ), 
                pstWindowManager->pstVideoMemory, stMarkerArea.iX2 - 
                WINDOWMANAGER_THICK_RESIZEMARKER, stMarkerArea.iY1,
                stMarkerArea.iX2, stMarkerArea.iY2, WINDOWMANAGER_COLOR_RESIZEMARKER,
                TRUE );

        // 왼쪽 아래 표식
        kSetRectangleData( pstArea->iX1, pstArea->iY2 - WINDOWMANAGER_RESIZEMARKERSIZE, 
            pstArea->iX1 + WINDOWMANAGER_RESIZEMARKERSIZE, pstArea->iY2, &stMarkerArea );
        kInternalDrawRect( &( pstWindowManager->stScreenArea ), 
                pstWindowManager->pstVideoMemory, stMarkerArea.iX1, stMarkerArea.iY2 - 
                WINDOWMANAGER_THICK_RESIZEMARKER, stMarkerArea.iX2, stMarkerArea.iY2, 
                WINDOWMANAGER_COLOR_RESIZEMARKER, TRUE );
        kInternalDrawRect( &( pstWindowManager->stScreenArea ), 
                pstWindowManager->pstVideoMemory, stMarkerArea.iX1, stMarkerArea.iY1,
                stMarkerArea.iX1 + WINDOWMANAGER_THICK_RESIZEMARKER, stMarkerArea.iY2, 
                WINDOWMANAGER_COLOR_RESIZEMARKER, TRUE );

        // 오른쪽 아래 표식
        kSetRectangleData( pstArea->iX2 - WINDOWMANAGER_RESIZEMARKERSIZE, 
            pstArea->iY2 - WINDOWMANAGER_RESIZEMARKERSIZE, pstArea->iX2, pstArea->iY2,
            &stMarkerArea );
        kInternalDrawRect( &( pstWindowManager->stScreenArea ), 
                pstWindowManager->pstVideoMemory, stMarkerArea.iX1, stMarkerArea.iY2 - 
                WINDOWMANAGER_THICK_RESIZEMARKER, stMarkerArea.iX2, stMarkerArea.iY2, 
                WINDOWMANAGER_COLOR_RESIZEMARKER, TRUE );
        kInternalDrawRect( &( pstWindowManager->stScreenArea ), 
                pstWindowManager->pstVideoMemory, stMarkerArea.iX2 - 
                WINDOWMANAGER_THICK_RESIZEMARKER, stMarkerArea.iY1, stMarkerArea.iX2, 
                stMarkerArea.iY2, WINDOWMANAGER_COLOR_RESIZEMARKER, TRUE );
    }
    // 윈도우 크기 변경 표식을 삭제하는 경우
    else
    {
        // 크기 변경 표식은 영역의 네 모서리에 있으므로 모서리만 화면 업데이트
        // 왼쪽 위 표식
        kSetRectangleData( pstArea->iX1, pstArea->iY1, 
            pstArea->iX1 + WINDOWMANAGER_RESIZEMARKERSIZE, 
            pstArea->iY1 + WINDOWMANAGER_RESIZEMARKERSIZE, &stMarkerArea );
        kRedrawWindowByArea( &stMarkerArea, WINDOW_INVALIDID );
        
        // 오른쪽 위 표식
        kSetRectangleData( pstArea->iX2 - WINDOWMANAGER_RESIZEMARKERSIZE, 
            pstArea->iY1, pstArea->iX2, pstArea->iY1 + WINDOWMANAGER_RESIZEMARKERSIZE,
            &stMarkerArea );
        kRedrawWindowByArea( &stMarkerArea, WINDOW_INVALIDID );

        // 왼쪽 아래 표식
        kSetRectangleData( pstArea->iX1, pstArea->iY2 - WINDOWMANAGER_RESIZEMARKERSIZE, 
            pstArea->iX1 + WINDOWMANAGER_RESIZEMARKERSIZE, pstArea->iY2, &stMarkerArea );
        kRedrawWindowByArea( &stMarkerArea, WINDOW_INVALIDID );

        // 오른쪽 아래 표식
        kSetRectangleData( pstArea->iX2 - WINDOWMANAGER_RESIZEMARKERSIZE, 
            pstArea->iY2 - WINDOWMANAGER_RESIZEMARKERSIZE, pstArea->iX2, pstArea->iY2,
            &stMarkerArea );
        kRedrawWindowByArea( &stMarkerArea, WINDOW_INVALIDID );
    }
}


