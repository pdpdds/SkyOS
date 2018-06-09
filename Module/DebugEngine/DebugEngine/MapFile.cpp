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

#include "MapFile.h"
#include "MapFileSection.h"
#include "MapFileSymbol.h"
#include "MapFileImport.h"
#include "MapFileExport.h"
#include "MapFileLineNumber.h"
#include "MapFileLineNumberFile.h"
#include "MapFileAddress.h"
#include "MapFileFixup.h"
#include "string.h"
#include "ctype.h"
#include <stdio.h>

extern SkyMockInterface g_mockInterface;

#define LINE_LENGTH 1024		// should be more than enough
#define DEFAULT_VERBOSE TRUE	// set to TRUE for debugging

#if _UNICODE
#define tcs_fopen _wfopen
//#define fgets fgetws
#else
#define tcs_fopen fopen
//#define fgets fgets
#endif

//-NAME---------------------------------
//.DESCRIPTION..........................
//.PARAMETERS...........................
//.RETURN.CODES.........................
//--------------------------------------

MapFileReader::MapFileReader(SKY_FILE_Interface fileInterface, char	*fileName)
{
	verbose = DEFAULT_VERBOSE;

	maxAddress = NULL;
	entryPoint = NULL;
	preferredLoadAddress = NULL;
	actualLoadAddress = NULL;

	readFile(fileName);
}

//-NAME---------------------------------
//.DESCRIPTION..........................
//.PARAMETERS...........................
//.RETURN.CODES.........................
//--------------------------------------

MapFileReader::MapFileReader()
{
	verbose = DEFAULT_VERBOSE;

	maxAddress = NULL;
	entryPoint = NULL;
	preferredLoadAddress = NULL;
	actualLoadAddress = NULL;

}

//-NAME---------------------------------
//.DESCRIPTION..........................
//.PARAMETERS...........................
//.RETURN.CODES.........................
//--------------------------------------

MapFileReader::~MapFileReader()
{
	flush();
}

//-NAME---------------------------------
//.DESCRIPTION..........................
//.PARAMETERS...........................
//.RETURN.CODES.........................
//--------------------------------------

void MapFileReader::flush()
{
	delete entryPoint;
	entryPoint = NULL;
	
	flushSections();
	flushPublicSymbols();
	flushStaticSymbols();
	flushImports();
	flushExports();
	flushLineNumberFiles();
	flushFixups();
}

//-NAME---------------------------------
//.DESCRIPTION..........................
//.PARAMETERS...........................
//.RETURN.CODES.........................
//--------------------------------------

void MapFileReader::flushSections()
{
	int	i, n;

	n = sections.size();
	for(i = 0; i < n; i++)
	{
		MapFileSection	*mfs;

		mfs = (MapFileSection *)sections[i];
		delete mfs;
	}
	sections.clear();
}

//-NAME---------------------------------
//.DESCRIPTION..........................
//.PARAMETERS...........................
//.RETURN.CODES.........................
//--------------------------------------

void MapFileReader::flushPublicSymbols()
{
	flushSymbols(publicSymbols);
}

//-NAME---------------------------------
//.DESCRIPTION..........................
//.PARAMETERS...........................
//.RETURN.CODES.........................
//--------------------------------------

void MapFileReader::flushStaticSymbols()
{
	flushSymbols(staticSymbols);
}

//-NAME---------------------------------
//.DESCRIPTION..........................
//.PARAMETERS...........................
//.RETURN.CODES.........................
//--------------------------------------

void MapFileReader::flushSymbols(std::vector<MapFileSymbol*>	&syms)
{
	int	i, n;

	n = syms.size();
	for(i = 0; i < n; i++)
	{		
		MapFileSymbol* mfs = syms[i];
		delete mfs;
	}
	syms.clear();
}

//-NAME---------------------------------
//.DESCRIPTION..........................
//.PARAMETERS...........................
//.RETURN.CODES.........................
//--------------------------------------

void MapFileReader::flushImports()
{
	int	i, n;

	n = imports.size();
	for(i = 0; i < n; i++)
	{
		MapFileImport	*mfs;

		mfs = (MapFileImport *)imports[i];
		delete mfs;
	}
	imports.clear();
}

//-NAME---------------------------------
//.DESCRIPTION..........................
//.PARAMETERS...........................
//.RETURN.CODES.........................
//--------------------------------------

void MapFileReader::flushExports()
{
	int	i, n;

	n = exports.size();
	for(i = 0; i < n; i++)
	{
		MapFileExport	*mfs;

		mfs = (MapFileExport *)exports[i];
		delete mfs;
	}
	exports.clear();
}

//-NAME---------------------------------
//.DESCRIPTION..........................
//.PARAMETERS...........................
//.RETURN.CODES.........................
//--------------------------------------

void MapFileReader::flushLineNumberFiles()
{
	int	i, n;

	n = lineNumberFiles.size();
	for(i = 0; i < n; i++)
	{
		MapFileLineNumberFile	*mfs;

		mfs = (MapFileLineNumberFile *)lineNumberFiles[i];
		delete mfs;
	}
	lineNumberFiles.clear();
}

//-NAME---------------------------------
//.DESCRIPTION..........................
//.PARAMETERS...........................
//.RETURN.CODES.........................
//--------------------------------------

void MapFileReader::flushFixups()
{
	int	i, n;

	n = fixups.size();
	for(i = 0; i < n; i++)
	{
		MapFileFixup	*mfs;

		mfs = (MapFileFixup *)fixups[i];
		delete mfs;
	}
	fixups.clear();
}

