#include "ServiceFunc.h"
#include "MapFile.h"
#include "I_MapFileReader.h"

//샘플 C++ 클래스 익스포트 테스트용

class CSampleDLLInterface : public I_SampleDLLInterface
{
public:
	virtual int AddNumbers(int a, int b) override
	{		
		return a + b;
	}
	virtual void MyMessageBox(const char* message) override
	{
		
	}
};

static CSampleDLLInterface g_SampleDLLInterface;

__declspec(dllexport) CSampleDLLInterface* GetSampleDLLInterface()
{
	return &g_SampleDLLInterface;
}

//End

SkyMockInterface g_mockInterface;

extern "C" __declspec(dllexport) void SetSkyMockInterface(SKY_ALLOC_Interface allocInterface, 
														  SKY_FILE_Interface fileInterface, 
														  SKY_Print_Interface printInterface)
{
	g_mockInterface.g_allocInterface = allocInterface;
	g_mockInterface.g_fileInterface = fileInterface;
	g_mockInterface.g_printInterface = printInterface;
}


extern "C" __declspec(dllexport) I_MapFileReader* GetDebugEngineDLL()
{
	I_MapFileReader* reader = new MapFileReader();
	return reader;
}

#define kmalloc(a) g_mockInterface.g_allocInterface.sky_kmalloc(a);
#define kfree(p) g_mockInterface.g_allocInterface.sky_kfree(p);


void *operator new(size_t size)
{
	return (void*)kmalloc(size);
}

void* __cdecl operator new[](size_t size)
{
	return (void*)kmalloc(size);
}

void __cdecl operator delete(void *p)
{
	kfree(p);
}

void operator delete(void *p, size_t size)
{
	kfree(p);
}

void operator delete[](void *p, size_t size)
{
	kfree(p);
}


void operator delete[](void *p)
{
	kfree(p);
}

int __cdecl _purecall()
{
	return 0;
}