/**
 *  file    2DGraphics.h
 *  date    2009/09/5
 *  author  kkamagui 
 *          Copyright(c)2008 All rights reserved by kkamagui
 *  brief   2D Graphic에 대한 소스 파일
 */

#include "2DGraphics.h"
#include "MintFont.h"
#include "math.h"

/**
*  메모리를 16bit 특정 값으로 채움
*      iSize는 채울 16bit 데이터의 개수를 의미
*/
inline void kMemSetWord(void* pvDestination, WORD wData, int iWordSize)
{
	int i;
	QWORD qwData;
	int iRemainWordStartOffset;

	// 8 바이트에 WORD 데이터를 채움
	qwData = 0;
	for (i = 0; i < 4; i++)
	{
		qwData = (qwData << 16) | wData;
	}

	// 8 바이트씩 먼저 채움, WORD 데이터를 4개씩 한꺼번에 채울 수 있음
	for (i = 0; i < (iWordSize / 4); i++)
	{
		((QWORD*)pvDestination)[i] = qwData;
	}

	// 8 바이트씩 채우고 남은 부분을 마무리
	iRemainWordStartOffset = i * 4;
	for (i = 0; i < (iWordSize % 4); i++)
	{
		((WORD*)pvDestination)[iRemainWordStartOffset++] = wData;
	}
}	

/**
 *  (x, y)가 사각형 영역 안에 있는지 여부를 반환
 */
inline bool kIsInRectangle( const RECT* pstArea, int iX, int iY )
{
    // 화면에 표시되는 영역을 벗어났다면 그리지 않음
    if( ( iX <  pstArea->iX1 ) || ( pstArea->iX2 < iX ) ||
        ( iY <  pstArea->iY1 ) || ( pstArea->iY2 < iY ) )
    {
        return FALSE;
    }
    
    return TRUE;
}

/**
 *  사각형의 너비를 반환
 */
inline int kGetRectangleWidth( const RECT* pstArea )
{
    int iWidth;
    
    iWidth = pstArea->iX2 - pstArea->iX1 + 1;
    
    if( iWidth < 0 )
    {
        return -iWidth;
    }
    
    return iWidth;
}

/**
 *  사각형의 높이를 반환
 */
inline int kGetRectangleHeight( const RECT* pstArea )
{
    int iHeight;
    
    iHeight = pstArea->iY2 - pstArea->iY1 + 1;
    
    if( iHeight < 0 )
    {
        return -iHeight;
    }
    
    return iHeight;
}

/**
 *  두 개의 사각형이 교차하는가 판단하여 결과를 반환
 */
inline bool kIsRectangleOverlapped( const RECT* pstArea1, const RECT* pstArea2 )
{
    // 영역 1의 끝점이 영역 2의 시작점보다 작은 경우나
    // 영역 1의 시작점이 영역 2의 끝점보다 큰 경우는 서로 겹치는 부분이 없음
    if( ( pstArea1->iX1 > pstArea2->iX2 ) || ( pstArea1->iX2 < pstArea2->iX1 ) ||
        ( pstArea1->iY1 > pstArea2->iY2 ) || ( pstArea1->iY2 < pstArea2->iY1 ) )
    {
        return FALSE;
    }
    return TRUE;
}

/**
 *  영역 1과 영역 2의 겹치는 영역을 반환
 */
inline bool kGetOverlappedRectangle( const RECT* pstArea1, const RECT* pstArea2,
        RECT* pstIntersection  )
{
    int iMaxX1;
    int iMinX2;
    int iMaxY1;
    int iMinY2;
    
    // X축의 시작점은 두 점 중에서 큰 것을 찾음
    iMaxX1 = MAX( pstArea1->iX1, pstArea2->iX1 );
    // X축의 끝점은 두 점 중에서 작은 것을 찾음
    iMinX2 = MIN( pstArea1->iX2, pstArea2->iX2 );
    // 계산한 시작점의 위치가 끝점의 위치보다 크다면 두 사각형은 겹치지 않음
    if( iMinX2 < iMaxX1 )
    {
        return FALSE;
    }
    
    // Y축의 시작점은 두 점 중에서 큰 것을 찾음
    iMaxY1 = MAX( pstArea1->iY1, pstArea2->iY1 );
    // Y축의 끝점은 두 점 중에서 작은 것을 찾음
    iMinY2 = MIN( pstArea1->iY2, pstArea2->iY2 );
    // 계산한 시작점의 위치가 끝점의 위치보다 크다면 두 사각형은 겹치지 않음
    if( iMinY2 < iMaxY1 )
    {
        return FALSE;
    }
    
    // 겹치는 영역의 정보 저장
    pstIntersection->iX1 = iMaxX1;
    pstIntersection->iY1 = iMaxY1;
    pstIntersection->iX2 = iMinX2;
    pstIntersection->iY2 = iMinY2;
    
    return TRUE;
}