//-NAME---------------------------------
//.DESCRIPTION..........................
// Method to allow user to get information relating to nearest
// code location (file/function/line) to an address. This allows
// crash locations and/or functions to be found by address.
//
// See MSJ Oct 99, page 92 for a description of how to do this.
// Using test address, test if between preferred load address
// and maximum public symbol address. If so, continue, else must
// be in map file for a different dll/exe.
//
// Scan public symbol rva+base data for highest address that is
// below the test address. Now perform the following sum:-
//
// offset = (test address - preferred load address) - 0x1000
//
// (0x1000 is data offset in PE header).
//
// offset is now the byte offset into the file. Now scan the line
// number data for the line with the highest address offset that 
// is below the calculated offset.
// 
//.PARAMETERS...........................
// address					-in-	Address to test
// alternateLoadAddress		-in-	Alternate address of DLL if DLL couldn't be loaded at
//									preferred load address
// useAlternateLoadAddress	-in-	TRUE if should use alternate address
// module					-out-	Name of the DLL/OBJ/LIB/EXE that holds the function
// fileName					-out-	Name of file which holds the function
// lineNumber				-out-	Line in the source file of the function
// function					-out-	Name of the function
// resultAddress			-out-	The actual address of the function
//.RETURN.CODES.........................
// TRUE		Valid results
// FALSE	Not valid results
//--------------------------------------

int MapFileReader::getAddressInfo(DWORD		testAddress,
								  DWORD		alternateLoadAddress,
								  int		useAlternateLoadAddress,
								  std::string	&module,
	std::string	&fileName,
								  int		&lineNumber,
	std::string	&function,
								  DWORD		&resultAddress)
{
	DWORD			address = testAddress;
	DWORD			offset = NULL;
	DWORD			matchingRvaBase = NULL;
	MapFileSymbol	*matchingSymbol = NULL;
	
	if (useAlternateLoadAddress)
	{
		offset = address - alternateLoadAddress - 0x1000;

		// now move address back into range of map file, so that symbol mapping works

		address -= alternateLoadAddress;
		address += preferredLoadAddress;
	}
	else
		offset = address - preferredLoadAddress - 0x1000;

	// setup so as not to have found anything

	module = "";
	fileName = "";
	function = "";
	lineNumber = 0;				// not a valid line number

	// scan public symbols

	int				numSymbols;
	int				i;
	MapFileSymbol	*mfs = NULL;

	numSymbols = getNumPublicSymbols();
	for(i = 0; i < numSymbols; i++)
	{
		DWORD			rva;

		mfs = getPublicSymbol(i);
		rva = mfs->getRVABase();
		if (rva > address)
			break;

		// get data for this function, it may be this one, or the next one

		matchingSymbol = mfs;
		matchingRvaBase = rva;
	}

	// check if we scanned past the end of the array

	if (i == numSymbols && mfs == matchingSymbol && mfs != NULL)
	{
		DWORD	rva;
		
		rva = mfs->getRVABase();
		if (address > rva)
		{
			// we're looking for something that is outside this map file
			// reset the data

			matchingSymbol = NULL;
			matchingRvaBase = NULL;
		}
	}

	// scan private symbols - see if we can get a better match for function name

	mfs = NULL;
	numSymbols = getNumStaticSymbols();
	for(i = 0; i < numSymbols; i++)
	{
		DWORD			rva;

		mfs = getStaticSymbol(i);
		rva = mfs->getRVABase();
		if (rva > address)
			break;

		// only accept this if this is closer to the intended address than
		// what we have already got

		if (rva < matchingRvaBase)
			continue;

		// get data for this function, it may be this one, or the next one

		matchingSymbol = mfs;
	}

	// check if we scanned past the end of the array

	if (i == numSymbols && mfs == matchingSymbol && mfs != NULL)
	{
		DWORD	rva;

		rva = mfs->getRVABase();
		if (address > rva)
		{
			// we're looking for something that is outside this map file
			// reset the data

			matchingSymbol = NULL;
			matchingRvaBase = NULL;
			resultAddress = NULL;
		}
	}

	// get the data for the matched symbol 

	if (matchingSymbol != NULL)
	{
		char funcName[256];
		matchingSymbol->getName(funcName);
		function = funcName;
		matchingSymbol->getModule(module);
		resultAddress = matchingSymbol->getRVABase();
	}
	else
		return FALSE;

/*
	WORD	sectionNumber = 0;

	if (matchingSymbol != NULL)
	{
		MapFileAddress	*mfa;
		DWORD			addr;

		mfa = matchingSymbol->getAddress();
		if (mfa != NULL)
			mfa->getAddress(sectionNumber, addr);
	}
*/

	// now scan the line numbers, module by module

	DWORD	offsetDiff = 0x7ffffff;
	int		numLineNumberFiles;

	numLineNumberFiles = getNumLineNumberFiles();
	for(i = 0; i < numLineNumberFiles; i++)
	{
		MapFileLineNumberFile	*lnf;
		int						j, k;

		lnf = getLineNumberFile(i);

		// scan each line number for this file

		k = lnf->getNumLineNumbers();
		for(j = 0; j < k; j++)
		{
			MapFileLineNumber	*mfln;
			MapFileAddress		*mfa;

			mfln = lnf->getLineNumber(j);

			// check the offset for this line number

			mfa = mfln->getLineNumberAddress();
			if (mfa != NULL)
			{
				WORD	sectionNumber = 0;
				DWORD	addressOffset = 0;

				mfa->getAddress(sectionNumber, addressOffset);

				// only matches that are less than the offset are valid

				if (addressOffset <= offset)
				{
					DWORD	off;

					off = offset - addressOffset;
					if (off < offsetDiff)
					{
						// closer match, setup data

						offsetDiff = off;

						lineNumber = mfln->getLineNumber();
						lnf->getName(fileName);
					}
				}
			}
		}
	}

	// if we have a valid line number etc then all went OK

	return TRUE;
}

