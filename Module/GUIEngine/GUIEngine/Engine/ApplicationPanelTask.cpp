/**
 *  file    ApplicationPanelTask.c
 *  date    2009/11/03
 *  author  kkamagui 
 *          Copyright(c)2008 All rights reserved by kkamagui 
 *  brief   애플리케이션 패널에 관련된 소스 파일
 */

#include "ApplicationPanelTask.h"
#include "string.h"
//#include "RTC.h"
//#include "Task.h"
#include "GUITask.h"
#include "SkyInterface.h"

extern SKY_PROCESS_INTERFACE g_processInterface;

// 애플리케이션 테이블
APPLICATIONENTRY gs_vstApplicationTable[] =
{
        { "Base GUI Task", kBaseGUITask },
        { "Hello World GUI Task",  kHelloWorldGUITask },
        //{ "System Monitor Task", kSystemMonitorTask },
       // { "Console Shell for GUI", kGUIConsoleShellTask },
        { "Image Viewer Task", kImageViewerTask },
};

// 애플리케이션 패널에서 사용하는 자료구조
APPLICATIONPANELDATA gs_stApplicationPanelData;

/**
 *  애플리케이션 패널 태스크
 */
DWORD WINAPI kApplicationPanelGUITask(LPVOID parameter)
{
    EVENT stReceivedEvent;
	bool bApplicationPanelEventResult;
	bool bApplicationListEventResult;    

    //--------------------------------------------------------------------------
    // 윈도우를 생성
    //--------------------------------------------------------------------------
    // 애플리케이션 패널 윈도우와 응용프로그램 리스트 윈도우를 생성
    if( ( kCreateApplicationPanelWindow() == FALSE ) ||
        ( kCreateApplicationListWindow() == FALSE ) )
    {
        return 0;
    }

    //--------------------------------------------------------------------------
    // GUI 태스크의 이벤트 처리 루프
    //--------------------------------------------------------------------------
    while( 1 )
    {
        // 윈도우 이벤트를 처리
        bApplicationPanelEventResult = kProcessApplicationPanelWindowEvent();
        bApplicationListEventResult = kProcessApplicationListWindowEvent();
        
        // 처리한 이벤트가 없으면 프로세서를 반환
        if( ( bApplicationPanelEventResult == FALSE ) &&
            ( bApplicationListEventResult == FALSE ) )
        {
			int j = 1;
			//20180628
            ksleep( 0 );
        }            
    }

	return 1;
}

/**
 *  애플리케이션 패널 윈도우를 생성
 */
bool kCreateApplicationPanelWindow( void )
{
    WINDOWMANAGER* pstWindowManager;
    QWORD qwWindowID;
    
    // 윈도우 매니저를 반환
    pstWindowManager = kGetWindowManager();
    
    // 화면 위쪽에 애플리케이션 패널 윈도우를 생성, 가로로 가득 차도록 생성
    qwWindowID = kCreateWindow( 0, 0, pstWindowManager->stScreenArea.iX2 + 1,
        APPLICATIONPANEL_HEIGHT, NULL, APPLICATIONPANEL_TITLE );
    // 윈도우를 생성하지 못했으면 실패
    if( qwWindowID == WINDOW_INVALIDID )
    {
        return FALSE;
    }
    
    // 애플리케이션 패널 윈도우의 테두리와 내부를 표시
    kDrawRect( qwWindowID, 0, 0, pstWindowManager->stScreenArea.iX2, 
            APPLICATIONPANEL_HEIGHT - 1, APPLICATIONPANEL_COLOR_OUTERLINE, FALSE );   
    kDrawRect( qwWindowID, 1, 1, pstWindowManager->stScreenArea.iX2 - 1, 
            APPLICATIONPANEL_HEIGHT - 2, APPLICATIONPANEL_COLOR_MIDDLELINE, FALSE );   
    kDrawRect( qwWindowID, 2, 2, pstWindowManager->stScreenArea.iX2 - 2, 
            APPLICATIONPANEL_HEIGHT - 3, APPLICATIONPANEL_COLOR_INNERLINE, FALSE );  
    kDrawRect( qwWindowID, 3, 3, pstWindowManager->stScreenArea.iX2 - 3, 
            APPLICATIONPANEL_HEIGHT - 4, APPLICATIONPANEL_COLOR_BACKGROUND, TRUE );
    
    // 애플리케이션 패널의 왼쪽에 GUI 태스크의 리스트를 보여주는 버튼을 표시
    kSetRectangleData( 5, 5, 120, 25, &( gs_stApplicationPanelData.stButtonArea ) );
    kDrawButton( qwWindowID, &( gs_stApplicationPanelData.stButtonArea ), 
                 APPLICATIONPANEL_COLOR_ACTIVE, "Application", RGB( 255, 255, 255 ) );
    
    // 애플리케이션 패널 윈도우의 오른쪽에 시계를 표시
    kDrawDigitalClock( qwWindowID );
    
    // 애플리케이션 패널을 화면에 표시
    kShowWindow( qwWindowID, TRUE );    
    
    // 애플리케이션 패널 자료구조에 윈도우 ID 저장
    gs_stApplicationPanelData.qwApplicationPanelID = qwWindowID;
    
    return TRUE;
}