/**
 *  사각형 자료구조를 채움
 *      x1과 x2, y1과 y2를 비교해서 x1 < x2, y1 < y2가 되도록 저장
 */
inline void kSetRectangleData( int iX1, int iY1, int iX2, int iY2, RECT* pstRect )
{
    // x1 < x2가 되도록 RECT 자료구조에 X좌표를 설정
    if( iX1 < iX2 )
    {
        pstRect->iX1 = iX1;
        pstRect->iX2 = iX2;
    }
    else
    {
        pstRect->iX1 = iX2;
        pstRect->iX2 = iX1;
    }
    
    // y1 < y2가 되도록 RECT 자료구조에 Y좌표를 설정
    if( iY1 < iY2 )
    {
        pstRect->iY1 = iY1;
        pstRect->iY2 = iY2;
    }
    else
    {
        pstRect->iY1 = iY2;
        pstRect->iY2 = iY1;
    }
}

/**
 *  점 그리기
 */
inline void kInternalDrawPixel( const RECT* pstMemoryArea, COLOR* pstMemoryAddress, 
        int iX, int iY, COLOR stColor )
{
    int iWidth;
    
    // 클리핑 처리
    // 화면에 표시되는 영역을 벗어났다면 그리지 않음
    if( kIsInRectangle( pstMemoryArea, iX, iY ) == FALSE )
    {
        return ;
    }
    
    // 출력할 메모리 영역의 너비를 구함
    iWidth = kGetRectangleWidth( pstMemoryArea );
    
    // 픽셀 오프셋으로 계산하여 픽셀 출력
    *( pstMemoryAddress + ( iWidth * iY ) + iX ) = stColor;
}

/**
 *  직선 그리기
 */
void kInternalDrawLine( const RECT* pstMemoryArea, COLOR* pstMemoryAddress, 
        int iX1, int iY1, int iX2, int iY2, COLOR stColor )
{
    int iDeltaX, iDeltaY;
    int iError = 0;
    int iDeltaError;
    int iX, iY;
    int iStepX, iStepY;
    RECT stLineArea;
    
    // 클리핑 처리
    // 직선이 그려지는 영역과 메모리 영역이 겹치지 않으면 그리지 않아도 됨
    kSetRectangleData( iX1, iY1, iX2, iY2, &stLineArea );
    if( kIsRectangleOverlapped( pstMemoryArea, &stLineArea ) == FALSE )
    {
        return ;
    }
    
    // 변화량 계산
    iDeltaX = iX2 - iX1;
    iDeltaY = iY2 - iY1;

    // X축 변화량에 따라 X축 증감 방향 계산
    if( iDeltaX < 0 ) 
    {
        iDeltaX = -iDeltaX; 
        iStepX = -1; 
    } 
    else 
    { 
        iStepX = 1; 
    }

    // Y축 변화량에 따라 Y축 증감 방향 계산 
    if( iDeltaY < 0 ) 
    {
        iDeltaY = -iDeltaY; 
        iStepY = -1; 
    } 
    else 
    {
        iStepY = 1; 
    }

    // X축 변화량이 Y축 변화량보다 크다면 X축을 중심으로 직선을 그림
    if( iDeltaX > iDeltaY )
    {
        // 기울기로 매 픽셀마다 더해줄 오차, Y축 변화량의 2배
        // 시프트 연산으로 * 2를 대체
        iDeltaError = iDeltaY << 1;
        iY = iY1;
        for( iX = iX1 ; iX != iX2 ; iX += iStepX )
        {
            // 점 그리기
            kInternalDrawPixel( pstMemoryArea, pstMemoryAddress, iX, iY, stColor );

            // 오차 누적
            iError += iDeltaError;

            // 누적된 오차가 X축 변화량보다 크면 위에 점을 선택하고 오차를 위에 점을
            // 기준으로 갱신
            if( iError >= iDeltaX )
            {
                iY += iStepY;
                // X축의 변화량의 2배를 빼줌
                // 시프트 연산으로 *2를 대체
                iError -= iDeltaX << 1;
            }
        }
        
        // iX == iX2인 최종 위치에 점 그리기
        kInternalDrawPixel( pstMemoryArea, pstMemoryAddress, iX, iY, stColor );
    }
    // Y축 변화량이 X축 변화량보다 크거나 같다면 Y축을 중심으로 직선을 그림
    else
    {
        // 기울기로 매 픽셀마다 더해줄 오차, X축 변화량의 2배
        // 시프트 연산으로 * 2를 대체
        iDeltaError = iDeltaX << 1;
        iX = iX1;
        for( iY = iY1 ; iY != iY2 ; iY += iStepY )
        {
            // 점 그리기
            kInternalDrawPixel( pstMemoryArea, pstMemoryAddress, iX, iY, stColor );

            // 오차 누적
            iError += iDeltaError;

            // 누적된 오차가 Y축 변화량보다 크면 위에 점을 선택하고 오차를 위에 점을
            // 기준으로 갱신
            if( iError >= iDeltaY )
            {
                iX += iStepX;
                // Y축의 변화량의 2배를 빼줌
                // 시프트 연산으로 *2를 대체
                iError -= iDeltaY << 1;
            }
        }

        // iY == iY2인 최종 위치에 점 그리기
        kInternalDrawPixel( pstMemoryArea, pstMemoryAddress, iX, iY, stColor );
    }
}