//-NAME---------------------------------
//.DESCRIPTION..........................
// Method to allow user to get information relating to nearest
// code location (file/function/line) to an address. This allows
// crash locations and/or functions to be found by address.
//
// See MSJ Oct 99, page 92 for a description of how to do this.
// Using test address, test if between preferred load address
// and maximum public symbol address. If so, continue, else must
// be in map file for a different dll/exe.
//
// Scan public symbol rva+base data for highest address that is
// below the test address. Now perform the following sum:-
//
// offset = (test address - preferred load address) - 0x1000
//
// (0x1000 is data offset in PE header).
//
// offset is now the byte offset into the file. Now scan the line
// number data for the line with the highest address offset that 
// is below the calculated offset.
// 
// CAUTION! This version of getAddressInfo() should only be used if
//          you have already set the actual load address, using
//			setActualLoadAddress() to a load address other than NULL
//
//.PARAMETERS...........................
// address					-in-	Address to test
// module					-out-	Name of the DLL/OBJ/LIB/EXE that holds the function
// fileName					-out-	Name of file which holds the function
// lineNumber				-out-	Line in the source file of the function
// function					-out-	Name of the function
// resultAddress			-out-	The actual address of the function
//.RETURN.CODES.........................
// TRUE		Valid results
// FALSE	Not valid results
//--------------------------------------

int MapFileReader::getAddressInfo(DWORD		testAddress,
	char* module,
	char* fileName,
								  int		&lineNumber,
	char* function,
								  DWORD		&resultAddress)
{
	std::string l_module;
	std::string l_fileName;
	std::string l_function;

	if (actualLoadAddress == NULL)
		return FALSE;

	bool result = getAddressInfo(testAddress, 
						  actualLoadAddress, TRUE,
		l_module, l_fileName, lineNumber, l_function, resultAddress);

	if (result == false)
		return false;

	strcpy(module, l_module.c_str());

	if(l_fileName.length() != 0)
		strcpy(fileName, l_fileName.c_str());
	strcpy(function, l_function.c_str());


	return true;
}

//-NAME---------------------------------
//.DESCRIPTION..........................
//.PARAMETERS...........................
//.RETURN.CODES.........................
//--------------------------------------

int MapFileReader::getSymbolInfo(DWORD		offset, 
	std::string	&symbolName,
								 int		partialMatch,
	std::string	&module,
	std::string	&fileName,
								 int		&lineNumber,
	std::string	&function,
								 DWORD		&resultAddress)
{
	MapFileSymbol	*symbol = NULL;
	int				r = FALSE;

	// try to find symbol in public symbols

	int				numSymbols;
	int				i;
	MapFileSymbol	*mfs = NULL;

	numSymbols = getNumPublicSymbols();
	for(i = 0; i < numSymbols; i++)
	{
		mfs = getPublicSymbol(i);
		if (mfs != NULL)
		{
			std::string	s;
			char funcName[256];
			mfs->getName(funcName);
			s = funcName;
			if (partialMatch)
			{
				// partial match

				char	*ptr;

				ptr = strstr(s, symbolName);
				if (ptr != NULL)
				{
					symbol = mfs;
					break;
				}
			}
			else
			{
				// exact match

				if (strcmp(symbolName, s) == 0)
				{
					symbol = mfs;
					break;
				}
			}
		}
	}

	// if not found try to find symbol in static symbols

	if (symbol == NULL)
	{
		mfs = NULL;
		numSymbols = getNumStaticSymbols();
		for(i = 0; i < numSymbols; i++)
		{
			mfs = getStaticSymbol(i);
			if (mfs != NULL)
			{
				std::string	s;
				char funcName[256];
				mfs->getName(funcName);
				s = funcName;
				if (partialMatch)
				{
					// partial match

					char	*ptr;

					ptr = strstr(s, symbolName);
					if (ptr != NULL)
					{
						symbol = mfs;
						break;
					}
				}
				else
				{
					// exact match

					if (strcmp(symbolName, s) == 0)
					{
						symbol = mfs;
						break;
					}
				}
			}
		}
	}

	// if not found, then bail out

	if (symbol == NULL)
		return FALSE;

	// add offset to get real address

	DWORD			address = 0;
	DWORD			rvaBase;
//	WORD			section;
//	MapFileAddress	*mfa;

//	mfa = symbol->getAddress();
//	mfa->getAddress(section, address);

	rvaBase = symbol->getRVABase();

	address += rvaBase;
	address += offset;

	// now get the real address info, using the preferred load address as the load
	// address doesn't matter if we are doing offset from a symbol

	return getAddressInfo(address,
						  NULL,				// alternateLoadAddress,
						  FALSE,			// useAlternateLoadAddress,
						  module,
						  fileName,
						  lineNumber,
						  function,
						  resultAddress);
}

//-NAME---------------------------------
//.DESCRIPTION..........................
//.PARAMETERS...........................
//.RETURN.CODES.........................
//--------------------------------------

