#include "StdAfx.h"
#include "EHStackWalker.h"
#include "EHUtil.h"
#include <stdio.h>
#include <fstream>

using namespace  std;

CEHStackWalker::CEHStackWalker(void)
{
}

void CEHStackWalker::SetLogFilePath(void)
{
	_set_printf_count_output(true);
	GetOutofMemoryLogPath(m_szDumpPath);
}

CEHStackWalker::~CEHStackWalker(void)
{
}

void CEHStackWalker::OnOutput( LPCSTR szText )
{
	printf("%s%n", szText);
	wofstream outFile(m_szDumpPath,ios::app);
	if(!outFile.good())
	{
		return;
	}

	outFile<<szText<<endl;
}