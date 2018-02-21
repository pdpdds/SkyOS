#include "kmain.h"
#include "stdio.h"
#include "FAT32VFSAdaptor.h"
#include "LowVGA.h"

extern bool systemOn;
void HardwareInitiize();

_declspec(naked) void multiboot_entry(void)
{
	__asm {
		align 4

		multiboot_header:
		dd(MULTIBOOT_HEADER_MAGIC); magic number
			dd(MULTIBOOT_HEADER_FLAGS); flags
			dd(CHECKSUM); checksum
			dd(HEADER_ADRESS); header address
			dd(KERNEL_LOAD_ADDRESS); load address
			dd(00); load end address : not used
			dd(00); bss end addr : not used
			dd(HEADER_ADRESS + 0x20); entry_addr: equ kernel entry
			; 0x20 is the size of multiboot header

			kernel_entry :
		mov     esp, KERNEL_STACK; Setup the stack

			push    0; Reset EFLAGS
			popf

			push    ebx; Push multiboot info address
			push    eax; and magic number
			; which are loaded in registers
			; eax and ebx before jump to
			; entry adress
			; [HEADER_ADRESS + 0x20]
			call    kmain; kernel entry
			halt :
		jmp halt; halt processor
	}
}

void kmain(unsigned long magic, unsigned long addr)
{
	bool consoleMode = true;

	InitializeConstructors();

	SkyConsole::Initialize();

	//헥사를 표시할 때 %X는 integer, %x는 unsigned integer의 헥사값을 표시한다.	
	SkyConsole::Print("*** Sky OS Console System Init ***\n");

	kEnterCriticalSection(&g_criticalSection);

	HardwareInitiize();
	SkyConsole::Print("Hardware Init..\n");

	SetInterruptVector();
	InitializeSysCall();	
	install_tss(5, 0x10, 0);

	SkyConsole::Print("Interrput Handler & System Call Init..\n");

	InitKeyboard();

	SkyConsole::Print("Keyboard Init..\n");

	multiboot_info* pBootInfo = (multiboot_info*)addr;
	InitMemoryManager(pBootInfo, 0);

	InitFloppyDrive();
	SkyConsole::Print("Floppy Disk Init..\n");

	//StartPITCounter(100, I86_PIT_OCW_COUNTER_0, I86_PIT_OCW_MODE_SQUAREWAVEGEN);

	//kLeaveCriticalSection(&g_criticalSection);
	/*if (true == InitHardDrive())
	{
		InitFATFileSystem();
#ifdef _SKY_DEBUG
		TestHardDrive();
#endif
		InitializeVFSFat32();
		SkyConsole::Print("Harddisk Init..\n");
	}
	else
	{
		SkyConsole::Print("Harddisk not detected..\n");
	}*/

	//DumpSystemInfo(pBootInfo);
	SkyConsole::Print("Boot Loader Name : %s\n", (char*)pBootInfo->boot_loader_name);


	if (consoleMode == true)
	{
		StartConsoleSystem();
	}
	else
	{
		//InitGraphics(pBootInfo);
		StartLowModeVGA();
	}
}

// 하드 디스크 초기화가 실패하면 8Mbyte 크기의 램 디스크를 생성
// 힙영역을 크게 잡아야 한다.
/*if (kInitializeRDD(RDD_TOTALSECTORCOUNT) != FALSE || kFormat())
{
HaltSystem("Ram Disk Create Fail");
}*/

//Initialize FILESYSTEM
//VFSInitialize();

void HardwareInitiize()
{
	GDTInitialize();
	IDTInitialize(0x8);
	PICInitialize(0x20, 0x28);
	InitializePIT();
}

uint32_t GetFreeSpaceMemory(multiboot_info* bootinfo)
{
	uint32_t memorySize = 0;
	uint32_t mmapEntryNum = bootinfo->mmap_length / sizeof(multiboot_memory_map_t);

	//SkyConsole::Print("GRUB Information\n");
	//SkyConsole::Print("Boot Loader Name : %s\n", (char*)bootinfo->boot_loader_name);
	//SkyConsole::Print("Memory Map Entry Num : %d\n", mmapEntryNum);

	uint32_t mmapAddr = bootinfo->mmap_addr;

	for (uint32_t i = 0; i < mmapEntryNum; i++)
	{
		multiboot_memory_map_t* entry = (multiboot_memory_map_t*)mmapAddr;

#ifdef _SKY_DEBUG
		SkyConsole::Print("Memory Address : %x\n", entry->addr);
		SkyConsole::Print("\nMemory Length : %x", entry->len);
		SkyConsole::Print("\nMemory Type : %x", entry->type);
		SkyConsole::Print("\nEntry Size : %d", entry->size);
#endif

		mmapAddr += sizeof(multiboot_memory_map_t);

		if (entry->addr + entry->len < FREE_MEMORY_SPACE_ADDRESS)
			continue;

		memorySize += entry->len;
		if (entry->addr < FREE_MEMORY_SPACE_ADDRESS)
			memorySize -= (FREE_MEMORY_SPACE_ADDRESS - entry->addr);
	}

	memorySize -= (memorySize % 4096);

	return memorySize;
}

