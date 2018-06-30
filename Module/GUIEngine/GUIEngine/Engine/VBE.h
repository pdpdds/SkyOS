/**
 *  file    VBE.h
 *  date    2009/08/29
 *  author  kkamagui 
 *          Copyright(c)2008 All rights reserved by kkamagui
 *  brief   VBE에 관련된 함수를 정의한 헤더 파일
 */

#ifndef __VBE_H__
#define __VBE_H__

#include "windef.h"

////////////////////////////////////////////////////////////////////////////////
//
// 매크로
//
////////////////////////////////////////////////////////////////////////////////
// 모드 정보 블록이 저장된 어드레스
#define VBE_MODEINFOBLOCKADDRESS            0x7E00
// 그래픽 모드로 시작하는 플래그가 저장된 어드레스
#define VBE_STARTGRAPHICMODEFLAGADDRESS     0x7C0A

////////////////////////////////////////////////////////////////////////////////
//
// 구조체
//
////////////////////////////////////////////////////////////////////////////////
#pragma pack( push, 1 )

// VBE에서 정의한 모드 정보 블록(ModeInfoBlock) 자료구조, 256바이트
typedef struct kVBEInfoBlockStruct
{
    //==========================================================================
    // 모든 VBE 버전에 공통인 부분
    //==========================================================================
    WORD wModeAttribute;        // 모드의 속성
    BYTE bWinAAttribute;        // 윈도우 A의 속성
    BYTE bWinBAttribute;        // 윈도우 B의 속성
    WORD wWinGranulity;         // 윈도우의 가중치(Granularity)
    WORD wWinSize;              // 윈도우의 크기
    WORD wWinASegment;          // 윈도우 A가 시작하는 세그먼트 어드레스
    WORD wWinBSegment;          // 윈도우 B가 시작하는 세그먼트 어드레스
    DWORD dwWinFuncPtr;         // 윈도우 관련 함수의 포인터(리얼 모드 용)
    WORD wBytesPerScanLine;     // 화면 스캔 라인(Scan Line) 당 바이트 수
    
    //==========================================================================
    // VBE 버전 1.2 이상 공통인 부분
    //==========================================================================
    WORD wXResolution;          // X축 픽셀 수 또는 문자 수
    WORD wYResolution;          // Y축 픽셀 수 또는 문자 수
    BYTE bXCharSize;            // 한 문자의 X축 픽셀 수
    BYTE bYCharSize;            // 한 문자의 Y축 픽셀 수
    BYTE bNumberOfPlane;        // 메모리 플레인(Memory Plane) 수
    BYTE bBitsPerPixel;         // 한 픽셀을 구성하는 비트 수
    BYTE bNumberOfBanks;        // 뱅크(Bank) 수
    BYTE bMemoryModel;          // 비디오 메모리 구성
    BYTE bBankSize;             // 뱅크의 크기(Kbyte)
    BYTE bNumberOfImagePages;   // 이미지 페이지 개수
    BYTE bReserved;             // 페이지 기능을 위해 예약된 영역
    
    // 다이렉트 컬러(Direct Color)에 관련된 필드
    BYTE bRedMaskSize;              // 빨간색(Red) 필드가 차지하는 크기
    BYTE bRedFieldPosition;         // 빨간색 필드의 위치
    BYTE bGreenMaskSize;            // 녹색(Green) 필드가 차지하는 크기
    BYTE bGreenFieldPosition;       // 녹색 필드의 위치
    BYTE bBlueMaskSize;             // 파란색(Blue) 필드가 차지하는 크기
    BYTE bBlueFieldPosition;        // 파란색 필드의 위치
    BYTE bReservedMaskSize;         // 예약된 필드의 크기
    BYTE bReservedFieldPosition;    // 예약된 필드의 위치
    BYTE bDirectColorModeInfo;      // 다이렉트 컬러 모드의 정보
    
    //==========================================================================
    // VBE 버전 2.0 이상 공통인 부분
    //==========================================================================
    DWORD dwPhysicalBasePointer;    // 선형 프레임 버퍼 메모리의 시작 어드레스
    DWORD dwReserved1;              // 예약된 필드
    DWORD dwReserved2;
    
    //==========================================================================
    // VBE 버전 3.0 이상 공통인 부분
    //==========================================================================
    WORD wLinearBytesPerScanLine;       // 선형 프레임 버퍼 모드의 
                                        // 화면 스캔 라인(Scan Line) 당 바이트 수
    BYTE bBankNumberOfImagePages;       // 뱅크 모드일 때 이미지 페이지 수
    BYTE bLinearNumberOfImagePages;     // 선형 프레임 버퍼 모드일 때 이미지 페이지 수
    // 선형 프레임 버퍼 모드일 때 다이렉트 컬러(Direct Color)에 관련된 필드
    BYTE bLinearRedMaskSize;            // 빨간색(Red) 필드가 차지하는 크기 
    BYTE bLinearRedFieldPosition;       // 빨간색 필드의 위치
    BYTE bLinearGreenMaskSize;          // 녹색(Green) 필드가 차지하는 크기
    BYTE bLinearGreenFieldPosition;     // 녹색 필드의 위치
    BYTE bLinearBlueMaskSize;           // 파란색(Blue) 필드가 차지하는 크기
    BYTE bLinearBlueFieldPosition;      // 파란색 필드의 위치
    BYTE bLinearReservedMaskSize;       // 예약된 필드의 크기
    BYTE bLinearReservedFieldPosition;  // 예약된 필드의 위치
    DWORD dwMaxPixelClock;              // 픽셀 클록의 최대 값(Hz)

    BYTE vbReserved[ 189 ];             // 나머지 영역
} VBEMODEINFOBLOCK;

#pragma pack( pop )

////////////////////////////////////////////////////////////////////////////////
//
// 함수
//
////////////////////////////////////////////////////////////////////////////////
VBEMODEINFOBLOCK* kGetVBEModeInfoBlock( void );

#endif /*__VBE_H__*/
