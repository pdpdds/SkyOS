#include "SkyDebugger.h"
#include "SkyConsole.h"
#include "ProcessManager.h"
#include "Thread.h"
#include "SkyModuleManager.h"
#include "Exception.h"
#include "SkyMockInterface.h"
#include "MapFile.h"
#include "StorageManager.h"
#include "stdio.h"
#include "kheap.h"
#include "SkyAPI.h"
#include "Process.h"

SkyDebugger* SkyDebugger::m_pDebugger = nullptr;

typedef I_MapFileReader*(*PGetDebugEngineDLL)();

SKY_FILE_Interface g_FileInterface =
{
	fread,
	fopen,
	fwrite,
	fclose,
	feof,
	ferror,
	fflush,
	freopen,
	fseek,
	ftell,
	fgetc,
	fgets,
};

u32int sky_kmalloc(u32int sz)
{
	return kmalloc(sz);
}

void sky_kfree(void *p)
{
	kfree(p);
}

unsigned int sky_kcalloc(unsigned int count, unsigned int size)
{
	return calloc(count, size);
}

void* sky_krealloc(void* ptr, size_t size)
{
	return krealloc(ptr, size);
}

SKY_ALLOC_Interface g_allocInterface =
{
	sky_kmalloc,
	sky_kfree,
	sky_kcalloc,
	sky_krealloc,
};


void sky_printf(const char* str, ...)
{
	char *p = new char[128];
	va_list ap;

	va_start(ap, str);
	(void)vsprintf(p, str, ap);
	va_end(ap);

	SkyConsole::Print(p);
	delete p;
}


SKY_Print_Interface g_printInterface =
{
	sky_printf,
	0,
	0,
	0,
};

unsigned int sky_kcreate_process_from_memory(const char* appName, LPTHREAD_START_ROUTINE lpStartAddress, void* param, UINT32 processType)
{
	return 0;
}

unsigned int sky_kcreate_process_from_file(char* appName, void* param, UINT32 processType)
{
	return 0;
}

unsigned int sky_kcreate_thread_from_memory(unsigned int processId, LPTHREAD_START_ROUTINE lpStartAddress, LPVOID param)
{
	DWORD dwThreadId = 0;

	kEnterCriticalSection();
	Process* pProcess = ProcessManager::GetInstance()->GetCurrentTask()->m_pParent;
	Thread* pThread = ProcessManager::GetInstance()->CreateThread(pProcess, lpStartAddress, param);
	pProcess->AddThread(pThread);

	kLeaveCriticalSection();

	return (unsigned int)pThread;
}

unsigned int sky_kcreate_thread_from_file(unsigned int processId, FILE* pFile, LPVOID param)
{
	return 0;
}


unsigned int sky_kdestroy_task(unsigned int taskId)
{

	return 0;
}

void sky_ksleep(int ms)
{
	ksleep(ms);
}

//프로세스 생성 및 삭제
SKY_PROCESS_INTERFACE g_processInterface =
{
	sky_kcreate_process_from_memory,
	sky_kcreate_process_from_file,
	sky_kcreate_thread_from_memory,
	sky_kcreate_thread_from_file,
	sky_kdestroy_task,
	sky_ksleep,
};

SkyDebugger::SkyDebugger()
{
	m_pMapReader = nullptr;
	m_symbolInit = false;
}


SkyDebugger::~SkyDebugger()
{
}

void SkyDebugger::TraceStackWithSymbol(unsigned int maxFrames)
{
	//스택 상황
	//  첫번째 파라메터 maxFrames
	//  TraceStackWithSymbol 함수를 실행시킨 호출함수 복귀주소
	//  호출함수의 EBP(현재 EBP 레지스터가 이값을 가리키고 있다)
	unsigned int* ebp = &maxFrames - 2;
	SkyConsole::Print("Stack trace:\n");
	//현재 태스크를 얻어낸다.
	Thread* pTask = ProcessManager::GetInstance()->GetCurrentTask();

	int lineNumber = 0;
	DWORD resultAddress = 0;
	char function[256];
	char objFileName[256];
	char filename[256];
	char undecorateName[256];

	for (unsigned int frame = 0; frame < maxFrames; ++frame)
	{
		unsigned int eip = ebp[1];
		if (eip == 0)
			//함수 복귀주소가 0이면 콜스택 출력을 끝낸다.
			break;
		// 직전 호출함수의 스택프레임으로 이동한다.
		ebp = reinterpret_cast<unsigned int *>(ebp[0]);
		unsigned int * arguments = &ebp[2];

		if (m_symbolInit == true && m_pMapReader != nullptr)
		{
			// 심벌엔진으로 부터 해당주소의 함수이름 정보 등을 얻어온다.
			memset(function, 0, 256);
			memset(objFileName, 0, 256);
			memset(filename, 0, 256);
			memset(undecorateName, 0, 256);

			bool result = m_pMapReader->getAddressInfo(eip, objFileName, filename, lineNumber, function, resultAddress);

			if (result == true)
			{
				result = m_pMapReader->SimpleUndecorateCPP(function, undecorateName, 256);
				if (result == true)
				{
					SkyConsole::Print("  %s + 0x%x, %s", undecorateName, eip - resultAddress, objFileName);
				}
				else
				{
					SkyConsole::Print("  %s + 0x%x, %s", function, eip - resultAddress, objFileName);
				}
				
			}
		}
		else
		{
			SkyConsole::Print("  0x{%x}", eip);
		}		
	}
}