/**
 *  사각형 그리기
 */
void kInternalDrawRect( const RECT* pstMemoryArea, COLOR* pstMemoryAddress,
        int iX1, int iY1, int iX2, int iY2, COLOR stColor, bool bFill )
{
    int iWidth;
    int iTemp;
    int iY;
    int iMemoryAreaWidth;
    RECT stDrawRect;
    RECT stOverlappedArea;

    // 채움 여부에 따라 코드를 분리
    if( bFill == FALSE )
    {
        // 네 점을 이웃한 것끼리 직선으로 연결
        kInternalDrawLine( pstMemoryArea, pstMemoryAddress, iX1, iY1, iX2, iY1, stColor );
        kInternalDrawLine( pstMemoryArea, pstMemoryAddress, iX1, iY1, iX1, iY2, stColor );
        kInternalDrawLine( pstMemoryArea, pstMemoryAddress, iX2, iY1, iX2, iY2, stColor );
        kInternalDrawLine( pstMemoryArea, pstMemoryAddress, iX1, iY2, iX2, iY2, stColor );
    }
    else
    {
        // 출력할 사각형의 정보를 RECT 자료구조에 저장
        kSetRectangleData( iX1, iY1, iX2, iY2, &stDrawRect );
        
        // 출력할 메모리 영역과 사각형 영역이 겹치는 부분을 계산하여 클리핑 처리
        if( kGetOverlappedRectangle( pstMemoryArea, &stDrawRect, 
                &stOverlappedArea ) == FALSE )
        {
            // 겹치는 영역이 없으면 그릴 필요 없음
            return ;
        }
                
        // 클리핑된 사각형의 너비를 계산
        iWidth = kGetRectangleWidth( &stOverlappedArea );
        
        // 출력할 메모리 영역의 너비를 계산
        iMemoryAreaWidth = kGetRectangleWidth( pstMemoryArea );
        
        // 출력할 메모리 어드레스의 시작 위치를 계산
        // 파라미터로 전달된 사각형을 그대로 그리는 것이 아니라 클리핑 처리된 사각형을
        // 기준으로 그림
        pstMemoryAddress += stOverlappedArea.iY1 * iMemoryAreaWidth + 
            stOverlappedArea.iX1;
        
        // 루프를 돌면서 각 Y축마다 값을 채움
        for( iY = stOverlappedArea.iY1 ; iY < stOverlappedArea.iY2 ; iY++ )
        {
            // 메모리에 사각형의 너비만큼 픽셀을 채움
            kMemSetWord( pstMemoryAddress, stColor, iWidth );
            
            // 출력할 비디오 메모리 어드레스 갱신
            // x, y좌표로 매번 비디오 메모리 어드레스를 계산하는 것을 피하려고
            // X축 해상도를 이용하여 다음 라인의 y좌표 어드레스를 계산 
            pstMemoryAddress += iMemoryAreaWidth;
        }
        
        // 메모리에 사각형의 너비만큼 픽셀을 채움, 마지막 줄 출력
        kMemSetWord( pstMemoryAddress, stColor, iWidth );
    }
}

