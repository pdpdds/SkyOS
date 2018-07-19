#include "GUIMint64.h"
#include "Hal.h"
#include "SkyIOHandler.h"
#include "I_GUIEngine.h"
#include "SkyMockInterface.h"
#include "SkyModuleManager.h"
#include "Exception.h"
#include "ProcessManager.h"
#include "Scheduler.h"
#include "Thread.h"
#include "Process.h"
#include "SkyAPI.h"

typedef I_GUIEngine* (*PGUIEngine)();
extern SKY_FILE_Interface g_FileInterface;
extern SKY_ALLOC_Interface g_allocInterface;
extern SKY_Print_Interface g_printInterface;
extern SKY_PROCESS_INTERFACE g_processInterface;

GUIMint64::GUIMint64()
{

}


GUIMint64::~GUIMint64()
{
}

extern void SampleFillRect(ULONG* lfb, int x, int y, int w, int h, int col);
bool GUIMint64::Initialize(void* pVideoRamPtr, int width, int height, int bpp, uint8_t buffertype)
{
	m_pVideoRamPtr = (ULONG*)pVideoRamPtr;
	m_width = width;
	m_height = height;
	m_bpp = bpp;
	
	MODULE_HANDLE hwnd = SkyModuleManager::GetInstance()->LoadModuleFromMemory("GUIEngine.dll");

	if (hwnd == nullptr)
	{
		HaltSystem("Memory Module Load Fail!!");
	}	

	
	PSetSkyMockInterface SetSkyMockInterface = (PSetSkyMockInterface)SkyModuleManager::GetInstance()->GetModuleFunction(hwnd, "SetSkyMockInterface");
	
	PSetSkyProcessInterface SetSkyProcessInterface = (PSetSkyProcessInterface)SkyModuleManager::GetInstance()->GetModuleFunction(hwnd, "SetSkyProcessInterface");
	PGUIEngine GUIEngine = (PGUIEngine)SkyModuleManager::GetInstance()->GetModuleFunction(hwnd, "GetGUIEngine");
	
	SetSkyMockInterface(g_allocInterface, g_FileInterface, g_printInterface);
	SetSkyProcessInterface(g_processInterface);
	
	m_pEngine = GUIEngine();
	LinearBufferInfo info;
	info.pBuffer = (unsigned long*)m_pVideoRamPtr;
	info.width = m_width;
	info.height = m_height;
	info.depth = m_bpp;
	info.type = 0;
	info.isDirectVideoBuffer = true;

	m_pEngine->SetLinearBuffer(info);
	m_pEngine->Initialize();

	SkyIOHandler::GetInstance();
	SkyIOHandler::GetInstance()->SetCallback(m_pEngine);
	

	SkyIOHandler::GetInstance()->Initialize();	
	
	return true;
}

bool GUIMint64::Run()
{
	//SampleFillRect(m_pVideoRamPtr, 1004, 0, 20, 20, 0x00FF0000);
	//for (;;);
	//SampleFillRect((ULONG *)m_pVideoRamPtr, 1004, 0, 20, 20, 0x00FF0000);

	int colorStatus[] = { 0x00FF0000, 0x0000FF00, 0x0000FF };
	int pos = 0;

	m_pEngine->Update(0);

	while (1)
	{
		if (++pos > 2)
			pos = 0; 

		SampleFillRect(m_pVideoRamPtr, 1004, 0, 20, 20, colorStatus[pos]);
		
		
	}
	
	return true;
}

bool GUIMint64::Print(char* pMsg)
{
	return true;
}

bool GUIMint64::Clear()
{
	return true;
}