/**
 *  애플리케이션 패널에 시계를 표시
 */
static void kDrawDigitalClock( QWORD qwWindowID )
{
    RECT stWindowArea;
    RECT stUpdateArea;
    static BYTE s_bPreviousHour, s_bPreviousMinute, s_bPreviousSecond;
    BYTE bHour, bMinute, bSecond;
    char vcBuffer[ 10 ] = "00:00 AM";

    // 현재 시간을 RTC에서 반환
	//20180628
    //kReadRTCTime( &bHour, &bMinute, &bSecond );
    
    // 이전 시간과 변화가 없으면 시계를 표시할 필요 없음
    if( ( s_bPreviousHour == bHour ) && ( s_bPreviousMinute == bMinute ) &&
        ( s_bPreviousSecond == bSecond ) )
    {
        return ;
    }
    
    // 다음 비교를 위해 시, 분, 초를 업데이트
    s_bPreviousHour = bHour;
    s_bPreviousMinute = bMinute;
    s_bPreviousSecond = bSecond;

    // 시간이 12시가 넘으면 PM으로 변경
    if( bHour >= 12 )
    {
        if( bHour > 12 )
        {
            bHour -= 12;
        }
        vcBuffer[ 6 ] = 'P';
    }
    
    // 시간 설정
    vcBuffer[ 0 ] = '0' + bHour / 10;
    vcBuffer[ 1 ] = '0' + bHour % 10;
    // 분 설정
    vcBuffer[ 3 ] = '0' + bMinute / 10;
    vcBuffer[ 4 ] = '0' + bMinute % 10;
    
    // 초에 따라서 가운데 :를 깜빡임
    if( ( bSecond % 2 ) == 1 )
    {
        vcBuffer[ 2 ] = ' ';
    }
    else
    {
        vcBuffer[ 2 ] = ':';
    }
    
    // 애플리케이션 패널 윈도우의 위치를 반환
    kGetWindowArea( qwWindowID, &stWindowArea );
    
    // 시계 영역의 테두리를 표시
    kSetRectangleData( stWindowArea.iX2 - APPLICATIONPANEL_CLOCKWIDTH - 13, 5,
                       stWindowArea.iX2 - 5, 25, &stUpdateArea );    
    kDrawRect( qwWindowID, stUpdateArea.iX1, stUpdateArea.iY1, 
            stUpdateArea.iX2, stUpdateArea.iY2, APPLICATIONPANEL_COLOR_INNERLINE, 
            FALSE );
    
    // 시계를 표시
    kDrawText( qwWindowID, stUpdateArea.iX1 + 4, stUpdateArea.iY1 + 3, 
            RGB( 255, 255, 255 ), APPLICATIONPANEL_COLOR_BACKGROUND, vcBuffer, 
            strlen( vcBuffer ) );
    
    // 시계가 그려진 영역만 화면에 업데이트
    kUpdateScreenByWindowArea( qwWindowID, &stUpdateArea );
}

/**
 *  애플리케이션 패널에 수신된 이벤트를 처리
 */