int MapFileReader::readFile(char	*fileName)
{

	FILE	*fp;
	int		r = TRUE;
	
	// open the map file
	fp = g_mockInterface.g_fileInterface.sky_fopen(fileName, "r");
	if (fp != NULL)
	{
		
		
		r = readDLLName(fp);
		
		if (r)
			r = readTimeStamp(fp);
		
		if (r)
			r = readPreferredLoadAddress(fp);

		if (r)
			r = readSectionInformation(fp);
		
		if (r)
			r = readPublicSymbols(fp);
		
		if (r)
			r = readEntryPoint(fp);

		// from here, we may get the remaining data in any order...
		// so we'll need to rewrite this code to handle that possibility
		
		while(r)
		{
			char	line[LINE_LENGTH];
			char	*p = NULL;

			p = getNonBlankLine(fp, line, LINE_LENGTH);
			if (p == NULL)		// getting a duff line here is OK, since this loop
				break;			// will always run whilst things are going OK

			if (strstr(p, "Line numbers for") != NULL)
				r = readLineNumbers(p, fp);
			else if (strstr(p, "Exports") != NULL)
				r = readExports(p, fp);
			else if (strstr(p, "Static symbols") != NULL)
				r = readStaticSymbols(p, fp);
			else if (strstr(p, "FIXUPS:") == p)		// at start of line
				r = readFixup(p);
		}
/*
		if (r)
			r = readLineNumbers(fp);
		
		if (r)
			r = readExports(fp);

		if (r)
			r = readStaticSymbols(fp);

		if (r)
			r = readFixups(fp);
*/
		// close map file
	
		if (verbose)
		{
			if (r)
				g_mockInterface.g_printInterface.sky_printf("Parsing C and C++ decorated names complete\n");
			else
				g_mockInterface.g_printInterface.sky_printf("Parsing C and C++ decorated names failed\n");
		}
		g_mockInterface.g_fileInterface.sky_fclose(fp);
	}
	else
	{
		if (verbose)
			g_mockInterface.g_printInterface.sky_printf("Failed to open map file %s\n", fileName);

		r = FALSE;
	}

	g_mockInterface.g_printInterface.sky_printf("success to open map file %s\n", fileName);

	return r;
}

//-NAME---------------------------------
//.DESCRIPTION..........................
//.PARAMETERS...........................
//.RETURN.CODES.........................
//--------------------------------------

char *MapFileReader::skipWhiteSpace(char	*p,
									 char	mustSkip)
{
	int	found = (mustSkip == '\0');

	while(TRUE)
	{
		if (!found)
		{
			if (*p == mustSkip)
				found = TRUE;
			p++;
		}
		else if (isspace(*p))
			p++;
		else
			break;
	}

	return p;
}

//-NAME---------------------------------
//.DESCRIPTION..........................
//.PARAMETERS...........................
//.RETURN.CODES.........................
//--------------------------------------

char *MapFileReader::findWhiteSpace(char	*p,
									 char	mustSkip)
{
	int	found = (mustSkip == '\0');

	while(TRUE)
	{
		if (!found)
		{
			if (*p == mustSkip)
				found = TRUE;
			p++;
		}
		else if (!isspace(*p))
			p++;
		else
			break;
	}

	return p;
}

//-NAME---------------------------------
//.DESCRIPTION..........................
//.PARAMETERS...........................
//.RETURN.CODES.........................
//--------------------------------------

int MapFileReader::isBlankLine(char	*p)
{
	while(*p != '\0')
	{
		if (isspace(*p))
			p++;
		else
			return FALSE;
	}

	return TRUE;
}

//-NAME---------------------------------
//.DESCRIPTION..........................
//.PARAMETERS...........................
//.RETURN.CODES.........................
//--------------------------------------

char *MapFileReader::getNextWord(char		*p,
								  std::string &s,
								  char		mustSkip)
{
	char	*p1;
	char	*p2;
	char	c;

	p1 = skipWhiteSpace(p);
	if (p1 == NULL)
		return p1;

	p2 = findWhiteSpace(p1, mustSkip);
	if (p2 == NULL)
		return p2;

	// get the word

	c = *p2;
	*p2 = '\0';
	s = p1;
	*p2 = c;

	// return new position

	return p2;
}

//-NAME---------------------------------
//.DESCRIPTION..........................
//.PARAMETERS...........................
//.RETURN.CODES.........................
//--------------------------------------

int MapFileReader::getNextWord(FILE		*fp,
	std::string	&s)
{
	char	line[LINE_LENGTH];
	char	*p = NULL;

	p = getNonBlankLine(fp, line, LINE_LENGTH);
	if (p == NULL)
		return FALSE;

	p = getNextWord(p, s);

	return p != NULL;
}

//-NAME---------------------------------
//.DESCRIPTION..........................
//.PARAMETERS...........................
//.RETURN.CODES.........................
//--------------------------------------

char *MapFileReader::getNonBlankLine(FILE	*fp,
								      char	*line,
								      int	len)
{
	char	*p = nullptr;

	// get next line that isn't blank and get the first word on it
	
	do 
	{
		p = g_mockInterface.g_fileInterface.sky_fgets(line, (len - 1), fp);

		if (p == NULL)
		{
			// check if at end of file, if not, then we must be at a \n character

			if (g_mockInterface.g_fileInterface.sky_feof(fp) != 0)
				break;				// end of file

			// move past the \n character

			g_mockInterface.g_fileInterface.sky_fseek(fp, 1, SEEK_CUR);
		}
	} while(p == NULL || isBlankLine(p));

	return p;
}

//-NAME---------------------------------
//.DESCRIPTION..........................
//.PARAMETERS...........................
//.RETURN.CODES.........................
//--------------------------------------

int MapFileReader::readDLLName(FILE	*fp)
{
	return getNextWord(fp, mapObjectName);
}

//-NAME---------------------------------
//.DESCRIPTION..........................
//.PARAMETERS...........................
//.RETURN.CODES.........................
//--------------------------------------

int MapFileReader::readTimeStamp(FILE	*fp)
{
	char	line[LINE_LENGTH];
	char	*p;

	// timestamp is on third line, after any whitespace

	p = getNonBlankLine(fp, line, LINE_LENGTH);
	if (p == NULL)
		return FALSE;

	// skip first two words

	p = strstr(p, "Timestamp is ");
	if (p == NULL)
		return FALSE;

	p += strlen("Timestamp is ");

	// now at start of timestamp string

	timeStamp = p;
	//timeStamp = timeStamp.trimright();

	return TRUE;
}

//-NAME---------------------------------
//.DESCRIPTION..........................
//.PARAMETERS...........................
//.RETURN.CODES.........................
//--------------------------------------