/**
 *  원 그리기
 */
void kInternalDrawCircle( const RECT* pstMemoryArea, COLOR* pstMemoryAddress,
        int iX, int iY, int iRadius, COLOR stColor, bool bFill )
{
    int iCircleX, iCircleY;
    int iDistance;
    
    // 반지름이 0보다 작다면 그릴 필요 없음
    if( iRadius < 0 )
    {
        return ;
    }
    
    // (0, R)인 좌표에서 시작
    iCircleY = iRadius;

    // 채움 여부에 따라 시작점을 그림
    if( bFill == FALSE )
    {
        // 시작점은 네 접점 모두 그림
        kInternalDrawPixel( pstMemoryArea, pstMemoryAddress, 0 + iX, iRadius + iY, stColor);
        kInternalDrawPixel( pstMemoryArea, pstMemoryAddress, 0 + iX, -iRadius + iY, stColor);
        kInternalDrawPixel( pstMemoryArea, pstMemoryAddress, iRadius + iX, 0 + iY, stColor);
        kInternalDrawPixel( pstMemoryArea, pstMemoryAddress, -iRadius + iX, 0 + iY, stColor);
    }
    else
    {
        // 시작 직선은 X축과 Y축 모두 그림
        kInternalDrawLine( pstMemoryArea, pstMemoryAddress, 
                   0 + iX, iRadius + iY, 0 + iX, -iRadius + iY, stColor);
        kInternalDrawLine( pstMemoryArea, pstMemoryAddress, 
                iRadius + iX, 0 + iY, -iRadius + iX, 0 + iY, stColor);
    }
    
    // 최초 시작점의 중심점과 원의 거리
    iDistance = -iRadius;

    // 원 그리기
    for( iCircleX = 1 ; iCircleX <= iCircleY ; iCircleX++ )
    {
        // 원에서 떨어진 거리 계산
        // 시프트 연산으로 * 2를 대체
        iDistance += ( iCircleX << 1 ) - 1;  //2 * iCircleX - 1;
                    
        // 중심점이 원의 외부에 있으면 아래에 있는 점 선택
        if( iDistance >= 0 )
        {
            iCircleY--;
            
            // 새로운 점에서 다시 원과 거리 계산
            // 시프트 연산으로 * 2를 대체
            iDistance += ( -iCircleY << 1 ) + 2; //-2 * iCircleY + 2;
        }
        
        // 채움 여부에 따라 그림
        if( bFill == FALSE )
        {
            // 8 방향 모두 점 그림
            kInternalDrawPixel( pstMemoryArea, pstMemoryAddress, 
                    iCircleX + iX, iCircleY + iY, stColor );
            kInternalDrawPixel( pstMemoryArea, pstMemoryAddress, 
                    iCircleX + iX, -iCircleY + iY, stColor );
            kInternalDrawPixel( pstMemoryArea, pstMemoryAddress, 
                    -iCircleX + iX, iCircleY + iY, stColor );
            kInternalDrawPixel( pstMemoryArea, pstMemoryAddress, 
                    -iCircleX + iX, -iCircleY + iY, stColor );
            kInternalDrawPixel( pstMemoryArea, pstMemoryAddress, 
                    iCircleY + iX, iCircleX + iY, stColor );
            kInternalDrawPixel( pstMemoryArea, pstMemoryAddress, 
                    iCircleY + iX, -iCircleX + iY, stColor );
            kInternalDrawPixel( pstMemoryArea, pstMemoryAddress, 
                    -iCircleY + iX, iCircleX + iY, stColor );
            kInternalDrawPixel( pstMemoryArea, pstMemoryAddress, 
                    -iCircleY + iX, -iCircleX + iY, stColor );
        }
        else
        {
            // 대칭되는 점을 찾아 X축에 평행한 직선을 그어 채워진 원을 그림
            // 평행선을 그리는 것은 사각형 그리기 함수로 빠르게 처리할 수 있음
            kInternalDrawRect( pstMemoryArea, pstMemoryAddress, -iCircleX + iX, iCircleY + iY, 
                    iCircleX + iX, iCircleY + iY, stColor, TRUE );
            kInternalDrawRect( pstMemoryArea, pstMemoryAddress, -iCircleX + iX, -iCircleY + iY, 
                    iCircleX + iX, -iCircleY + iY, stColor, TRUE );
            kInternalDrawRect( pstMemoryArea, pstMemoryAddress, -iCircleY + iX, iCircleX + iY, 
                    iCircleY + iX, iCircleX + iY, stColor, TRUE );
            kInternalDrawRect( pstMemoryArea, pstMemoryAddress, -iCircleY + iX, -iCircleX + iY, 
                    iCircleY + iX, -iCircleX + iY, stColor, TRUE );
        }
    }
}

