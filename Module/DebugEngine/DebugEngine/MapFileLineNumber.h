#ifndef _MAP_FILE_LINE_NUMBER_H
#define _MAP_FILE_LINE_NUMBER_H
#include "windef.h"
#include "stl_string.h"
//----------------------------------------
// Copyright (c) 2001 Object Media Limited
// (Un-Published Material)
//
// Object Media Limited
// 24 Windmill Walk
// Sutton
// Ely 
// Cambs
// CB6 2NH
// United Kingdom
//
// snail@objmedia.demon.co.uk
//
//----------------------------------------
// Author:  Stephen Kellett
// Date:    16/Sept/2001
//
// Purpose: 
//----------------------------------------

//----------------------------------------
// License:
//
// You may use this source code in any product, commercial, shareware, or freeware. 
//
// You MUST not impose a GNU style license on this software. If it is going to be 
// free for the use of all, it must not have a GNU, or similar style license imposed 
// on it. That is, you may take this source code and embed as part of a commercial 
// product without the requirement to then disclose the source code you added to 
// create your proprietry product. 
//
// You can give away the source code and/or the binary executable.
//
// You can sell for profit the binary executable.
//
// You can not sell the source code that accompanies this license. You may charge a reasonable fee for
// copying the source code.
//
// You can not include this source code in a magazine, CD, or other storage media that is distributed
// on a for-profit basis without the written (on paper) permission of Object Media Limited. Source code 
// and software developer centred websites such as www.codeproject.com may host the source code, but may
// not sell the source code on a CD.
//
// You MUST keep this license in the source code, and keep the copyright notice at the top of 
// this file that identifies Object Media Limited as the original author of this source code.
//----------------------------------------

#ifndef MAPFILEDLL_API
#ifdef MAPFILEDLL_EXPORTS
#define MAPFILEDLL_API __declspec(dllexport)
#else
#define MAPFILEDLL_API __declspec(dllimport)
#endif
#endif

class MapFileAddress;

class MapFileLineNumber
{
public:
	MAPFILEDLL_API MapFileLineNumber();

	MAPFILEDLL_API MapFileLineNumber(std::string		p_lineNumber,
									 MapFileAddress	*mfa);

	MAPFILEDLL_API MapFileLineNumber(int			p_lineNumber,
									 MapFileAddress	*mfa);

	MAPFILEDLL_API ~MapFileLineNumber();

	MAPFILEDLL_API int getLineNumber()
	{
		return lineNumber;
	}
	
	MAPFILEDLL_API MapFileAddress *getLineNumberAddress()
	{
		return address;
	}

	MAPFILEDLL_API void setLineNumber(int	ln)
	{
		lineNumber = ln;
	}
	
	MAPFILEDLL_API void setLineNumberAddress(MapFileAddress *a)
	{
		address = a;
	}

private:
	MapFileAddress	*address;
	int				lineNumber;
};

#endif
