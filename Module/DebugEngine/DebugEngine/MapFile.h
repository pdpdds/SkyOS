#ifndef _MAP_FILE_READER_H
#define _MAP_FILE_READER_H

#ifdef SKYOS32
#include "windef.h"
#include "stl_string.h"
#include "fileio.h"
#include "vector.h"
#elif WIN32
#include <string>
#include <vector>
#endif

#include "SkyMockInterface.h"
#include "I_MapFileReader.h"
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

class MapFileSection;
class MapFileSymbol;
class MapFileImport;
class MapFileExport;
class MapFileLineNumberFile;
class MapFileAddress;
class MapFileFixup;

class MapFileReader : public I_MapFileReader
{
public:
	MAPFILEDLL_API MapFileReader(SKY_FILE_Interface fileInterface, char	*fileName);

	MAPFILEDLL_API MapFileReader();

	MAPFILEDLL_API ~MapFileReader();

	MAPFILEDLL_API void flush();

	MAPFILEDLL_API DWORD getMinAddress();

	MAPFILEDLL_API DWORD getMaxAddress();

	MAPFILEDLL_API int getAddressInfo(DWORD	address,
									   DWORD	alternateLoadAddress,
									   int		useAlternateLoadAddress,
									   std::string	&module,
									   std::string	&fileName,
									   int		&lineNumber,
									   std::string	&function,
									   DWORD	&resultAddress);

	// caution! this version requires that you've set the load
	//          address using setLoadAddress()

	virtual bool MAPFILEDLL_API SimpleUndecorateCPP(char* in, char* out, int outLen) override;
	virtual bool MAPFILEDLL_API UndecorateCPP(char* in, char* out, int outLen) override;

	virtual MAPFILEDLL_API int getAddressInfo(DWORD	address,
							        	char* module,
		char* fileName,
									  int		&lineNumber,
		char* function,
									  DWORD		&resultAddress) override;

	MAPFILEDLL_API int getSymbolInfo(DWORD		offset, 
		std::string	&symbolName,
									 int		partialMatch,
		std::string	&module,
		std::string	&fileName,
									 int		&lineNumber,
		std::string	&function,
									 DWORD		&resultAddress);

	virtual MAPFILEDLL_API void setLoadAddress(DWORD	loadAddress) override;

	virtual MAPFILEDLL_API int readFile(char	*fileName) override;

	MAPFILEDLL_API void getTimeStamp(std::string	&s)
	{
		s = timeStamp;
	}

	MAPFILEDLL_API void getMapObjectName(std::string	&s)
	{
		s = mapObjectName;
	}

	virtual MAPFILEDLL_API unsigned int getPreferredLoadAddress()
	{
		return preferredLoadAddress;
	}
	
	MAPFILEDLL_API MapFileAddress *getEntryPoint()
	{
		return entryPoint;
	}

	MAPFILEDLL_API int getNumSections()
	{
		return sections.size();
	}

	MAPFILEDLL_API int getNumPublicSymbols()
	{
		return publicSymbols.size();
	}

	MAPFILEDLL_API int getNumStaticSymbols()
	{
		return staticSymbols.size();
	}

	MAPFILEDLL_API int getNumImportedSymbols()
	{
		return imports.size();
	}

	MAPFILEDLL_API int getNumExportedSymbols()
	{
		return exports.size();
	}

	MAPFILEDLL_API int getNumLineNumberFiles()
	{
		return lineNumberFiles.size();
	}

	MAPFILEDLL_API int getNumFixups()
	{
		return fixups.size();
	}

	MAPFILEDLL_API MapFileSection *getSection(int	index)
	{
		return (MapFileSection *)sections[index];
	}

	MAPFILEDLL_API MapFileSymbol *getPublicSymbol(int	index)
	{
		return (MapFileSymbol *)publicSymbols[index];
	}

	MAPFILEDLL_API MapFileSymbol *getStaticSymbol(int	index)
	{
		return (MapFileSymbol *)staticSymbols[index];
	}

	MAPFILEDLL_API MapFileImport *getImport(int	index)
	{
		return (MapFileImport *)imports[index];
	}

	MAPFILEDLL_API MapFileExport *getExport(int	index)
	{
		return (MapFileExport *)exports[index];
	}