/**
 *  문자 출력
 */
int kInternalDrawText(const RECT* pstMemoryArea, COLOR* pstMemoryAddress,
	int iX, int iY, COLOR stTextColor, COLOR stBackgroundColor,
	const char* pcString, int iLength)
{
	int i;
	int j;
	int totalLength = 0;

	for (i = 0; i < iLength; )
	{
		// 현재 문자가 한글이 아니면 영문자가 끝나는 곳을 검색
		if ((pcString[i] & 0x80) == 0)
		{
			// 문자열의 끝까지 검색
			for (j = i; j < iLength; j++)
			{
				if (pcString[j] & 0x80)
				{
					break;
				}
			}

			// 영문자를 출력하는 함수를 호출하고 현재 위치를 갱신
			int length = kInternalDrawEnglishText(pstMemoryArea, pstMemoryAddress,
				iX + (i * FONT_ENGLISHWIDTH), iY, stTextColor, stBackgroundColor,
				pcString + i, j - i);
			totalLength += length;
			i = j;
		}
		// 현재 문자가 한글이면 한글이 끝나는 곳을 검색
		else
		{
			// 문자열의 끝까지 검색
			for (j = i; j < iLength; j++)
			{
				if ((pcString[j] & 0x80) == 0)
				{
					break;
				}
			}

			// 영문자를 출력하는 함수를 호출하고 현재 위치를 갱신
			int length = kInternalDrawHangulText(pstMemoryArea, pstMemoryAddress,
				iX + i * FONT_ENGLISHWIDTH, iY, stTextColor, stBackgroundColor,
				pcString + i, j - i);
			i = j;
			totalLength += length;
		}
	}

	return totalLength;
}

/**
 *  영문자 문자 출력
 */