int MapFileReader::readPreferredLoadAddress(FILE	*fp)
{
	char	line[LINE_LENGTH];
	char	*p;

	// timestamp is on fifth line, after any whitespace

	p = getNonBlankLine(fp, line, LINE_LENGTH);
	if (p == NULL)
		return FALSE;

	p = strstr(p, "Preferred load address is ");
	if (p == NULL)
		return FALSE;

	// now at start of preferred load address string

	p += strlen("Preferred load address is ");						// skip to numeric
	preferredLoadAddress = (DWORD)strtoul(p, NULL, 16);				// hex is base 16

	return TRUE;
}

//-NAME---------------------------------
//.DESCRIPTION..........................
//.PARAMETERS...........................
//.RETURN.CODES.........................
//--------------------------------------

int MapFileReader::readSectionInformation(FILE	*fp)
{
	char	line[LINE_LENGTH];
	char	*p;

	p = getNonBlankLine(fp, line, LINE_LENGTH);
	if (p == NULL)
		return FALSE;

	// check matches the expected line
	// get next four words, they should be, Start, Length, Name, Class

	std::string	wStart, wLength, wName, wClass;

	p = getNextWord(p, wStart);
	p = getNextWord(p, wLength);
	p = getNextWord(p, wName);
	p = getNextWord(p, wClass);

	if (strcmp(wStart, "Start") != 0)
		return FALSE;
	if (strcmp(wLength, "Length") != 0)
		return FALSE;
	if (strcmp(wName, "Name") != 0)
		return FALSE;
	if (strcmp(wClass, "Class") != 0)
		return FALSE;

	// read each line until get a blank line, each line has data about a particular section

	while(TRUE)
	{
		p = g_mockInterface.g_fileInterface.sky_fgets(line, (LINE_LENGTH - 1), fp);
		if (p == NULL)
			return FALSE;

		// finished sections when we get a blank line

		if (isBlankLine(p))
			return TRUE;

		// process line

		wStart = "";
		wLength = "";
		wName = "";
		wClass = "";
		p = getNextWord(p, wStart);
		p = getNextWord(p, wLength);
		p = getNextWord(p, wName);
		p = getNextWord(p, wClass);

		// build the section and add it to the collection of section information

		MapFileAddress	*mfa;
		MapFileSection	*mfs;
		DWORD			length;

		mfs = new MapFileSection(wName);
		mfa = new MapFileAddress(wStart);
		mfs->setAddress(mfa);

		length = (DWORD)strtoul(wLength, NULL, 16);
		mfs->setLength(length);

		mfs->setClassName(wClass);

		addSection(mfs);
	}

	return TRUE;
}

//-NAME---------------------------------
//.DESCRIPTION..........................
//.PARAMETERS...........................
//.RETURN.CODES.........................
//--------------------------------------

int MapFileReader::readPublicSymbols(FILE	*fp)
{
	char	line[LINE_LENGTH];
	char	*p;

	// search for a line with the following phrases on it:-
	//  Address
	//  Publics by Value
	//  Rva+Base
	//  Lib:Object
	// then step past a blank line and then read all symbols (some exported, some imported)

	if (verbose)
		g_mockInterface.g_printInterface.sky_printf("Searching for start of C and C++ exported names\n");

	p = getNonBlankLine(fp, line, LINE_LENGTH);
	if (p == NULL)
		return FALSE;

	// check line matches

	std::string	wAddress, wPublics, wBy, wValue, wRvaBase, wLibObject;

	p = getNextWord(p, wAddress);
	p = getNextWord(p, wPublics);
	p = getNextWord(p, wBy);
	p = getNextWord(p, wValue);
	p = getNextWord(p, wRvaBase);
	p = getNextWord(p, wLibObject);

	if (strcmp(wAddress, "Address") != 0)
		return FALSE;
	if (strcmp(wPublics, "Publics") != 0)
		return FALSE;
	if (strcmp(wBy, "by") != 0)
		return FALSE;
	if (strcmp(wValue, "Value") != 0)
		return FALSE;
	if (strcmp(wRvaBase, "Rva+Base") != 0)
		return FALSE;
	if (strcmp(wLibObject, "Lib:Object") != 0)
		return FALSE;

	// line matched, move to next line

	p = g_mockInterface.g_fileInterface.sky_fgets(line, (LINE_LENGTH - 1), fp);
	if (p == NULL)
		return FALSE;

	return readSymbols(fp, TRUE);
}

//-NAME---------------------------------
//.DESCRIPTION..........................
//.PARAMETERS...........................
//.RETURN.CODES.........................
//--------------------------------------

char *MapFileReader::getFlagInSpace(char	*p,
								     char	match,
									 int	*flag)
{
	// if not on a space don't advance

	*flag = FALSE;
	if (*p != ' ')
		return p;

	// char after place to test, must be a space, if not, don't advance

	if (*(p + 2) != ' ')
		return p;

	// char to test

	p++;
	*flag = (*p == match);
	p++;

	return p;
}

//-NAME---------------------------------
//.DESCRIPTION..........................
//.PARAMETERS...........................
//.RETURN.CODES.........................
//--------------------------------------

