#pragma once
#include "MultiBoot.h"

class I_MapFileReader;

class SkyDebugger
{
public:
	~SkyDebugger();

	static SkyDebugger* GetInstance()
	{
		if (m_pDebugger == 0)
			m_pDebugger = new SkyDebugger();

		return m_pDebugger;
	}
	
	void TraceStackWithSymbol(unsigned int maxFrames = 20);
	void TraceStackWithProcessId(int processId);

	bool LoadSymbol(const char* moduleName);


private:
	SkyDebugger();
	static SkyDebugger* m_pDebugger;

	I_MapFileReader* m_pMapReader;
	bool m_symbolInit;
};