//방법상의 문제로 가장 마지막으로 호출된 함수는 표기되지 않는다.
//이유는 TraceStackWithSymbol 메소드와 대조해보면 알 수 있다.
//해결책
//1. 타켓 프로세스의 페이지 디렉토리로 교체한 후 EIP 레지스터를 덤프한다.
//2. 페이지 디렉토리를 원래대로 복원한뒤 EIP 주소에 해당하는 심벌을 얻는다.

void SkyDebugger::TraceStackWithProcessId(int processId)
{
	kEnterCriticalSection();

	Process* pProcess = ProcessManager::GetInstance()->FindProcess(processId);
	Thread* pTask = ProcessManager::GetInstance()->GetCurrentTask();


	if (pProcess == nullptr)
	{
		SkyConsole::Print("process not found!!\n");
	}
	else
	{
		if (pTask->m_pParent == pProcess)
		{
			kLeaveCriticalSection();
			TraceStackWithSymbol();
			return;
		}

		SkyConsole::Print("Stack trace:\n");
		Thread* pThread = pProcess->GetMainThread();
		unsigned int* ebp = (unsigned int*)pThread->m_contextSnapshot.ebp;
		

		int lineNumber = 0;
		DWORD resultAddress = 0;
		char function[256];
		char objFileName[256];
		char filename[256];
		char undecorateName[256];

		//SkyConsole::Print("  0x%x 0x%x 0x%x\n", ebp, ebp[0], ebp[1]);

		for (unsigned int frame = 0; frame < 20; ++frame)
		{
			unsigned int eip = ebp[1];
			if (eip == 0)
				//함수 복귀주소가 0이면 콜스택 출력을 끝낸다.
				break;

			// 직전 호출함수의 스택프레임으로 이동한다.
			ebp = reinterpret_cast<unsigned int *>(ebp[0]);
			unsigned int * arguments = &ebp[2];

			if (m_symbolInit == true && m_pMapReader != nullptr)
			{
				// 심벌엔진으로 부터 해당주소의 함수이름 정보 등을 얻어온다.
				memset(function, 0, 256);
				memset(objFileName, 0, 256);
				memset(filename, 0, 256);
				memset(undecorateName, 0, 256);
				
				bool result = m_pMapReader->getAddressInfo(eip, objFileName, filename, lineNumber, function, resultAddress);
								

				if (result == true)
				{
					result = m_pMapReader->SimpleUndecorateCPP(function, undecorateName, 256);
					if (result == true)
					{
						SkyConsole::Print("  %s + 0x%x, %s", undecorateName, eip - resultAddress, objFileName);
					}
					else
					{
						SkyConsole::Print("  %s + 0x%x, %s", function, eip - resultAddress, objFileName);
					}

				}
			}
			else
			{
				SkyConsole::Print("  0x{%x}", eip);
			}
		}
	}

	kLeaveCriticalSection();
}

//디버그엔진 모듈을 로드한다.
bool SkyDebugger::LoadSymbol(const char* moduleName)
{
	
//디버그 모듈 엔진을 찾는다.
	MODULE_HANDLE hwnd = SkyModuleManager::GetInstance()->LoadModuleFromMemory(moduleName);

	if (hwnd == nullptr)
	{
		HaltSystem("Memory Module Load Fail!!");
	}

	
//디버그 엔진 모듈로 부터 SetSkyMockInterface, GetDebugEngineDLL 함수를 얻어온다.
	PSetSkyMockInterface SetSkyMockInterface = (PSetSkyMockInterface)SkyModuleManager::GetInstance()->GetModuleFunction(hwnd, "SetSkyMockInterface");
	PGetDebugEngineDLL GetDebugEngineDLLInterface = (PGetDebugEngineDLL)SkyModuleManager::GetInstance()->GetModuleFunction(hwnd, "GetDebugEngineDLL");

//디버그 엔진에 플랫폼 종속적인 인터페이스를 넘긴다.
	SetSkyMockInterface(g_allocInterface, g_FileInterface, g_printInterface);

	if (!GetDebugEngineDLLInterface)
	{
		HaltSystem("Memory Module Load Fail!!");
	}
	
	m_pMapReader = GetDebugEngineDLLInterface();
	
	if (m_pMapReader == nullptr)
	{
		HaltSystem("Map Reader Creation Fail!!");
	}

	StorageManager::GetInstance()->SetCurrentFileSystemByID('L');

	bool result = m_pMapReader->readFile("SKYOS32_MAP");
	
	if (result == false)
	{
		HaltSystem("Map Read Fail!!");
	}

	unsigned int preferredAddress = m_pMapReader->getPreferredLoadAddress();
	m_pMapReader->setLoadAddress(preferredAddress);

	m_symbolInit = true;
	return true;
}