int MapFileReader::readSymbols(FILE	*fp, 
							   int	addToPublicSymbols)
{
	char	line[LINE_LENGTH];
	char	*p;

	// now read all symbol data
	// load lines, 1 line at a time.
	// each line has the following format:-
	//
	// 0001:00000000       _DllMain@12                10001000 f UTIL.OBJ
	//
	// space, address, spaces, decorated-name, spaces, rvaddress, space, flagFunction, space, flagInline, space, obj or module (dll) name
	// where address is section:address
	// where rvaddress is RVA+BASE
	// name is C++ if starts with a ?
	// flagFunction specifies f for function (I think - data doesn't have it)
	// flagInline specifies i for inline (not that if these are not present, the space and char
	//   for this are not present (the line is 2 chars shorter)
	// the decorated name is exported unless it has the prefix __imp__, except for the 
	// next condition. if the symbol is from an object file it is exported, if it is 
	// from a dll, a lib or an object file inside a dll or lib it is imported

	if (verbose)
		g_mockInterface.g_printInterface.sky_printf("Parsing C and C++ export names\n");
	
	while(TRUE)
	{
		p = g_mockInterface.g_fileInterface.sky_fgets(line, (LINE_LENGTH - 1), fp);
		if (p == NULL)
			return TRUE;

		// finished symbols when we get a blank line

		if (isBlankLine(p))
			return TRUE;

		// is this the correct line?
		
		// data we require is on first line
		// check that line starts with a space and a pointer address expressed as two hex words with a colon
		// " xxxx:xxxx",followed by one space, a name, an alias (often the same as the name)
		// the name and the alias may be a C name or a C++ name, we are only interested in the C names here
			
		if (line[0] == ' ' && 
			isxdigit(line[1]) &&
			isxdigit(line[2]) &&
			isxdigit(line[3]) &&
			isxdigit(line[4]) &&
			line[5] == ':' &&
			isxdigit(line[6]) &&
			isxdigit(line[7]) &&
			isxdigit(line[8]) &&
			isxdigit(line[9]) &&
			isxdigit(line[10]) &&
			isxdigit(line[11]) &&
			isxdigit(line[12]) &&
			isxdigit(line[13]) &&
			isspace(line[14]))
		{
			char	*address = NULL;
			char	*name = NULL;
			char	*rvabase = NULL;
			int		flagFunction = '\0';
			int		flagInline = '\0';
			char	*module = NULL;
			char	*pq;
			char	*pq2;

			// get address

			address = &line[1];
			line[14] = '\0';

			// first name is separated from second name by whitespace
			// skip spaces

			name = &line[15];
			while(isspace(*name))
				name++;
			if (*name == '\0')
				break;
				
			// terminate the name

			pq = strchr(name, ' ');
			if (*pq == '\0')
				break;
			*pq = '\0';
			
			// get the rva+base of the name

			rvabase = pq + 1;
			while(isspace(*rvabase))
				rvabase++;
			if (*rvabase == '\0')
				break;

			// terminate the address

			pq = strchr(rvabase, ' ');
			if (*pq == '\0')
				break;

			// restore the end of the string and remember address so that we can
			// terminate the rvabase string later (don't do it now, will break the
			// getFlagInSpace() functionality for certain map files)

			*pq = ' ';
			pq2 = pq;

			// get the flagFunction (space followed by (space or 'f') followed by space)

			pq = getFlagInSpace(pq, 'f', &flagFunction);

			// get the flagInline (space followed by (space or 'i') followed by space)

			pq = getFlagInSpace(pq, 'i', &flagInline);

			// terminate the rvabase string

			*pq2 = '\0';

			// get the object/module name

			module = pq;

			// now build a symbol entry and add to the list of symbols in the map

			MapFileSymbol	*mfs;

			mfs = new MapFileSymbol();
			mfs->setData(name, address, rvabase, flagFunction, flagInline, module);

			if (addToPublicSymbols)
				addPublicSymbol(mfs);
			else
				addStaticSymbol(mfs);

			// keep track of 'highest' address 

			MapFileAddress	*mfa;
			WORD			section;
			DWORD			addr;

			mfa = mfs->getAddress();
			mfa->getAddress(section, addr);
			if (addr > maxAddress)
				maxAddress = (DWORD)addr;

		}
		else
			return FALSE;	// error whilst parsing
	}

	return TRUE;
}

//-NAME---------------------------------
//.DESCRIPTION..........................
//.PARAMETERS...........................
//.RETURN.CODES.........................
//--------------------------------------

int MapFileReader::readEntryPoint(FILE	*fp)
{
	char	line[LINE_LENGTH];
	char	*p;

	p = getNonBlankLine(fp, line, LINE_LENGTH);
	if (p == NULL)
		return FALSE;

	// check line matches

	if (strstr(p, " entry point at") == NULL)
		return FALSE;

	p += strlen(" entry point at");
	p = skipWhiteSpace(p);

	// create new entry point

	delete entryPoint;
	entryPoint = new MapFileAddress(p);

	return TRUE;
}

//-NAME---------------------------------
//.DESCRIPTION..........................
//.PARAMETERS...........................
//.RETURN.CODES.........................
//--------------------------------------

int	MapFileReader::readFixups(FILE	*fp)
{
	char	line[LINE_LENGTH];
	char	*p;

	// all lines starting with FIXUPS:
	// then start with a 4 digit hex number and then a load of other numbers

	// find the line containing the word 'FIXUPS'

	p = getNonBlankLine(fp, line, LINE_LENGTH);
	if (p == NULL)
		return FALSE;

	do
	{
		int	r;

		if (strcmp(p, "FIXUPS:") != 0)
			return FALSE;

		r = readFixup(p);
		if (!r)
			return FALSE;

		p = g_mockInterface.g_fileInterface.sky_fgets(line, (LINE_LENGTH - 1), fp);
		if (p == NULL)
			return TRUE;	// running out of document is same as a blank line
	}
	while(!isBlankLine(p));

	return TRUE;
}

//-NAME---------------------------------
//.DESCRIPTION..........................
//.PARAMETERS...........................
//.RETURN.CODES.........................
//--------------------------------------