// where the kernel is to be loaded to in protected mode
// define IMAGE_PMODE_BASE 0xC0000000; 0x100000
//  where the kernel is to be loaded to in real mode
//  define IMAGE_RMODE_BASE 0x3000

bool InitMemoryManager(multiboot_info* bootinfo, uint32_t kernelSize)
{
	PhysicalMemoryManager::EnablePaging(false);

	uint32_t freeSpaceMemorySize = GetFreeSpaceMemory(bootinfo);

	freeSpaceMemorySize = 4096 * 1024 * 200;

	//SkyConsole::Print("KernelSize : %d Bytes\n", kernelSize);
	SkyConsole::Print("FreeSpace MemorySize From 0x%x: 0x%x Bytes\n", FREE_MEMORY_SPACE_ADDRESS, freeSpaceMemorySize);


	//물리 메모리 매니저 초기화
	PhysicalMemoryManager::Initialize(freeSpaceMemorySize, FREE_MEMORY_SPACE_ADDRESS);
	//PhysicalMemoryManager::Dump();

	uint32_t memoryMapSize = PhysicalMemoryManager::GetMemoryMapSize();
	uint32_t alignedMemoryMapSize = (memoryMapSize / 4096) * 4096;

	if (memoryMapSize % 4096 > 0)
		alignedMemoryMapSize += 4096;

	PhysicalMemoryManager::SetAvailableMemory(FREE_MEMORY_SPACE_ADDRESS + alignedMemoryMapSize, freeSpaceMemorySize - (FREE_MEMORY_SPACE_ADDRESS + alignedMemoryMapSize));
	PhysicalMemoryManager::Dump();

//가상 메모리 매니저 초기화	
	VirtualMemoryManager::Initialize();

	SkyConsole::Print("Init Complete\n");
	//커널 힙을 생성한다
	VirtualMemoryManager::CreateKernelHeap();

	return true;
}

void StartConsoleSystem()
{
	//kEnterCriticalSection(&g_criticalSection);
	
	Process* pProcess = ProcessManager::GetInstance()->CreateKernelProcessFromMemory("ConsoleSystem", SystemConsoleProc);

	if (pProcess == nullptr)
		HaltSystem("Console Creation Fail!!");

	ProcessManager::GetInstance()->CreateKernelProcessFromMemory("WatchDog", WatchDogProc);
	ProcessManager::GetInstance()->CreateKernelProcessFromMemory("ProcessRemover", ProcessRemoverProc);
	ProcessManager::GetInstance()->CreateProcessFromMemory("SampleLoop", SampleLoop);
	ProcessManager::GetInstance()->CreateProcessFromMemory("TestProc", TestProc);
	

	SkyConsole::Print("Init Console....\n");

	Thread* pThread = pProcess->GetThread(0);
	pThread->m_taskState = TASK_STATE_RUNNING;

	int entryPoint = (int)pThread->frame.eip;
	unsigned int procStack = pThread->frame.esp;
	
	kLeaveCriticalSection(&g_criticalSection);

	JumpToNewKernelEntry(entryPoint, procStack);
}

void JumpToNewKernelEntry(int entryPoint, unsigned int procStack)
{
	__asm
	{
		mov     ax, 0x10;
		mov     ds, ax
		mov     es, ax
		mov     fs, ax
		mov     gs, ax

			//; create stack frame
			//; push   0x10;
		//; push procStack; stack
		mov     esp, procStack
		push	0x10;
		push    0x200; EFLAGS
		push    0x08; CS
		push    entryPoint; EIP
		iretd
	}
}


