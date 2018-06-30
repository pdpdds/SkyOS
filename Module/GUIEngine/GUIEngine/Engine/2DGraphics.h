/**
 *  file    2DGraphics.h
 *  date    2009/09/5
 *  author  kkamagui 
 *          Copyright(c)2008 All rights reserved by kkamagui
 *  brief   2D Graphic에 대한 헤더 파일
 */

#ifndef __2DGRAPHICS_H__
#define __2DGRAPHICS_H__

#include "windef.h"

////////////////////////////////////////////////////////////////////////////////
//
// 매크로
//
////////////////////////////////////////////////////////////////////////////////
// 색을 저장하는데 사용할 자료구조, 16비트 색을 사용하므로 WORD로 정의
typedef WORD                COLOR;

// 0~255 범위의 R, G, B를 16비트 색 형식으로 변환하는 매크로
// 0~255의 범위를 0~31, 0~63으로 축소하여 사용하므로 각각 8과 4로 나누어줘야 함
// 나누기 8과 나누기 4는 >> 3과 >> 2로 대체
#define RGB( r, g, b )      ( ( ( BYTE )( r ) >> 3 ) << 11 | \
                ( ( ( BYTE )( g ) >> 2 ) ) << 5 |  ( ( BYTE )( b ) >> 3 ) )


////////////////////////////////////////////////////////////////////////////////
//
// 구조체
//
////////////////////////////////////////////////////////////////////////////////
// 사각형의 정보를 담는 자료구조
typedef struct kRectangleStruct
{
    // 왼쪽 위(시작점)의 X 좌표
    int iX1;
    // 왼쪽 위(시작점)의 Y 좌표
    int iY1;
    
    // 오른쪽 아래(끝점)의 X 좌표
    int iX2;
    // 오른쪽 아래(끝점)의 Y좌표
    int iY2;
} RECT;

// 점의 정보를 담는 자료구조
typedef struct kPointStruct
{
    // X와 Y의 좌표
    int iX;
    int iY;
} POINT;

////////////////////////////////////////////////////////////////////////////////
//
// 함수
//
////////////////////////////////////////////////////////////////////////////////
inline void kInternalDrawPixel( const RECT* pstMemoryArea, COLOR* pstMemoryAddress, 
        int iX, int iY, COLOR stColor );
void kInternalDrawLine( const RECT* pstMemoryArea, COLOR* pstMemoryAddress, 
        int iX1, int iY1, int iX2, int iY2, COLOR stColor );
void kInternalDrawRect( const RECT* pstMemoryArea, COLOR* pstMemoryAddress,
        int iX1, int iY1, int iX2, int iY2, COLOR stColor, bool bFill );
void kInternalDrawCircle( const RECT* pstMemoryArea, COLOR* pstMemoryAddress,
        int iX, int iY, int iRadius, COLOR stColor, bool bFill );
int kInternalDrawText( const RECT* pstMemoryArea, COLOR* pstMemoryAddress,
        int iX, int iY, COLOR stTextColor, COLOR stBackgroundColor, 
        const char* pcString, int iLength );
int kInternalDrawEnglishText( const RECT* pstMemoryArea, COLOR* pstMemoryAddress,
        int iX, int iY, COLOR stTextColor, COLOR stBackgroundColor,
        const char* pcString, int iLength );
int kInternalDrawHangulText( const RECT* pstMemoryArea, COLOR* pstMemoryAddress,
        int iX, int iY, COLOR stTextColor, COLOR stBackgroundColor,
        const char* pcString, int iLength );

inline bool kIsInRectangle( const RECT* pstArea, int iX, int iY );
inline int kGetRectangleWidth( const RECT* pstArea );
inline int kGetRectangleHeight( const RECT* pstArea );
inline void kSetRectangleData( int iX1, int iY1, int iX2, int iY2, RECT* pstRect );
inline bool kGetOverlappedRectangle( const RECT* pstArea1, const RECT* pstArea2,
        RECT* pstIntersection  );
inline bool kIsRectangleOverlapped( const RECT* pstArea1, 
        const RECT* pstArea2 );

#endif /*__2DGRAPHICS_H__*/