static bool kProcessApplicationPanelWindowEvent( void )
{
    EVENT stReceivedEvent;
    MOUSEEVENT* pstMouseEvent;
    bool bProcessResult;
    QWORD qwApplicationPanelID;
    QWORD qwApplicationListID;

    // 윈도우 ID 저장
    qwApplicationPanelID = gs_stApplicationPanelData.qwApplicationPanelID;
    qwApplicationListID = gs_stApplicationPanelData.qwApplicationListID;
    bProcessResult = FALSE;
    
    // 이벤트를 처리하는 루프
    while( 1 )
    {
        // 애플리케이션 패널 윈도우의 오른쪽에 시계를 표시
        kDrawDigitalClock( gs_stApplicationPanelData.qwApplicationPanelID );
        
        // 이벤트 큐에서 이벤트를 수신
        if( kReceiveEventFromWindowQueue( qwApplicationPanelID, &stReceivedEvent ) 
                == FALSE )
        {
            break;
        }

        bProcessResult = TRUE;
        
        // 수신된 이벤트를 타입에 따라 나누어 처리
        switch( stReceivedEvent.qwType )
        {
            // 마우스 왼쪽 버튼 처리
        case EVENT_MOUSE_LBUTTONDOWN:
            pstMouseEvent = &( stReceivedEvent.stMouseEvent );
            // 마우스 왼쪽 버튼이 애플리케이션 패널의 버튼 내부에서 눌렸으면
            // 애플리케이션 리스트 윈도우를 표시
            if( kIsInRectangle( &( gs_stApplicationPanelData.stButtonArea ), 
                    pstMouseEvent->stPoint.iX, pstMouseEvent->stPoint.iY ) == FALSE )
            {
                break;
            }
            
            // 버튼이 떨어진 상태에서 눌리는 경우
            if( gs_stApplicationPanelData.bApplicationWindowVisible == FALSE )
            {
                // 버튼을 눌린 상태로 표시
                kDrawButton( qwApplicationPanelID, &( gs_stApplicationPanelData.stButtonArea ), 
                             APPLICATIONPANEL_COLOR_BACKGROUND, "Application", 
                             RGB( 255, 255, 255 ) );
                // 버튼이 있는 영역만 화면 업데이트
                kUpdateScreenByWindowArea( qwApplicationPanelID, 
                        &( gs_stApplicationPanelData.stButtonArea ) );
        
                // 애플리케이션 리스트 윈도우에 아무것도 선택되지 않은 것으로 초기화하고 
                // 윈도우를 화면에 최상위로 표시
                if( gs_stApplicationPanelData.iPreviousMouseOverIndex != -1 )
                {
                    kDrawApplicationListItem( 
                        gs_stApplicationPanelData.iPreviousMouseOverIndex, FALSE );
                    gs_stApplicationPanelData.iPreviousMouseOverIndex = -1;
                }
                kMoveWindowToTop( gs_stApplicationPanelData.qwApplicationListID );
                kShowWindow( gs_stApplicationPanelData.qwApplicationListID, TRUE );
                // 플래그는 화면에 표시된 것으로 설정
                gs_stApplicationPanelData.bApplicationWindowVisible = TRUE;
            }
            // 버튼이 눌린 상태에서 떨어진 경우
            else
            {
                // 애플리케이션 패널의 버튼을 떨어진 상태로 표시
                kDrawButton( qwApplicationPanelID, 
                         &( gs_stApplicationPanelData.stButtonArea ), 
                         APPLICATIONPANEL_COLOR_ACTIVE, "Application", 
                         RGB( 255, 255, 255 ) );
                // 버튼이 있는 영역만 화면 업데이트
                kUpdateScreenByWindowArea( qwApplicationPanelID, 
                         &( gs_stApplicationPanelData.stButtonArea ) );

                // 애플리케이션 리스트 윈도우를 숨김
                kShowWindow( qwApplicationListID, FALSE );
                // 플래그는 화면에 표시되지 않은 것으로 설정
                gs_stApplicationPanelData.bApplicationWindowVisible = FALSE;            
            }
            break;
            
            // 그 외 이벤트 처리
        default:
            break;
        }
    }
    
    return bProcessResult;
}

/**
 *  애플리케이션 리스트 윈도우를 생성
 */
