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

//-NAME---------------------------------
// Undecorate.cpp
//.DESCRIPTION..........................
// Part of the MapFileReader application/DLL
// To be able to read a VC++ 6.0 map file
//
// My own undecorating functions so that I don't have to rely
// on imagehlp.dll
//--------------------------------------


#include "undecorate.h"
#include "string.h"
#include "sprintf.h"
#include "MapFile.h"

std::string Right(std::string& strText, int nRight)
{
	if (nRight < 0)
	{
		return "";
	}
	std::string str;
	size_t size = strText.length();
	const char* p = strText.c_str() + size;
	for (size_t i = 0; (i != nRight) && (i != size); ++i, --p)
	{
		// nothing
	}
	str = p;
	return str;
}

int Find(std::string& strText, char value)
{
	for (int i = 0; i < strText.length(); i++)
	{
		if (strText[i] == value)
			return i;
	}

	return -1;
}

std::string Mid(std::string& strText, int start, int len)
{
	if (start + len >= strText.length())
		return "";

	std::string str;
	const char* p = strText.c_str() + start;
	char buf[256];
	size_t i = 0;
	for (i = 0; i < len; i++)
	{
		buf[i] = p[i];
	}

	buf[i] = 0;
	str = buf;

	return str;
}

std::string Left(std::string& strText, int len)
{
	if (len >= strText.length())
		return "";

	std::string str;
	char buf[256];
	const char* p = strText.c_str();
	size_t i = 0;
	for (i = 0; i < len; i++)
	{
		buf[i] = p[i];
	}
	
	buf[i] = 0;
	str = buf;

	return str;
}

// for use in undecorating

static void processParams(std::string	&params, 
	std::string	&returnType,
	std::string	&arguments);

static void decodeCPPType(char		**p, 
	std::string	&type);


//-NAME---------------------------------
//.DESCRIPTION..........................
//
// rules:-
// ?
// FunctionName
// @
// @
// function type (__cdecl etc)
// return type
// param types
// optional '@' don't know why - yet
// Z
//
// Don't know how:-
//  arrays are declared
//  default values declared
//  non intrinsic types are handled (structures, unions, classes)
//  __stdcall, __fastcall, PASCAL are declared
//
// types are:-
// YA __cdecl
// QAE __thiscall	C++
// X   void
// H   int
// F   short
// J   long
// C   signed char
// D   char
// I   unsigned int
// G   unsigned short
// K   unsigned long
// E   unsigned char
// M   float
// N   double
// PAtype   pointer (PAD is char *) (PAH is int*)
// PAX void *
// PAC signed char *
// PAD char *
// PAM float *
// PAN double *
// PAH int *
// PAF short *
// PAJ long *
// PAE unsigned char *
// PAC signed char *
// PAI unsigned int *
// PAH signed int *
// PAG unsigned short *
// PAF signed short *
// PAK unsigned long *
// PAJ signed long *
// Z   finished
//
// YA	__cdecl	
// QAE   __thiscall
//		__stdcall
//		__fastcall
//		PASCAL
//
//.PARAMETERS...........................
//.RETURN.CODES.........................
//--------------------------------------

static void processParams(std::string	&params,
	std::string	&returnType,
	std::string	&arguments)
{
	int		len, i;
	char	*p = (char *)(const char *)params;

	len = params.length();
	i = 0;

	// step past '@' symbols to return type

	do
	{
		if (params[i] == '@')
		{
			i++;
			p++;
		}
		else
			break;
	}
	while(i < len);

	// next two chars are calling convention

	std::string	cc;
	std::string	rt1, rt2;

	cc = Mid(params, i, 2);
	if (cc.compare("YA") == 0)
	{
		rt1 = " __cdecl ";
		p += 2;
		i += 2;
	}
	else if (cc.compare("QAE") == 0)
	{
		rt1 = " __thiscall ";
		p += 3;
		i += 3;
	}

	// next params are return type

	decodeCPPType(&p, rt2);
	returnType = rt2 + rt1;

	// remainder are function arguments

	int	gotOne = FALSE;

	arguments = "";
	while(TRUE)
	{
		if (*p == '\0')		// no more text
			break;
		else if (*p == 'Z')	// Z means end of the list
			break;
		else if (*p == '@')	// ignore
			p++;
		else				// process next parameters
		{
			std::string	pt;

			if (gotOne)
				arguments += ", ";

			decodeCPPType(&p, pt);
			arguments += pt;

			gotOne = TRUE;
		}
	}
}