bool kFormat(void);
bool kFormat(void)
{
	HDDINFORMATION hddInfo;
	MBR* pstMBR;
	DWORD dwTotalSectorCount, dwRemainSectorCount;
	DWORD dwMaxClusterCount, dwClsuterCount;
	DWORD dwClusterLinkSectorCount;
	DWORD i;
	char tempBuffer[512];
	memset(tempBuffer, 0, 512);

	//==========================================================================
	//  하드 디스크 정보를 읽어서 메타 영역의 크기와 클러스터의 개수를 계산
	//==========================================================================
	// 하드 디스크의 정보를 얻어서 하드 디스크의 총 섹터 수를 구함
	if (kReadRDDInformation(TRUE, TRUE, &hddInfo) == FALSE)
	{

		return FALSE;
	}
	dwTotalSectorCount = hddInfo.dwTotalSectors;

	// 전체 섹터 수를 4Kbyte, 즉 클러스터 크기로 나누어 최대 클러스터 수를 계산
	dwMaxClusterCount = dwTotalSectorCount / FILESYSTEM_SECTORSPERCLUSTER;

	// 최대 클러스터의 수에 맞추어 클러스터 링크 테이블의 섹터 수를 계산
	// 링크 데이터는 4바이트이므로, 한 섹터에는 128개가 들어감. 따라서 총 개수를
	// 128로 나눈 후 올림하여 클러스터 링크의 섹터 수를 구함
	dwClusterLinkSectorCount = (dwMaxClusterCount + 127) / 128;

	// 예약된 영역은 현재 사용하지 않으므로, 디스크 전체 영역에서 MBR 영역과 클러스터
	// 링크 테이블 영역의 크기를 뺀 나머지가 실제 데이터 영역이 됨
	// 해당 영역을 클러스터 크기로 나누어 실제 클러스터의 개수를 구함
	dwRemainSectorCount = dwTotalSectorCount - dwClusterLinkSectorCount - 1;
	dwClsuterCount = dwRemainSectorCount / FILESYSTEM_SECTORSPERCLUSTER;

	// 실제 사용 가능한 클러스터 수에 맞추어 다시 한번 계산
	dwClusterLinkSectorCount = (dwClsuterCount + 127) / 128;

	//==========================================================================
	// 계산된 정보를 MBR에 덮어 쓰고, 루트 디렉터리 영역까지 모두 0으로 초기화하여
	// 파일 시스템을 생성
	//==========================================================================
	// MBR 영역 읽기
	if (kReadRDDSector(TRUE, TRUE, 0, 1, tempBuffer) == FALSE)
	{
		return FALSE;
	}

	pstMBR = (MBR*)tempBuffer;
	memset(pstMBR->vstPartition, 0, sizeof(pstMBR->vstPartition));
	pstMBR->dwSignature = FILESYSTEM_SIGNATURE;
	pstMBR->dwReservedSectorCount = 0;
	pstMBR->dwClusterLinkSectorCount = dwClusterLinkSectorCount;
	pstMBR->dwTotalClusterCount = dwClsuterCount;

	// MBR 영역에 1 섹터를 씀
	if (kWriteRDDSector(TRUE, TRUE, 0, 1, tempBuffer) == FALSE)
	{
		return FALSE;
	}
	memset(tempBuffer, 0, 512);
	// MBR 이후부터 루트 디렉터리까지 모두 0으로 초기화
	for (i = 0; i < (dwClusterLinkSectorCount + FILESYSTEM_SECTORSPERCLUSTER); i++)
	{
		// 루트 디렉터리(클러스터 0)는 이미 파일 시스템이 사용하고 있으므로,
		// 할당된 것으로 표시
		if (i == 0)
		{
			((DWORD*)(tempBuffer))[0] = FILESYSTEM_LASTCLUSTER;
		}
		else
		{
			((DWORD*)(tempBuffer))[0] = FILESYSTEM_FREECLUSTER;
		}

		// 1 섹터씩 씀
		if (kWriteRDDSector(TRUE, TRUE, i + 1, 1, tempBuffer) == FALSE)
		{
			return FALSE;
		}
	}

	return TRUE;
}

#pragma pack( push, 1 )

// 패키지의 시그너처
#define PACKAGESIGNATURE    "MINT64OSPACKAGE "

// 파일 이름의 최대 길이, 커널의 FILESYSTEM_MAXFILENAMELENGTH와 같음
#define MAXFILENAMELENGTH   24

// 파라미터를 처리하기위해 정보를 저장하는 자료구조
typedef struct kParameterListStruct
{
	// 파라미터 버퍼의 어드레스
	const char* pcBuffer;
	// 파라미터의 길이
	int iLength;
	// 현재 처리할 파라미터가 시작하는 위치
	int iCurrentPosition;
} PARAMETERLIST;