	MAPFILEDLL_API MapFileLineNumberFile *getLineNumberFile(int	index)
	{
		return (MapFileLineNumberFile *)lineNumberFiles[index];
	}

	MAPFILEDLL_API MapFileFixup *getFixup(int	index)
	{
		return (MapFileFixup *)fixups[index];
	}

	MAPFILEDLL_API void addSection(MapFileSection	*mfs)
	{
		sections.push_back(mfs);
	}

	MAPFILEDLL_API void addPublicSymbol(MapFileSymbol	*mfs)
	{
		publicSymbols.push_back(mfs);
	}

	MAPFILEDLL_API void addStaticSymbol(MapFileSymbol	*mfs)
	{
		staticSymbols.push_back(mfs);
	}

	MAPFILEDLL_API void addImport(MapFileImport	*mfi)
	{
		imports.push_back(mfi);
	}

	MAPFILEDLL_API void addExport(MapFileExport	*mfe)
	{
		exports.push_back(mfe);
	}

	MAPFILEDLL_API void addLineNumberFile(MapFileLineNumberFile	*lnf)
	{
		lineNumberFiles.push_back(lnf);
	}

	MAPFILEDLL_API void addFixup(MapFileFixup	*mffu)
	{
		fixups.push_back(mffu);
	}

	MAPFILEDLL_API static char *getNonBlankLine(FILE	*fp,
												char	*line,
												int		len);

	MAPFILEDLL_API static int isBlankLine(char	*p);

	MAPFILEDLL_API static char *skipWhiteSpace(char	*p,
												char	mustSkip = '\0');

	MAPFILEDLL_API static char *findWhiteSpace(char	*p,
												char	mustSkip = '\0');

	MAPFILEDLL_API static char *getNextWord(char		*p,
		std::string		&s,
											char		mustSkip = '\0');

	MAPFILEDLL_API static int getNextWord(FILE		*fp,
		std::string	&s);

	MAPFILEDLL_API int findSymbol(std::string		&symbol,
								  MapFileSymbol	**mfs);

protected:
	int readDLLName(FILE	*fp);

	int readTimeStamp(FILE	*fp);

	int readPreferredLoadAddress(FILE	*fp);
	
	int readSectionInformation(FILE	*fp);

	int readPublicSymbols(FILE	*fp);

	int readEntryPoint(FILE	*fp);

	int readLineNumbers(FILE	*fp);

	int readExports(FILE	*fp);

	int readStaticSymbols(FILE	*fp);

	int	readFixups(FILE	*fp);

	int readLineNumbers(char	*p,
						FILE	*fp);

	int readExports(char	*p,
					FILE	*fp);

	int readStaticSymbols(char	*p,
						  FILE	*fp);

	int	readFixup(char	*p);

private:
	void flushSymbols(std::vector<MapFileSymbol*>	&syms);

	void flushSections();

	void flushPublicSymbols();

	void flushStaticSymbols();

	void flushImports();

	void flushExports();

	void flushLineNumberFiles();

	void flushFixups();

	int readSymbols(FILE	*fp, 
				    int		addToPublicSymbols);

	char *getFlagInSpace(char	*p,
						  char	match,
						  int	*flag);
private:
	int				verbose;			// for debugging
	
	std::string			fileName;			// map file to read
	
	MapFileAddress	*entryPoint;
	DWORD			preferredLoadAddress;
	DWORD			maxAddress;			// maxAddress found in dll for a symbol

	std::string			mapObjectName;		// name of dll (may be different from name of map)
	std::string			timeStamp;			// when created

	std::vector<MapFileSection*>		sections;			// array of MapFileSection objects
	std::vector<MapFileSymbol*>		publicSymbols;			// array of MapFileSymbol objects
	std::vector<MapFileSymbol*>		staticSymbols;		// array of MapFileSymbol objects
	std::vector<MapFileImport *>		imports;			// array of MapFileImport objects
	std::vector<MapFileExport *>		exports;			// array of MapFileExport objects
	std::vector<MapFileLineNumberFile *>		lineNumberFiles;	// array of MapFileLineNumberFile objects
	std::vector<MapFileFixup *>		fixups;				// array of MapFileFixup objects

	DWORD			actualLoadAddress;	// for those occasions when you know this value, NULL otherwise
};

#endif
