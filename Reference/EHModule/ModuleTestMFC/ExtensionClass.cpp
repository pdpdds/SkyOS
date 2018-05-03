#include "StdAfx.h"
#include "BaseClass.h"
#include "ExtensionClass.h"

CExtensionClass::CExtensionClass(void)
: m_iExtensionVar(0)
{
}

CExtensionClass::~CExtensionClass(void)
{
}

void CExtensionClass::InitArray()
{
	memset(m_aArray, 0, MAX_ARRAY_SIZE * sizeof(int));
}

int CExtensionClass::ProcessTask()
{
	DWORD dwTickCount = GetTickCount();		

	int iLocalExtensionVar = (int)dwTickCount;

	return iLocalExtensionVar;
}

int CExtensionClass::ProcessStackOverFlow()
{
	DWORD dwTickCount = GetTickCount();	

	this->ProcessStackOverFlow();

	int iLocalExtensionVar = (int)dwTickCount;

	return iLocalExtensionVar;
}

int CExtensionClass::ProcessHeapCorruption()
{
	DWORD dwTickCount = GetTickCount();		

	int iLocalExtensionVar = (int)dwTickCount;

	memset(m_aArray, 0, MAX_ARRAY_SIZE* sizeof(int));

	return iLocalExtensionVar;
}