bool kCreateApplicationListWindow( void )
{
    int i;
    int iCount;
    int iMaxNameLength;
    int iNameLength;
    QWORD qwWindowID;
    int iX;
    int iY;
    int iWindowWidth;
    
    // 애플리케이션 테이블에 정의된 이름 중에서 가장 긴 것을 검색
    iMaxNameLength = 0;
    iCount = sizeof( gs_vstApplicationTable ) / sizeof( APPLICATIONENTRY );
    for( i = 0 ; i < iCount ; i++ )
    {
        iNameLength = strlen( gs_vstApplicationTable[ i ].pcApplicationName );
        if( iMaxNameLength < iNameLength )
        {
            iMaxNameLength = iNameLength;
        }
    }
    
    // 윈도우의 너비 계산, 20은 좌우 10픽셀의 여유공간
    iWindowWidth = iMaxNameLength * FONT_ENGLISHWIDTH + 20;
    
    // 윈도우의 위치는 애플리케이션 패널의 버튼 아래로 설정
    iX = gs_stApplicationPanelData.stButtonArea.iX1;
    iY = gs_stApplicationPanelData.stButtonArea.iY2 + 5;
    
    // 아이템의 개수와 최대 길이로 애플리케이션 리스트 윈도우를 생성
    // 애플리케이션 윈도우는 윈도우 제목 표시줄이 필요 없으므로 속성은 NULL로 전달
    qwWindowID = kCreateWindow( iX, iY, iWindowWidth, 
        iCount * APPLICATIONPANEL_LISTITEMHEIGHT + 1, NULL, 
        APPLICATIONPANEL_LISTTITLE );
    // 윈도우를 생성하지 못했으면 실패
    if( qwWindowID == WINDOW_INVALIDID )
    {
        return FALSE;
    }
    
    // 애플리케이션 패널 자료구조에 윈도우의 너비를 저장
    gs_stApplicationPanelData.iApplicationListWidth = iWindowWidth;
    
    // 시작할 때 애플리케이션 리스트는 숨겨놓음
    gs_stApplicationPanelData.bApplicationWindowVisible = FALSE;

    // 애플리케이션 패널 자료구조에 윈도우 ID를 저장하고 이전에 마우스가 위치한 아이템은 
    // 없는 것으로 설정
    gs_stApplicationPanelData.qwApplicationListID = qwWindowID;
    gs_stApplicationPanelData.iPreviousMouseOverIndex = -1;

    // 윈도우 내부에 응용프로그램 이름과 영역을 표시
    for( i = 0 ; i < iCount ; i++ )
    {
        kDrawApplicationListItem( i, FALSE );
    }
    
    kMoveWindow( qwWindowID, gs_stApplicationPanelData.stButtonArea.iX1, 
            gs_stApplicationPanelData.stButtonArea.iY2 + 5 );
    return TRUE;
}

/**
 *  애플리케이션 리스트 윈도우에 GUI 태스크 아이템을 표시
 */
static void kDrawApplicationListItem( int iIndex, bool bMouseOver )
{
    QWORD qwWindowID;
    int iWindowWidth;
    COLOR stColor;
    RECT stItemArea;
    
    // 애플리케이션 리스트 윈도우의 ID와 너비
    qwWindowID = gs_stApplicationPanelData.qwApplicationListID;
    iWindowWidth = gs_stApplicationPanelData.iApplicationListWidth;
    
    // 마우스가 위에 있는지 여부에 따라 내부 색을 다르게 표시
    if( bMouseOver == TRUE )
    {
        stColor = APPLICATIONPANEL_COLOR_ACTIVE;
    }
    else
    {
        stColor = APPLICATIONPANEL_COLOR_BACKGROUND;        
    }
    
    // 리스트 아이템에 테두리를 표시
    kSetRectangleData( 0, iIndex * APPLICATIONPANEL_LISTITEMHEIGHT,
        iWindowWidth - 1, ( iIndex + 1 ) * APPLICATIONPANEL_LISTITEMHEIGHT,
        &stItemArea );
    kDrawRect( qwWindowID, stItemArea.iX1, stItemArea.iY1, stItemArea.iX2, 
            stItemArea.iY2, APPLICATIONPANEL_COLOR_INNERLINE, FALSE );
    
    // 리스트 아이템의 내부를 채움
    kDrawRect( qwWindowID, stItemArea.iX1 + 1, stItemArea.iY1 + 1, 
               stItemArea.iX2 - 1, stItemArea.iY2 - 1, stColor, TRUE );
    
    // GUI 태스크의 이름을 표시
    kDrawText( qwWindowID, stItemArea.iX1 + 10, stItemArea.iY1 + 2, 
            RGB( 255, 255, 255 ), stColor,
            gs_vstApplicationTable[ iIndex ].pcApplicationName,
            strlen( gs_vstApplicationTable[ iIndex ].pcApplicationName ) );    
    
    // 업데이트된 아이템을 화면에 업데이트
    kUpdateScreenByWindowArea( qwWindowID, &stItemArea );
}

/**
 *  애플리케이션 리스트에 수신된 이벤트를 처리
 */
