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

#include "MapFileAddress.h"
#include "string.h"
#include "sprintf.h"
//-NAME---------------------------------
//.DESCRIPTION..........................
//.PARAMETERS...........................
//.RETURN.CODES.........................
//--------------------------------------

MapFileAddress::MapFileAddress(WORD		section,
							   DWORD	address)
{
	setAddress(section, address);
}

//-NAME---------------------------------
//.DESCRIPTION..........................
//.PARAMETERS...........................
//.RETURN.CODES.........................
//--------------------------------------

MapFileAddress::MapFileAddress(const char *s)
{
	// string passed in is WORD:DWORD

	const char	*p1;
	char		*p2 = NULL;

	p1 = (const char *)s;
	
	sectionNumber = (WORD)strtoul(p1, &p2, 16);
	
	if (p2 != NULL)
	{
		// p2 should point at ':'

		if (*p2 == ':')
			p2++;

		address = (DWORD)strtoul(p2, NULL, 16);
	}
	else
	{
		sectionNumber = NULL;	// error
		address = NULL;			// error
	}
}

//-NAME---------------------------------
//.DESCRIPTION..........................
//.PARAMETERS...........................
//.RETURN.CODES.........................
//--------------------------------------

MapFileAddress::~MapFileAddress()
{
}

//-NAME---------------------------------
//.DESCRIPTION..........................
//.PARAMETERS...........................
//.RETURN.CODES.........................
//--------------------------------------

void MapFileAddress::getAddressString(std::string	&s)
{
	//s.Format(_T("0x%04x:0x%08lx"), sectionNumber, address);
	char buf[MAXPATH];
	sprintf(buf, "0x%x:0x%x", sectionNumber, address);
	s = buf;
}