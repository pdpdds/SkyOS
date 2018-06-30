/**
 *  file    Font.h
 *  date    2009/09/09
 *  author  kkamagui 
 *          Copyright(c)2008 All rights reserved by kkamagui
 *  brief   비트맵 폰트에 관련된 헤더 파일
 */

#ifndef __FONT_H__
#define __FONT_H__

////////////////////////////////////////////////////////////////////////////////
//
// 매크로와 전역 변수
//
////////////////////////////////////////////////////////////////////////////////
// 영문 폰트의 너비와 길이
#define FONT_ENGLISHWIDTH   8
#define FONT_ENGLISHHEIGHT  16

// 한글 폰트의 너비와 길이
#define FONT_HANGULWIDTH   16
#define FONT_HANGULHEIGHT  16

// 영문 비트맵 폰트 데이터
extern unsigned char g_vucEnglishFont[];
// 한글 비트맵 폰트 데이터
extern unsigned short g_vusHangulFont[];

#endif /*__FONT_H__*/