//-NAME---------------------------------
//.DESCRIPTION..........................
//.PARAMETERS...........................
// YA __cdecl
// QAE __thiscall	?method@class@paramsZ
// X   void
// H   int
// F   short
// J   long
// C   signed char
// D   char
// I   unsigned int
// G   unsigned short
// K   unsigned long
// E   unsigned char
// M   float
// N   double
// PAtype   pointer (PAD is char *) (PAH is int*)
// PAX void *
// PAC signed char *
// PAD char *
// PAM float *
// PAN double *
// PAH int *
// PAF short *
// PAJ long *
// PAE unsigned char *
// PAC signed char *
// PAI unsigned int *
// PAH signed int *
// PAG unsigned short *
// PAF signed short *
// PAK unsigned long *
// PAJ signed long *
// Z   finished
//
// YA	__cdecl	
// QAE   __thiscall
//		__stdcall
//		__fastcall
//		PASCAL
//.RETURN.CODES.........................
//--------------------------------------

typedef struct _decodeType
{
	char	character;
	char	*text;
} DECODE_TYPE;

static DECODE_TYPE decodeArray[] =
{
	{'X', "void"},
	{'H', "int"},
	{'F', "short"},
	{'J', "long"},
	{'C', "signed char"},
	{'D', "char"},
	{'I', "unsigned int"},
	{'G', "unsigned short"},
	{'K', "unsigned long"},
	{'E', "unsigned char"},
	{'M', "float"},
	{'N', "double"}
};

static void decodeCPPType(char		**pp,
	std::string	&type)
{
	char	*p = *pp;
	std::string	t;

	type = "";
	if (strcmp(p, "PA") == 0)			// pointer
	{
		t = " *";
		p += 2;
	}

	// get actual value

	char	c;
	int		i, n;

	c = *p;
	p++;

	n = sizeof(decodeArray) / sizeof(decodeArray[0]);
	for(i = 0; i < n; i++)
	{
		if (c == decodeArray[i].character)
		{
			type = decodeArray[i].text + t;
			break;
		}
	}

	// return update pointer

	*pp = p;
}

//-NAME---------------------------------
//.DESCRIPTION..........................
// Trivial implementation of this so that can get
// away from imagehlp library.
//.PARAMETERS...........................
//.RETURN.CODES.........................
//--------------------------------------

std::string GetClassName(std::string& strText, int start)
{
	if (start >= strText.length())
		return "";

	std::string str;
	const char* p = strText.c_str() + start;
	char buf[256];
	size_t i = 0;
	for (i = 0; i < strlen(p); i++)
	{
		if (p[i] == '@')
			break;

		buf[i] = p[i];
	}

	buf[i] = 0;
	str = buf;

	return str;
}

bool MapFileReader::SimpleUndecorateCPP(char* inSymbol, char* outSymbol, int outLen)
{
	std::string out;
	std::string className;

	std::string in = inSymbol;

	if (strlen(inSymbol) == 0)
		return false;

	if (inSymbol[0] == '?')
	{
		// C++
		// if the name is prefixed with ? its a C++ name
		// remove the ? and then remove all from the @ symbol onwards

		out = Right(in, in.length() - 1);

		int	idx;

		idx = Find(out, '@');
		if (idx != -1)
		{
			className = GetClassName(out, idx + 1);
			out = Left(out, idx);
		}
	}
	else if (in[0] == '_')
	{
		// C
		// if the name starts with an underscore, its a C name, 
		// remove it (just the one underscore)

		out = Right(in, in.length() - 1);
	}

	if (className.length() > 0)
	{
		out = className + "::" + out;
	}

	if (out.length() > outLen)
		return false;

	strcpy(outSymbol, out.c_str());
	return true;
}

//-NAME---------------------------------
//.DESCRIPTION..........................
// Own implementation of this so that can get
// away from imagehlp library. This is not a full
// decoding, but its not a bad attempt for now...
//
// See MapDLL project for a lot of simple dummy functions so that I
// could determine argument type mapping.
//.PARAMETERS...........................
//.RETURN.CODES.........................
//--------------------------------------

bool MapFileReader::UndecorateCPP(char* inSymbol, char* outSymbol, int outLen)
{
	std::string out;
	out = "";

	std::string in = inSymbol;

	if (strlen(inSymbol) == 0)
		return false;

	if (in.length() == 0)
		return false;

	if (in[0] == '?')
	{
		// C++
		// if the name is prefixed with ? its a C++ name
		// remove the ? and then remove all from the @ symbol onwards

		out = Right(in, in.length() - 1);

		int	idx;

		idx = Find(out, '@');
		if (idx != -1)
		{
			std::string	params;
			std::string	func, arguments, returnType;

			params = Right(out, in.length() - idx - 1);
			func = Left(out, idx);

			// now process the params

			processParams(params, returnType, arguments);
			out = returnType;
			out += func;
			out += "(";
			out += arguments;
			out += ")";
		}
	}
	else if (in[0] == '_')
	{
		// C
		// if the name starts with an underscore, its a C name, 
		// remove it (just the one underscore)

		out = Right(in, in.length() - 1);
	}

	if (out.length() > outLen)
		return false;
	
	strcpy(outSymbol, out.c_str());

	return true;
}
