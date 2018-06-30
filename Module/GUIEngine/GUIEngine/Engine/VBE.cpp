/**
 *  file    VBE.h
 *  date    2009/08/29
 *  author  kkamagui 
 *          Copyright(c)2008 All rights reserved by kkamagui
 *  brief   VBE에 관련된 함수를 정의한 소스 파일
 */

#include "VBE.h"

// 모드 정보 블록 자료구조
static VBEMODEINFOBLOCK* gs_pstVBEModeBlockInfo = 
    ( VBEMODEINFOBLOCK* ) VBE_MODEINFOBLOCKADDRESS;

VBEMODEINFOBLOCK* g_pstVBEModeBlockInfo = nullptr;

/**
 *  VBE 모드 정보 블록을 반환
 */
VBEMODEINFOBLOCK* kGetVBEModeInfoBlock( void )
{
	if (g_pstVBEModeBlockInfo == nullptr)
		g_pstVBEModeBlockInfo = new VBEMODEINFOBLOCK;

	return g_pstVBEModeBlockInfo;
 //   return gs_pstVBEModeBlockInfo;
}