// 패키지 헤더 내부의 각 파일 정보를 구성하는 자료구조
typedef struct PackageItemStruct
{
	// 파일 이름
	char vcFileName[MAXFILENAMELENGTH];

	// 파일의 크기
	DWORD dwFileLength;
} PACKAGEITEM;

// 패키지 헤더 자료구조
typedef struct PackageHeaderStruct
{
	// MINT64 OS의 패키지 파일을 나타내는 시그너처
	char vcSignature[16];

	// 패키지 헤더의 전체 크기
	DWORD dwHeaderSize;

	// 패키지 아이템의 시작 위치
	PACKAGEITEM vstItem[0];
} PACKAGEHEADER;

#pragma pack( pop )

//  고수준 함수(High Level Function)
FILE* kOpenFile(const char* pcFileName, const char* pcMode);
DWORD kReadFile(void* pvBuffer, DWORD dwSize, DWORD dwCount, FILE* pstFile);
DWORD kWriteFile(const void* pvBuffer, DWORD dwSize, DWORD dwCount, FILE* pstFile);
int kSeekFile(FILE* pstFile, int iOffset, int iOrigin);
int kCloseFile(FILE* pstFile);
int kRemoveFile(const char* pcFileName);


// MINT 파일 시스템 함수를 표준 입출력 함수 이름으로 재정의
#define fopen       kOpenFile
#define fread       kReadFile
#define fwrite      kWriteFile
#define fseek       kSeekFile
#define fclose      kCloseFile
#define remove      kRemoveFile
#define opendir     kOpenDirectory
#define readdir     kReadDirectory
#define rewinddir   kRewindDirectory
#define closedir    kCloseDirectory

static void kInstallPackage(const char* pcParameterBuffer)
{
	PACKAGEHEADER* pstHeader;
	PACKAGEITEM* pstItem;
	WORD wKernelTotalSectorCount;
	int i;
	FILE* fp;
	UINT64 qwDataAddress;

	SkyConsole::Print("Package Install Start...\n");

	// 부트 로더가 로딩된 0x7C05 어드레스에서 보호 모드 커널과 IA-32e 모드 커널을
	// 합한 섹터 수를 읽음
	wKernelTotalSectorCount = *((WORD*)0x7C05);

	// 디스크 이미지는 0x10000 어드레스에 로딩되므로 이를 기준으로
	// 커널 섹터 수만큼 떨어진 곳에 패키지 헤더가 있음
	pstHeader = (PACKAGEHEADER*)((UINT64)0x10000 + wKernelTotalSectorCount * 512);

	// 시그너처를 확인
	if (memcmp(pstHeader->vcSignature, PACKAGESIGNATURE,
		sizeof(pstHeader->vcSignature)) != 0)
	{
		SkyConsole::Print("Package Signature Fail\n");
		return;
	}

	//--------------------------------------------------------------------------
	// 패키지 내의 모든 파일을 찾아서 하드 디스크에 복사
	//--------------------------------------------------------------------------
	// 패키지 데이터가 시작하는 어드레스
	qwDataAddress = (UINT64)pstHeader + pstHeader->dwHeaderSize;
	// 패키지 헤더의 첫 번째 파일 데이터
	pstItem = pstHeader->vstItem;

	// 패키지에 포함된 모든 파일을 찾아서 복사
	for (i = 0; i < pstHeader->dwHeaderSize / sizeof(PACKAGEITEM); i++)
	{
		SkyConsole::Print("[%d] file: %s, size: %d Byte\n", i + 1, pstItem[i].vcFileName,
			pstItem[i].dwFileLength);

		// 패키지에 포함된 파일 이름으로 파일을 생성
		fp = fopen(pstItem[i].vcFileName, "w");
		if (fp == NULL)
		{
			SkyConsole::Print("%s File Create Fail\n");
			return;
		}

		// 패키지 데이터 부분에 포함된 파일 내용을 하드 디스크로 복사
		if (fwrite((BYTE*)qwDataAddress, 1, pstItem[i].dwFileLength, fp) !=
			pstItem[i].dwFileLength)
		{
			SkyConsole::Print("Write Fail\n");

			// 파일을 닫고 파일 시스템 캐시를 내보냄
			fclose(fp);

			return;
		}

		// 파일을 닫음        
		fclose(fp);

		// 다음 파일이 저장된 위치로 이동
		qwDataAddress += pstItem[i].dwFileLength;
	}

	SkyConsole::Print("Package Install Complete\n");

}