int kInternalDrawEnglishText(const RECT* pstMemoryArea, COLOR* pstMemoryAddress,
	int iX, int iY, COLOR stTextColor, COLOR stBackgroundColor,
	const char* pcString, int iLength)
{
	int iCurrentX, iCurrentY;
	int i, j, k;
	BYTE bBitmap;
	BYTE bCurrentBitmask;
	int iBitmapStartIndex;
	int iMemoryAreaWidth;
	RECT stFontArea;
	RECT stOverlappedArea;
	int iStartYOffset;
	int iStartXOffset;
	int iOverlappedWidth;
	int iOverlappedHeight;

	// 문자를 출력하는 X좌표
	iCurrentX = iX;

	// 메모리 영역의 너비를 계산
	iMemoryAreaWidth = kGetRectangleWidth(pstMemoryArea);

	// 문자의 개수만큼 반복
	for (k = 0; k < iLength; k++)
	{
		// 문자를 출력할 위치의 Y좌표를 구함
		iCurrentY = iY * iMemoryAreaWidth;

		// 현재 폰트를 표시하는 영역을 RECT 자료구조에 설정
		kSetRectangleData(iCurrentX, iY, iCurrentX + FONT_ENGLISHWIDTH - 1,
			iY + FONT_ENGLISHHEIGHT - 1, &stFontArea);

		// 현재 그려야 할 문자가 메모리 영역과 겹치는 부분이 없으면 다음 문자로 이동
		if (kGetOverlappedRectangle(pstMemoryArea, &stFontArea,
			&stOverlappedArea) == FALSE)
		{
			// 문자 하나를 뛰어넘었으므로 폰트의 너비만큼 x좌표를 이동하여 다음 문자를 출력
			iCurrentX += FONT_ENGLISHWIDTH;
			continue;
		}

		// 비트맵 폰트 데이터에서 출력할 문자의 비트맵이 시작하는 위치를 계산
		// 1바이트 * FONT_HEIGHT로 구성되어 있으므로 문자의 비트맵 위치는
		// 아래와 같이 계산 가능
		iBitmapStartIndex = pcString[k] * FONT_ENGLISHHEIGHT;

		// 문자를 출력할 영역과 메모리 영역이 겹치는 부분을 이용하여 x, y오프셋과
		// 출력할 너비, 높이를 계산
		iStartXOffset = stOverlappedArea.iX1 - iCurrentX;
		iStartYOffset = stOverlappedArea.iY1 - iY;
		iOverlappedWidth = kGetRectangleWidth(&stOverlappedArea);
		iOverlappedHeight = kGetRectangleHeight(&stOverlappedArea);

		// 출력에서 제외된 y오프셋만큼 비트맵 데이터를 제외
		iBitmapStartIndex += iStartYOffset;

		// 문자 출력
		// 겹치는 영역의 y오프셋부터 높이만큼 출력
		for (j = iStartYOffset; j < iOverlappedHeight; j++)
		{
			// 이번 라인에서 출력할 폰트 비트맵과 비트 오프셋 계산
			bBitmap = g_vucEnglishFont[iBitmapStartIndex++];
			bCurrentBitmask = 0x01 << (FONT_ENGLISHWIDTH - 1 - iStartXOffset);

			// 겹치는 영역의 x오프셋부터 너비만큼 현재 라인에 출력
			for (i = iStartXOffset; i <= iOverlappedWidth; i++)
			{
				// 비트가 설정되어있으면 화면에 문자색을 표시
				if (bBitmap & bCurrentBitmask)
				{
					pstMemoryAddress[iCurrentY + iCurrentX + i] = stTextColor;
				}
				// 비트가 설정되어있지 않으면 화면에 배경색을 표시
				else
				{
					pstMemoryAddress[iCurrentY + iCurrentX + i] = stBackgroundColor;
				}

				bCurrentBitmask = bCurrentBitmask >> 1;
			}

			// 다음 라인으로 이동해야 하므로, 현재 y좌표에 메모리 영역의 너비만큼 더해줌
			iCurrentY += iMemoryAreaWidth;
		}

		// 문자 하나를 다 출력했으면 폰트의 넓이만큼 X 좌표를 이동하여 다음 문자를 출력
		iCurrentX += FONT_ENGLISHWIDTH;
	}

	return iCurrentX - iX;
}

/**
 *  한글 출력
 */
