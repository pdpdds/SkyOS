#include <string.h>
#include "fsys.h"
#include "image.h"
#include "VirtualMemoryManager.h"
#include "PhysicalMemoryManager.h"
#include "task.h"
#include "Console.h"
#include "ProcessManager.h"
#include "fsys.h"
#include "defines.h"
#include "pit.h"

//32비트 PE파일 이미지 유효성 검사
bool ValidatePEImage(void* image) 
{
    IMAGE_DOS_HEADER* dosHeader = 0;
    IMAGE_NT_HEADERS* ntHeaders = 0;
    
    dosHeader = (IMAGE_DOS_HEADER*)image;
    if (dosHeader->e_magic != IMAGE_DOS_SIGNATURE)             
		return false;
    
	if (dosHeader->e_lfanew == 0)
		return false;
    
    //NT Header 체크
    ntHeaders = (IMAGE_NT_HEADERS*)(dosHeader->e_lfanew + (uint32_t)image);
    if (ntHeaders->Signature != IMAGE_NT_SIGNATURE)            
		return false;    

    /* only supporting for i386 archs */
    if (ntHeaders->FileHeader.Machine != IMAGE_FILE_MACHINE_I386)             
		return false;    

    /* only support 32 bit executable images */
    if (! (ntHeaders->FileHeader.Characteristics & (IMAGE_FILE_EXECUTABLE_IMAGE | IMAGE_FILE_32BIT_MACHINE)))             
		return false;
    
    /*
            Note: 1st 4 MB remains idenitity mapped as kernel pages as it contains
            kernel stack and page directory. If you want to support loading below 1MB,
            make sure to move these into kernel land
    */

	//로드되는 프로세스의 베이스 주소는 0x00400000다. 
	//비쥬얼 스튜디오에서 속성=> 링커 => 고급의 기준주소 항목에서 확인 가능하다
    if ( (ntHeaders->OptionalHeader.ImageBase < 0x400000) || (ntHeaders->OptionalHeader.ImageBase > 0x80000000))
		return false;
    
    /* only support 32 bit optional header format */
    if (ntHeaders->OptionalHeader.Magic != IMAGE_NT_OPTIONAL_HDR32_MAGIC) 
            return false;

//유효한 32비트 PE 파일이다.
	return true;
}

extern "C" 
{
	uint32_t MemoryAlloc(size_t size)
	{
		Process* pProcess = ProcessManager::GetInstance()->GetCurrentProcess();
		void *addr = alloc(size, (u8int)0, (heap_t*)pProcess->m_lpHeap);

#ifdef _ORANGE_DEBUG
		SkyConsole::Print("process heap alloc, %d %x\n", size, pProcess->m_lpHeap);
		SkyConsole::Print("process heap alloc, %d %x\n", size, pProcess->m_lpHeap);
#endif			
		return (u32int)addr;
	}

	void MemoryFree(void* p)
	{
		//힙은 스레드가 모두 공유한다.
		//따라서 메모리를 해제할시 컨텍스트 스위칭이 일어나서 다른 스레드가 같은 자원(힙)에 접근할 수 있는 가능성이 생기므로
		//인터럽트가 일어나지 않게 처리한다.
		EnterCriticalSection();
		Process* pProcess = ProcessManager::GetInstance()->GetCurrentProcess();
		free(p, (heap_t*)pProcess->m_lpHeap);
		LeaveCriticalSection();
	}

	//프로세스 전용의 디폴트 힙을 생성한다
	void CreateDefaultHeap() 
	{
		EnterCriticalSection();
		
		Process* pProcess = ProcessManager::GetInstance()->GetCurrentProcess();
		Thread* pThread = pProcess->GetThread(0);
		
	//1메가 바이트의 힙을 생성
		void* pHeapPhys = PhysicalMemoryManager::AllocBlocks(DEFAULT_HEAP_PAGE_COUNT);
		u32int heapAddess = pThread->m_imageBase + pThread->m_imageSize + PAGE_SIZE + PAGE_SIZE * 2;
		
	//힙 주소를 4K에 맞춰 Align	
		heapAddess -= (heapAddess % PAGE_SIZE);
		
#ifdef _ORANGE_DEBUG
		SkyConsole::Print("heap adress %x\n", heapAddess);
#endif // _ORANGE_DEBUG
				
		for (int i = 0; i < DEFAULT_HEAP_PAGE_COUNT; i++)
		{
			VirtualMemoryManager::MapPhysicalAddressToVirtualAddresss(pProcess->m_pPageDirectory,
				(uint32_t)heapAddess + i * PAGE_SIZE,
				(uint32_t)pHeapPhys + i * PAGE_SIZE,
				I86_PTE_PRESENT | I86_PTE_WRITABLE | I86_PTE_USER);
		}
		
		memset((void*)heapAddess, 0, DEFAULT_HEAP_PAGE_COUNT * PAGE_SIZE);
		
		pProcess->m_lpHeap = create_heap((u32int)heapAddess, (uint32_t)heapAddess + DEFAULT_HEAP_PAGE_COUNT * PAGE_SIZE, 
			                             (uint32_t)heapAddess + DEFAULT_HEAP_PAGE_COUNT * PAGE_SIZE, 0, 0);

		LeaveCriticalSection();		
	}
	
	//프로세스 종료	
	extern "C" void TerminateProcess()
	{
		EnterCriticalSection();

		Process* cur = ProcessManager::GetInstance()->GetCurrentProcess();

		if (cur == NULL || cur->m_processId == PROC_INVALID_ID)
		{
			SkyConsole::Print("Invailid Process Termination\n");
			return;
		}

		//프로세스 매니저에서 해당 프로세스를 완전히 제거한다.
		//태스크 목록에서도 제거되어 해당 프로세스는 더이상 스케쥴링 되지 않는다.
		ProcessManager::GetInstance()->DestroyProcess(cur);

		LeaveCriticalSection();

		for (;;);
	}	
}
