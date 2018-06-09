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

#include "MapFileSymbol.h"
#include "MapFileAddress.h"
#include "string.h"
#include "sprintf.h"
#include "SkyMockInterface.h"
//-NAME---------------------------------
//.DESCRIPTION..........................
//.PARAMETERS...........................
//.RETURN.CODES.........................
//--------------------------------------

MapFileSymbol::MapFileSymbol()
{
	address = NULL;
	rvaBase = NULL;
	
	isFunction = FALSE;
	isInline = FALSE;
	
	isImported = FALSE;
	isExported = FALSE;
	
	isObject = FALSE;
	isLibraryObject = FALSE;
	isDLLObject = FALSE;

	// extras for C++

	isCPP = FALSE;
	isDestructor = FALSE;
	isCRT = FALSE;
}

//-NAME---------------------------------
//.DESCRIPTION..........................
//.PARAMETERS...........................
//.RETURN.CODES.........................
//--------------------------------------

MapFileSymbol::~MapFileSymbol()
{
	if(address)
		delete address;
}

//-NAME---------------------------------
//.DESCRIPTION..........................
//.PARAMETERS...........................
//.RETURN.CODES.........................
//--------------------------------------
extern SkyMockInterface g_mockInterface;
void MapFileSymbol::setData(char	*p_name, 
							char	*p_address, 
							char	*p_rvabase, 
							int		flagFunction, 
							int		flagInline, 
							char	*p_module)
{
	// function/object name and DLL/EXE/LIB/OBJ name
	if (p_name != nullptr && strlen(p_name) > 0)
	{
		name = p_name;
		//name = name.trimright();
	}
	else
		name = "";

	if (p_module != nullptr && strlen(p_name) > 0)
	{
		module = p_module;
		//module = module.trimright();
	}
	else
		module = "";
	
	// section/address

	if (address != nullptr)
	{
		delete address;
		address = nullptr;
	}

	address = new MapFileAddress(p_address);
	
	// inline / function flags

	isFunction = flagFunction;
	isInline = flagInline;

	// rva + base

	if (p_rvabase != NULL)
		rvaBase = (DWORD)strtoul(p_rvabase, NULL, 16);

	// now analyse this data to set the other flags

	int			len;
	const char	*mptr;

	len = module.length();
	mptr = p_module;
	
	if (len > 4)
	{
		const char	*mptr2;

		mptr2 = mptr + ((strlen(mptr) - 4) * sizeof(char));

		isObject = (stricmp(mptr2, ".obj") == 0);
		isLibraryObject = (stricmp(mptr2, ".obj") == 0) && strchr(mptr, ':');
		isDLLObject = (stricmp(mptr2, ".dll") == 0);

		isImported = (isLibraryObject || isDLLObject);// && !isObject;
		isExported = isObject && !isLibraryObject && !isDLLObject;
	}
	else
	{
		isImported = FALSE;
		isExported = FALSE;
	
		isObject = FALSE;
		isLibraryObject = FALSE;
		isDLLObject = FALSE;
	}
	
	// extras for C++
	
	isCPP = (name.length() > 0 && name[0] == '?');
	isDestructor = (name.compare("??_E") == 0) ||	// is ??_E destructor
					(name.compare("??_G") == 0);	// is ??_G scalar destructor

	// CRT runtime start with underscore prior to linking, hence have two underscores

	isCRT = (name.length() > 1 &&
			name[0] == '_' &&
			name[1] == '_');
}