int kInternalDrawHangulText(const RECT* pstMemoryArea, COLOR* pstMemoryAddress,
	int iX, int iY, COLOR stTextColor, COLOR stBackgroundColor,
	const char* pcString, int iLength)
{
	int iCurrentX, iCurrentY;
	WORD wHangul;
	WORD wOffsetInGroup;
	WORD wGroupIndex;
	int i, j, k;
	WORD wBitmap;
	WORD wCurrentBitmask;
	int iBitmapStartIndex;
	int iMemoryAreaWidth;
	RECT stFontArea;
	RECT stOverlappedArea;
	int iStartYOffset;
	int iStartXOffset;
	int iOverlappedWidth;
	int iOverlappedHeight;

	// 문자를 출력하는 X좌표
	iCurrentX = iX;

	// 메모리 영역의 너비를 계산
	iMemoryAreaWidth = kGetRectangleWidth(pstMemoryArea);

	// 한글 문자의 개수만큼 반복
	for (k = 0; k < iLength; k += 2)
	{
		// 문자를 출력할 위치의 Y좌표를 구함
		iCurrentY = iY * iMemoryAreaWidth;

		// 현재 폰트를 표시하는 영역을 RECT 자료구조에 설정
		kSetRectangleData(iCurrentX, iY, iCurrentX + FONT_HANGULWIDTH - 1,
			iY + FONT_HANGULHEIGHT - 1, &stFontArea);

		// 현재 그려야 할 문자가 메모리 영역과 겹치는 부분이 없으면 다음 문자로 이동
		if (kGetOverlappedRectangle(pstMemoryArea, &stFontArea,
			&stOverlappedArea) == FALSE)
		{
			// 문자 하나를 뛰어넘었으므로 폰트의 너비만큼 x좌표를 이동하여 다음 문자를 출력
			iCurrentX += FONT_HANGULWIDTH;
			continue;
		}

		// 비트맵 폰트 데이터에서 출력할 문자의 비트맵이 시작하는 위치를 계산
		// 2바이트 * FONT_HEIGHT로 구성되어 있으므로 문자의 비트맵 위치는
		// 아래와 같이 계산 가능
		// 바이트를 워드로 변환
		wHangul = ((WORD)pcString[k] << 8) | (BYTE)(pcString[k + 1]);

		// 완성형 가~힝까지이면 자/모 오프셋을 더해줌
		if ((0xB0A1 <= wHangul) && (wHangul <= 0xC8FE))
		{
			wOffsetInGroup = (wHangul - 0xB0A1) & 0xFF;
			wGroupIndex = ((wHangul - 0xB0A1) >> 8) & 0xFF;
			// 그룹당 94개 문자가 있고 51개는 완성형에 없는 자모가 들어있으므로 그룹 인덱스에 94를 곱한 뒤
			// 그룹 내 오프셋에 51을 더하면 폰트 데이터에서 몇 번째인지 계산할 수 있음
			wHangul = (wGroupIndex * 94) + wOffsetInGroup + 51;
		}
		// 만약 자/모이면 자음의 시작인 ㄱ을 빼서 오프셋을 구함
		else if ((0xA4A1 <= wHangul) && (wHangul <= 0xA4D3))
		{
			wHangul = wHangul - 0xA4A1;
		}
		// 위의 두 가지 경우가 아니면 처리할 수 없으므로 다음 문자로 넘어감
		else
		{
			continue;
		}

		iBitmapStartIndex = wHangul * FONT_HANGULHEIGHT;

		// 문자를 출력할 영역과 메모리 영역이 겹치는 부분을 이용하여 x, y오프셋과
		// 출력할 너비, 높이를 계산
		iStartXOffset = stOverlappedArea.iX1 - iCurrentX;
		iStartYOffset = stOverlappedArea.iY1 - iY;
		iOverlappedWidth = kGetRectangleWidth(&stOverlappedArea);
		iOverlappedHeight = kGetRectangleHeight(&stOverlappedArea);

		// 출력에서 제외된 y오프셋만큼 비트맵 데이터를 제외
		iBitmapStartIndex += iStartYOffset;

		// 문자 출력
		// 겹치는 영역의 y오프셋부터 높이만큼 출력
		for (j = iStartYOffset; j < iOverlappedHeight; j++)
		{
			// 이번 라인에서 출력할 폰트 비트맵과 비트 오프셋 계산
			wBitmap = g_vusHangulFont[iBitmapStartIndex++];
			wCurrentBitmask = 0x01 << (FONT_HANGULWIDTH - 1 - iStartXOffset);

			// 겹치는 영역의 x오프셋부터 너비만큼 현재 라인에 출력
			for (i = iStartXOffset; i < iOverlappedWidth; i++)
			{
				// 비트가 설정되어있으면 화면에 문자색을 표시
				if (wBitmap & wCurrentBitmask)
				{
					pstMemoryAddress[iCurrentY + iCurrentX + i] = stTextColor;
				}
				// 비트가 설정되어있지 않으면 화면에 배경색을 표시
				else
				{
					pstMemoryAddress[iCurrentY + iCurrentX + i] = stBackgroundColor;
				}

				wCurrentBitmask = wCurrentBitmask >> 1;
			}

			// 다음 라인으로 이동해야 하므로, 현재 y좌표에 메모리 영역의 너비만큼 더해줌
			iCurrentY += iMemoryAreaWidth;
		}

		// 문자 하나를 다 출력했으면 폰트의 넓이만큼 X 좌표를 이동하여 다음 문자를 출력
		iCurrentX += FONT_HANGULWIDTH;
	}

	return iCurrentX - iX;
}
