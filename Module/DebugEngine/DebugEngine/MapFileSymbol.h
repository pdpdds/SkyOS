#ifndef _MAP_FILE_SYMBOL_H
#define _MAP_FILE_SYMBOL_H
#ifdef SKYOS32
#include "windef.h"
#include "stl_string.h"
#include "string.h"
#endif
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

class MapFileSymbol
{
public:
	MAPFILEDLL_API MapFileSymbol();

	MAPFILEDLL_API virtual ~MapFileSymbol();

	MAPFILEDLL_API virtual void setData(char	*name, 
										 char	*address, 
										 char	*rvabase, 
										 int	flagFunction, 
										 int	flagInline, 
										 char	*module);

	MAPFILEDLL_API void getName(char* p_name)
	{
		strcpy(p_name, name.c_str());
	}

	MAPFILEDLL_API void getModule(std::string	&p_name)
	{
		p_name = module;
	}

	MAPFILEDLL_API MapFileAddress *getAddress()
	{
		return address;
	}

	MAPFILEDLL_API DWORD getRVABase()
	{
		return rvaBase;
	}

	MAPFILEDLL_API int getIsFunction()
	{
		return isFunction;
	}

	MAPFILEDLL_API int getIsInLine()
	{
		return isInline;
	}

	MAPFILEDLL_API int getIsImported()
	{
		return isImported;
	}

	MAPFILEDLL_API int getIsExported()
	{
		return isExported;
	}

	MAPFILEDLL_API int getIsObject()
	{
		return isObject;
	}

	MAPFILEDLL_API int getIsLibraryObject()
	{
		return isLibraryObject;
	}

	MAPFILEDLL_API int getIsDLLObject()
	{
		return isDLLObject;
	}

	// extras for C++

	MAPFILEDLL_API int getIsCPP()
	{
		return isCPP;
	}

	MAPFILEDLL_API int getIsDestructor()
	{
		return isDestructor;
	}

	MAPFILEDLL_API int getIsCRT()
	{
		return isCRT;
	}

private:
	std::string			name;
	MapFileAddress	*address;
	DWORD			rvaBase;		// rva + base address
	
	int				isFunction;
	int				isInline;
	
	std::string			module;
	
	int				isImported;
	int				isExported;
	
	int				isObject;
	int				isLibraryObject;
	int				isDLLObject;

	// extras for C++

	int				isCPP;
	int				isDestructor;

	int				isCRT;
};

#endif
