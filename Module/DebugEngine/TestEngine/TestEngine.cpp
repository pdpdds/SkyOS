// TestEngine.cpp : 콘솔 응용 프로그램에 대한 진입점을 정의합니다.
//

#include "stdafx.h"
#include <stdio.h> 
#include <windows.h> 
#include "MapFile.h"
#include "MapFileSymbol.h"
#include "Undecorate.h"
#include "SkyFileInterface.h"

extern SKY_FILE_Interface g_FileInterface;

int main()
{
	BOOL freeResult;
	HINSTANCE dllHandle = NULL;

	//Load the dll and keep the handle to it
	//dllHandle = LoadLibrary("DebugEngine.dll");
	char* fileName = "SkyOS32.map";
	MapFileReader* pReader = new MapFileReader(g_FileInterface);

	if (pReader->readFile(fileName))
	{
		MapFileSymbol	*symbol = NULL;
		int				n, i;

		// do symbol query

		n = pReader->getNumPublicSymbols();
		std::string m_symbol = "GetCurrentTask";
		for (i = 0; i < n; i++)
		{
			MapFileSymbol	*sym;
			char nameDec[MAX_PATH];
			char nameUnDec[MAX_PATH];

			// get symbol name and undecorate to get simple name with no params or types

			sym = pReader->getPublicSymbol(i);
			sym->getName(nameDec);
			simpleUndecorateCPP(nameDec, nameUnDec);

			// compare to value user typed, we compare both decorated and undecorated names

			int	r;

			
				r = (m_symbol.compare(nameUnDec) == 0 ||
					m_symbol.compare(nameDec) == 0);
			
			
			if (r)
			{
				// found it

				symbol = sym;
				break;
			}
		}




		int				result;
		int				lineNumber = 0;
		char		l_module[MAX_PATH];
		char		l_function[MAX_PATH];
		DWORD			l_resultAddress;

		pReader->setLoadAddress(pReader->getPreferredLoadAddress());
		result = pReader->getAddressInfo(symbol->getRVABase(),
			l_module, fileName, lineNumber, l_function, l_resultAddress);
		if (result)
		{
			// setup line number

			char	temp[100];

			sprintf(temp, "%d", lineNumber);
			printf(temp);
		}

		pReader->getPreferredLoadAddress();
	}
}