static bool kProcessApplicationListWindowEvent( void )
{
    EVENT stReceivedEvent;
    MOUSEEVENT* pstMouseEvent;
	bool bProcessResult;
    QWORD qwApplicationPanelID;
    QWORD qwApplicationListID;
    int iMouseOverIndex;
    EVENT stEvent;
    
    // 윈도우 ID 저장
    qwApplicationPanelID = gs_stApplicationPanelData.qwApplicationPanelID;
    qwApplicationListID = gs_stApplicationPanelData.qwApplicationListID;
    bProcessResult = FALSE;
    
    // 이벤트를 처리하는 루프
    while( 1 )
    {
        // 이벤트 큐에서 이벤트를 수신
        if( kReceiveEventFromWindowQueue( qwApplicationListID, &stReceivedEvent ) 
                == FALSE )
        {
            break;
        }

        bProcessResult = TRUE;
        
        // 수신된 이벤트를 타입에 따라 나누어 처리
        switch( stReceivedEvent.qwType )
        {
            // 마우스 이동 처리
        case EVENT_MOUSE_MOVE:
            pstMouseEvent = &( stReceivedEvent.stMouseEvent );

            // 마우스가 위치한 아이템 계산
            iMouseOverIndex = kGetMouseOverItemIndex( pstMouseEvent->stPoint.iY );
            
            // 현재 마우스가 위치한 아이템과 이전에 위치한 아이템이 다를 때만 수행
            if( ( iMouseOverIndex == gs_stApplicationPanelData.iPreviousMouseOverIndex ) ||
                ( iMouseOverIndex == -1 ) )
            {
                break;
            }
            
            // 이전에 마우스가 위치한 아이템은 기본 상태로 표시
            if( gs_stApplicationPanelData.iPreviousMouseOverIndex != -1 )
            {
                kDrawApplicationListItem( 
                    gs_stApplicationPanelData.iPreviousMouseOverIndex, FALSE );
            }
            
            // 지금 마우스 커서가 있는 위치는 마우스가 위치한 상태로 표시
            kDrawApplicationListItem( iMouseOverIndex, TRUE );
            
            // 마우스가 위치한 아이템을 저장해둠
            gs_stApplicationPanelData.iPreviousMouseOverIndex = iMouseOverIndex;            
            break;
            
            // 마우스 왼쪽 버튼 처리
        case EVENT_MOUSE_LBUTTONDOWN:
            pstMouseEvent = &( stReceivedEvent.stMouseEvent );
            
            // 지금 마우스 커서가 있는 위치는 선택된 것으로 표시
            iMouseOverIndex = kGetMouseOverItemIndex( pstMouseEvent->stPoint.iY );
            if( iMouseOverIndex == -1 )
            {
                break;
            }

            // 선택된 아이템을 실행
			//20180628
            //kCreateTask( TASK_FLAGS_LOW | TASK_FLAGS_THREAD, 0, 0, 
              //      ( QWORD ) gs_vstApplicationTable[ iMouseOverIndex ].pvEntryPoint, 
                //    TASK_LOADBALANCINGID );   

			g_processInterface.sky_kcreate_thread_from_memory(1, gs_vstApplicationTable[iMouseOverIndex].pvEntryPoint, NULL);

            // 애플리케이션 패널에 마우스 왼쪽 버튼이 눌렸다는 메시지를 전송하여 처리
            kSetMouseEvent( qwApplicationPanelID, EVENT_MOUSE_LBUTTONDOWN,
                    gs_stApplicationPanelData.stButtonArea.iX1 + 1, 
                    gs_stApplicationPanelData.stButtonArea.iY1 + 1, 
                    NULL, &stEvent );
            kSendEventToWindow( qwApplicationPanelID, &stEvent );            
            break;
            
            // 그 외 이벤트 처리
        default:
            break;
        }
    }
    
    return bProcessResult;
}

/**
 *  마우스 커서가 위치한 애플리케이션 리스트 윈도우의 아이템 인덱스를 반환
 */
static int kGetMouseOverItemIndex( int iMouseY )
{
    int iCount;
    int iItemIndex;
    
    // 애플리케이션 테이블의 총 아이템 수
    iCount = sizeof( gs_vstApplicationTable ) / sizeof( APPLICATIONENTRY );
    
    // 마우스 좌표로 아이템의 인덱스를 계산
    iItemIndex = iMouseY / APPLICATIONPANEL_LISTITEMHEIGHT;
    // 범위를 벗어나면 -1을 반환
    if( ( iItemIndex < 0 ) || ( iItemIndex >= iCount ) )
    {
        return -1;
    }
    
    return iItemIndex;
}
