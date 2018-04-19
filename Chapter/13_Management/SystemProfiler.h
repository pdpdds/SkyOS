#pragma once
#include "windef.h"

typedef struct tag_MemoryInfo
{
	DWORD _totalSize;
	DWORD _usedSize;
	DWORD _heapSize;
	DWORD _usedHeapSize;

}MemoryInfo;

typedef struct tag_GlobalState
{
	DWORD _kernelLoadAddress;
	DWORD _HeapLoadAddress;
	DWORD _heapSize;
	DWORD _kernelSize;
	DWORD _stackPhysicalPoolAddress;
	DWORD _pageDirectoryPoolAddress;

}GlobalSate;

class SystemProfiler
{
public:
	~SystemProfiler();

	static SystemProfiler* GetInstance()
	{
		if (m_pSystemProfiler == nullptr)
			m_pSystemProfiler = new SystemProfiler();

		return m_pSystemProfiler;
	}

	void PrintMemoryState();
	void PrintGlobalState();
	void SetGlobalState(GlobalSate& state) { m_state = state; }

private:
	SystemProfiler();
	static SystemProfiler* m_pSystemProfiler;

	MemoryInfo m_memoryInfo;
	GlobalSate m_state;
};