int	MapFileReader::readFixup(char	*p)
{
	// process this fixup
	// start of line is FIXUPS: xxxx followed by many hex numbers

	p += strlen("FIXUPS:");

	MapFileFixup	*mff;
	WORD			fixupValue;
	char			*p2 = NULL;

	fixupValue = (WORD)strtoul(p, &p2, 16);
	mff = new MapFileFixup(fixupValue);

	p = p2;
	while(p != NULL && !isBlankLine(p))
	{
		DWORD	fixup;

		p2 = NULL;
		fixup = (DWORD)strtoul(p, &p2, 16);
		p = p2;

		if (p2 != NULL)
			mff->addFixup(fixup);
	}
	addFixup(mff);

	return TRUE;
}

//-NAME---------------------------------
//.DESCRIPTION..........................
//.PARAMETERS...........................
//.RETURN.CODES.........................
//--------------------------------------

int MapFileReader::readLineNumbers(FILE	*fp)
{
	char	line[LINE_LENGTH];
	char	*p;

	p = getNonBlankLine(fp, line, LINE_LENGTH);
	if (p == NULL)
		return FALSE;

	return readLineNumbers(p, fp);
}

//-NAME---------------------------------
//.DESCRIPTION..........................
//.PARAMETERS...........................
//.RETURN.CODES.........................
//--------------------------------------

int MapFileReader::readLineNumbers(char	*p,
								   FILE		*fp)
{
	char	line[LINE_LENGTH];
	std::string	objectName;
	std::string	segmentName;

	if (strstr(p, "Line numbers for") != p)
		return FALSE;

	p += strlen("Line numbers for");
	p = getNextWord(p, objectName, ')');
	p = getNextWord(p, segmentName);	// always reads 'segment'
	p = getNextWord(p, segmentName);	// real segment name

	// build line number object and add line numbers to it

	MapFileLineNumberFile	*lnf;

	lnf = new MapFileLineNumberFile(objectName);
	lnf->setSegmentName(segmentName);

	addLineNumberFile(lnf);

	p = getNonBlankLine(fp, line, LINE_LENGTH);
	if (p == NULL)
		return FALSE;

	do
	{
		std::string	lineNum;
		std::string	sectionAddress;

		// get words in pairs - first is line, second is section:address

		do
		{
			p = getNextWord(p, lineNum);
			p = getNextWord(p, sectionAddress);

			lnf->addLineNumber(lineNum, sectionAddress);
		}
		while(!isBlankLine(p));

		// get next line

		p = g_mockInterface.g_fileInterface.sky_fgets(line, (LINE_LENGTH - 1), fp);
		if (p == NULL)
			return TRUE;	// running out of document is same as blank line
	}
	while(!isBlankLine(p));

	return TRUE;
}

//-NAME---------------------------------
//.DESCRIPTION..........................
//.PARAMETERS...........................
//.RETURN.CODES.........................
//--------------------------------------

int MapFileReader::readExports(FILE	*fp)
{
	char	line[LINE_LENGTH];
	char	*p;

	// find the line containing the word 'Exports'

	p = getNonBlankLine(fp, line, LINE_LENGTH);
	if (p == NULL)
		return FALSE;

	return readExports(p, fp);
}

//-NAME---------------------------------
//.DESCRIPTION..........................
//.PARAMETERS...........................
//.RETURN.CODES.........................
//--------------------------------------

int MapFileReader::readExports(char	*p,
							   FILE		*fp)
{
	char	line[LINE_LENGTH];

	// find the line containing the word 'Exports'

	if (strstr(p, " Exports") != p)
		return FALSE;

	// find the line containing the words 'ordinal' and 'name'

	p = getNonBlankLine(fp, line, LINE_LENGTH);
	if (p == NULL)
		return FALSE;

	std::string	wOrdinal, wName;

	p = getNextWord(p, wOrdinal);
	p = getNextWord(p, wName);
	if (strcmp(wOrdinal, "ordinal") != 0)
		return FALSE;
	if (strcmp(wName, "name") != 0)
		return FALSE;

	// read the exported data

	p = getNonBlankLine(fp, line, LINE_LENGTH);
	if (p == NULL)
		return FALSE;

	do
	{
		std::string	wO, wN1, wN2;
		DWORD	ordinal;

		// each line has two parts (second part is in two logical parts of many words)
		// part1:-	ordinal integer
		// part2:-	non-whitespace name followed by full definition in parentheses ()
		// examples:-
		//	1    ?MapDLLFunction@@YAHXZ (int __cdecl MapDLLFunction(void))
        //	2    ?MapDLLHappyFunc@@YAPADPAD@Z (char * __cdecl MapDLLHappyFunc(char *))

		p = getNextWord(p, wO);
		ordinal = (DWORD)strtoul(wO, NULL, 10);	// I think its base 10!
		p = getNextWord(p, wN1);
		
		p = skipWhiteSpace(p);
		wN2 = p;
		//wN2 = wN2.trimright();
		
		MapFileExport	*mfe;

		mfe = new MapFileExport();
		mfe->setName(wN1);
		mfe->setFullName(wN2);
		mfe->setOrdinal(ordinal);

		addExport(mfe);

		// get next line

		p = g_mockInterface.g_fileInterface.sky_fgets(line, (LINE_LENGTH - 1), fp);
		if (p == NULL)
			return TRUE;	// running out is the same as a blank line
	}
	while(!isBlankLine(p));

	return TRUE;
}

//-NAME---------------------------------
//.DESCRIPTION..........................
//.PARAMETERS...........................
//.RETURN.CODES.........................
//--------------------------------------

int MapFileReader::readStaticSymbols(FILE	*fp)
{
	char	line[LINE_LENGTH];
	char	*p;

	// search for a line with the following phrases on it:-
	//  Static
	//  symbols
	// then step past a blank line and then read all symbols (some exported, some imported)

	if (verbose)
		g_mockInterface.g_printInterface.sky_printf("Searching for start of C and C++ exported names\n");

	p = getNonBlankLine(fp, line, LINE_LENGTH);
	if (p == NULL)
		return FALSE;

	return readStaticSymbols(p, fp);
}

//-NAME---------------------------------
//.DESCRIPTION..........................
//.PARAMETERS...........................
//.RETURN.CODES.........................
//--------------------------------------

int MapFileReader::readStaticSymbols(char	*p,
									 FILE	*fp)
{
	char	line[LINE_LENGTH];

	// check line matches

	std::string	wStatic, wSymbols;

	p = getNextWord(p, wStatic);
	p = getNextWord(p, wSymbols);

	if (strcmp(wStatic, "Static") != 0)
		return FALSE;
	if (strcmp(wSymbols, "symbols") != 0)
		return FALSE;

	// line matched, move to next line, which may be zero length (in which case fgets will return NULL)

	p = g_mockInterface.g_fileInterface.sky_fgets(line, (LINE_LENGTH - 1), fp);
	if (p == NULL)
		return FALSE;

	return readSymbols(fp, FALSE);
}

//-NAME---------------------------------
//.DESCRIPTION..........................
//.PARAMETERS...........................
//.RETURN.CODES.........................
//--------------------------------------

void MapFileReader::setLoadAddress(DWORD	loadAddress)
{
	actualLoadAddress = loadAddress;
}

//-NAME---------------------------------
//.DESCRIPTION..........................
//.PARAMETERS...........................
//.RETURN.CODES.........................
//--------------------------------------

DWORD MapFileReader::getMinAddress()
{
	if (actualLoadAddress == NULL)
		return preferredLoadAddress;

	return actualLoadAddress;
}

//-NAME---------------------------------
//.DESCRIPTION..........................
//.PARAMETERS...........................
//.RETURN.CODES.........................
//--------------------------------------

DWORD MapFileReader::getMaxAddress()
{
	DWORD	dw;

	if (actualLoadAddress == NULL)
		dw = preferredLoadAddress;
	else
		dw = actualLoadAddress;

	//return dw + (maxAddress - preferredLoadAddress);
	return dw + maxAddress;
}

//-NAME---------------------------------
//.DESCRIPTION..........................
//.PARAMETERS...........................
//.RETURN.CODES.........................
//--------------------------------------

int MapFileReader::findSymbol(std::string	&symbol,
							  MapFileSymbol	**mfs)
{
	int				i, n;
	MapFileSymbol	*ms;
	std::string			symName;

	// search public symbols

	n = getNumPublicSymbols();
	for(i = 0; i < n; i++)
	{
		ms = getPublicSymbol(i);
		char buf[256];
		ms->getName(buf);
		symName = buf;
		if (symName.compare(symbol) == 0)
		{
			*mfs = ms;
			return TRUE;
		}
	}

	// search static symbols

	n = getNumStaticSymbols();
	for(i = 0; i < n; i++)
	{
		MapFileSymbol	*ms;
		
		ms = getStaticSymbol(i);
		char buf[256];
		ms->getName(buf);
		symName = buf;
		if (symName.compare(symbol) == 0)
		{
			*mfs = ms;
			return TRUE;
		}
	}

	// not found

	*mfs = NULL;
	return FALSE;
}

/*
// further processing that can classify some of the symbols a bit better

			int		isWep, isCPP, isUnderScore, isOBJ, isDLL, isDestructor, isImport;
			int		len;

			isDLL = ((strstr(module, ".DLL") != NULL) || (strstr(module, ".dll") != NULL));
			isOBJ = ((strstr(module, ".OBJ") != NULL) || (strstr(module, ".obj") != NULL));
			if (isOBJ)
			{
				// check that this object is not really in a DLL

				if (strstr(module, ":") != NULL)
				{
					isOBJ = FALSE;
					isDLL = TRUE;
				}
			}

			isDestructor = ((strncmp(name, "??_E", 4) == 0) ||	// is ??_E destructor
							(strncmp(name, "??_G", 4) == 0));	// is ??_G scalar destructor
			isImport = (strncmp(name, "__imp__", 7) == 0);

			// check that name is not 'WEP' or a C++ name or starts with more than 1 underscore
			
			isWep = (strcmp(name, "WEP") == 0);
			
			isCPP = name[0] == '?';
			len = strlen(name);
			isUnderScore = FALSE;
			if (len >= 3 &&	name[0] == '_' && name[1] == '_')	// modified to remove _tcsncpy etc
				isUnderScore = TRUE;
				
			if (verbose)
				printf("Potential C export: %s\n", name);

			if (!isWep && isOBJ && !isImport)
			{
				if (!isCPP && !isUnderScore)
				{
					if ((strncmp(name, "_ExtRawDllMain", 14) != 0) &&
						(strncmp(name, "_DllMain", 8) != 0))
					{
						if (verbose)
						{
							printf("C export: %s\n", name);
							printf("  address: %s\n", address);
							printf("  flag(Function == f): %s\n", flagFunction);
							printf("  flag(Inline == i): %s\n", flagInline);
							printf("  module: %s\n\n", module);
						}

						//AfxCheckMemory();
						addSymbol(name, A_C_DECLARATION);
						//AfxCheckMemory();
					}
				}
				else if (isCPP && !isUnderScore)
				{
					// if starts with ??_E then ignore it, it causes a ref we can't resolve at link time
					// for some reason

					if (!isDestructor)
					{
						if (verbose)
						{
							printf("C++ export: %s\n", name);
							printf("  address: %s\n", address);
							printf("  flag(Function == f): %s\n", flagFunction);
							printf("  flag(Inline == i): %s\n", flagInline);
							printf("  module: %s\n\n", module);
						}
												
						//AfxCheckMemory();
						addSymbol(name, A_PUBLIC);
						//AfxCheckMemory();
					}
				}
			}
*/
