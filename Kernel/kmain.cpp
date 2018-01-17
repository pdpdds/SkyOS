#include "kmain.h"

extern bool systemOn;
BOOL kFormat(void);
#include "stdio.h"
#include "Vector.h"
#include "Stack.h"
#include "jsmn.h"
#include "map.hpp"
#include "List1.h"
#include "TestInteger.h"

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

void TestV8086();

#define printf SkyConsole::Print

static const char *JSON_STRING =
"{\"user\": \"johndoe\", \"admin\": false, \"uid\": 1000,\n  "
"\"groups\": [\"users\", \"wheel\", \"audio\", \"video\"]}";

static int jsoneq(const char *json, jsmntok_t *tok, const char *s) {
	if (tok->type == JSMN_STRING && (int)strlen(s) == tok->end - tok->start &&
		strncmp(json + tok->start, s, tok->end - tok->start) == 0) {
		return 0;
	}
	return -1;
}

void TestMap();
void TestList();
void TestCPP14();



void kmain(unsigned long magic, unsigned long addr)
{
	InitializeConstructors();

	SkyConsole::Initialize();
	//헥사를 표시할 때 %X는 integer, %x는 unsigned integer의 헥사값을 표시한다.	
	SkyConsole::Print("*** Sky OS Console System Init\n");

	EnterCriticalSection();	
	
	i86_gdt_initialize();			
	i86_idt_initialize(0x8);		
	i86_pic_initialize(0x20, 0x28);
	i86_pit_initialize();
											
	SetInterruptVector();	
	InitializeSysCall();
	//! initialize TSS
	install_tss(5, 0x10, 0);

	InitKeyboard();

	multiboot_info* pBootInfo = (multiboot_info*)addr;
	InitMemoryManager(pBootInfo, 0);	

	InitFloppyDrive();
			
	i86_pit_start_counter(100, I86_PIT_OCW_COUNTER_0, I86_PIT_OCW_MODE_SQUAREWAVEGEN);

	__asm sti
	if (true == InitHardDrive())
	{
		InitFATFileSystem();
#ifdef _SKY_DEBUG
		TestHardDrive();
#endif
	}

	// 하드 디스크 초기화가 실패하면 8Mbyte 크기의 램 디스크를 생성
	// 힙영역을 크게 잡아야 한다.
	/*if (kInitializeRDD(RDD_TOTALSECTORCOUNT) != FALSE || kFormat())
	{
		HaltSystem("Ram Disk Create Fail");
	}*/

	//Initialize FILESYSTEM
	//VFSInitialize();

	__asm cli

	LeaveCriticalSection();
	

	DumpSystemInfo(pBootInfo);
	SkyConsole::Print("Boot Loader Name : %s\n", (char*)pBootInfo->boot_loader_name);

	/*Vector<int> vec;
	vec.push_back(5);
	vec.push_back(1);
	vec.push_back(3);
	vec.push_back(4);



	while (vec.size() > 0)
	{
		int data = vec.back();
		SkyConsole::Print("%d\n", data);
		vec.pop_back();
	}

	for (;;);
	*/

	/*
	Stack<int> s;
	s.push(10);
	s.push(20);
	s.push(30);

	while (s.size() > 0)
	{
		int data = s.pop();
		SkyConsole::Print("%d\n", data);

	}*/


	//TestMap();
	TestList();
	TestCPP14();

	int i;
	int r;
	jsmn_parser p;
	jsmntok_t t[128]; /* We expect no more than 128 tokens */

	jsmn_init(&p);
	r = jsmn_parse(&p, JSON_STRING, strlen(JSON_STRING), t, sizeof(t) / sizeof(t[0]));
	if (r < 0) {
		printf("Failed to parse JSON: %d\n", r);
		return;
	}

	/* Assume the top-level element is an object */
	if (r < 1 || t[0].type != JSMN_OBJECT) {
		printf("Object expected\n");
		return;
	}

	char buf[256];
	memset(buf, 0, 256);


	/* Loop over all keys of the root object */
	for (i = 1; i < r; i++) {
		if (jsoneq(JSON_STRING, &t[i], "user") == 0) {
			/* We may use strndup() to fetch string value */
			memcpy(buf, JSON_STRING + t[i + 1].start, t[i + 1].end - t[i + 1].start);
			printf("- User: %s\n", buf);
			i++;
		}
		/*else if (jsoneq(JSON_STRING, &t[i], "admin") == 0) {
			
			printf("- Admin: %.*s\n", t[i + 1].end - t[i + 1].start,
				JSON_STRING + t[i + 1].start);
			i++;
		}
		else if (jsoneq(JSON_STRING, &t[i], "uid") == 0) {
			
			printf("- UID: %.*s\n", t[i + 1].end - t[i + 1].start,
				JSON_STRING + t[i + 1].start);
			i++;
		}
		else if (jsoneq(JSON_STRING, &t[i], "groups") == 0) {
			int j;
			printf("- Groups:\n");
			if (t[i + 1].type != JSMN_ARRAY) {
				continue;
			}
			for (j = 0; j < t[i + 1].size; j++) {
				jsmntok_t *g = &t[i + j + 2];
				printf("  * %.*s\n", g->end - g->start, JSON_STRING + g->start);
			}
			i += t[i + 1].size + 1;
		}
		else {
			printf("Unexpected key: %.*s\n", t[i].end - t[i].start,
				JSON_STRING + t[i].start);
		}*/
	}

	for (;;);

	//TestV8086();
	
		
		bool consoleMode = true;

	if (consoleMode == true)
	{
		StartConsoleSystem();
	}
	else
	{
		//InitGraphics(pBootInfo);
	}
}

void TestMap()
{
	std::map<int, int> m10;
	std::map<int, int>::iterator it1;
	std::map<int, int>::iterator it2;

	m10[1] = 2;
	m10[2] = 4;
	m10[3] = 6;
	m10[4] = 8;
	m10[5] = 10;
	m10[6] = 12;
	m10[7] = 14;
	m10[8] = 16;
	m10[8] = 18;
	int i = 0;
	for (it1 = m10.begin(); it1 != m10.end(); it1++) {
		//cout << (*it1).first << "   " << (*it1).second << endl;
		printf("%d  %d\n", (*it1).first, (*it1).second);
		i++;
	}

	//cout << "ERASE BY KEY" << endl;
	m10.erase(3);
	i = 0;
	for (it1 = m10.begin(); it1 != m10.end(); it1++) {
		//cout << (*it1).first << "   " << (*it1).second << endl;
		printf("%d  %d\n", (*it1).first, (*it1).second);
		i++;
	}

	for (;;);
}

void TestCPP14()
{
	auto func = [x = 5]() { return x; };   
	auto n1 = func();     
	printf("Lamda n1:%d\n", n1);

	constexpr TestInteger size(10);
	int x[size];  
	x[3] = 11; 
	printf("constexor x[3]:%d\n", x[3]);
}

void TestList()
{
	
		std::List<int> fstList = std::List<int>();
		std::List<int> scndList = std::List<int>();
		int counter = 0;

		for (int i = 0; i <= 10; ++i) {
			fstList.push_back(i);
		}
		std::List<int>::Iterator iter = fstList.begin();
		while (iter != fstList.end())
		{
			printf("item %d  done\n", *iter);
			iter++;
		}

		printf("done!!\n");

		/*while (1) {
			for (int i = 0; i <= 10; ++i) {
				fstList.push_back(i);
			}

			for (int i = 6; i <= 15; ++i) {
				scndList.push_front(i);
			}

			std::List<int>::Iterator iter = fstList.begin();

			fstList.splice(++iter, scndList);
			scndList.concat(fstList);
			fstList.concat(scndList);

			fstList.pop_back();
			fstList.clear();
			//std::cout << "Cycle # " << counter << " done" << std::endl;
			printf("Cycle %d  done\n", counter);
			++counter;
		}*/
	

}

uint32_t GetFreeSpaceMemory(multiboot_info* bootinfo)
{
	uint32_t memorySize = 0;
	uint32_t mmapEntryNum = bootinfo->mmap_length / sizeof(multiboot_memory_map_t);

	SkyConsole::Print("GRUB Impormation\n");
	SkyConsole::Print("Boot Loader Name : %s\n", (char*)bootinfo->boot_loader_name);
	SkyConsole::Print("Memory Map Entry Num : %d\n", mmapEntryNum);

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
	//PhysicalMemoryManager::Dump();
	
//가상 메모리 매니저 초기화	
	VirtualMemoryManager::Initialize();	

	//커널 힙을 생성한다
	VirtualMemoryManager::CreateKernelHeap();

	return true;
}

void StartConsoleSystem()
{		
	EnterCriticalSection();

	Process* pProcess = ProcessManager::GetInstance()->CreateConsoleProcess(SystemConsoleProc);

	ProcessManager::GetInstance()->CreateProcessFromMemory(WatchDogProc);

	ProcessManager::GetInstance()->CreateProcessFromMemory(ProcessRemoverProc);

	if (pProcess == nullptr)
		HaltSystem("Console Creation Fail!!");

	
	SkyConsole::Print("Init Console....\n");

	Thread* pThread = pProcess->GetThread(0);
	pThread->m_taskState = TASK_STATE_RUNNING;	

	int entryPoint = (int)pThread->frame.eip;
	unsigned int procStack = pThread->frame.esp;

	JumpToNewKernelEntry(entryPoint, procStack);

//쓸모없는 코드지만 쌍을 맞추기 위해
	LeaveCriticalSection();
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
			
		; create stack frame			
		; push   0x10;
		; push procStack; stack
		mov esp, procStack
		push	0x10;
		push    0x200; EFLAGS
		push    0x08; CS
		push entryPoint; EIP
		sti
		iretd
	}
}



BOOL kFormat(void)
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
	for (i = 0; i < (dwClusterLinkSectorCount + FILESYSTEM_SECTORSPERCLUSTER);i++)
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

#include "./v8086/rme.h"
#include "vesa.h"
#include "Math.h"

#define PROMPT_FOR_MODE 1

/* Friggin' frick, this should be a config option
* because it's 4096 on some instances of Qemu,
* ie the one on my laptop, but it's 2048 on
* the EWS machines. */
#define BOCHS_BUFFER_SIZE 2048
#define PREFERRED_VY 4096
#define PREFERRED_B 32

uint16_t bochs_resolution_x = 0;
uint16_t bochs_resolution_y = 0;
uint16_t bochs_resolution_b = 0;


/* vm86 Helpers */
typedef uint32_t  FARPTR;
typedef uintptr_t addr_t;
#define MK_FP(seg, off)        ((FARPTR) (((uint32_t) (seg) << 16) | (uint16_t) (off)))
#define FP_SEG(fp)             (((FARPTR) fp) >> 16)
#define FP_OFF(fp)             (((FARPTR) fp) & 0xffff)
#define FP_TO_LINEAR(seg, off) ((void*) ((((uint16_t) (seg)) << 4) + ((uint16_t) (off))))
#define LINEAR_TO_FP(ptr)      (MK_FP(((addr_t) (ptr) - ((addr_t) (ptr) & 0xf)) / 16, ((addr_t)(ptr) & 0xf)))

uint8_t * bochs_vid_memory = (uint8_t *)0xE0000000;

static void finalize_graphics(uint16_t x, uint16_t y, uint16_t b) {
	bochs_resolution_x = x;
	bochs_resolution_y = y;
	bochs_resolution_b = b;
}

void graphics_install_vesa(uint16_t resX, uint16_t resY) 
{
	/* VESA Structs */
	struct VesaControllerInfo *info = (VesaControllerInfo*)0x10000;
	struct VesaModeInfo *modeinfo = (VesaModeInfo*)0x9000;

	/* 8086 Emulator Status */
	tRME_State *emu;
	void * lowCache;
	lowCache = (void *)new BYTE[RME_BLOCK_SIZE];
	memcpy(lowCache, NULL, RME_BLOCK_SIZE);
	emu = RME_CreateState();
	emu->Memory[0] = (uint8_t*)lowCache;
	for (int i = RME_BLOCK_SIZE; i < 0x100000; i += RME_BLOCK_SIZE) {
		emu->Memory[i / RME_BLOCK_SIZE] = (uint8_t*)i;
	}
	int ret, mode;

	/* Find modes */
	uint16_t * modes;
	memcpy(info->Signature, "VBE2", 4);
	emu->AX.W = 0x4F00;
	emu->ES = 0x1000;
	emu->DI.W = 0;
	ret = RME_CallInt(emu, 0x10);
	if (info->Version < 0x200 || info->Version > 0x300)
	{
		SkyConsole::Print("\033[JYou have attempted to use the VESA/VBE2 driver\nwith a card that does not support VBE2.\n");
		SkyConsole::Print("\nSystem responded to VBE request with version: 0x%x\n", info->Version);

		HaltSystem("");
	}
	modes = (uint16_t*)FP_TO_LINEAR(info->Videomodes.Segment, info->Videomodes.Offset);

	uint16_t best_x = 0;
	uint16_t best_y = 0;
	uint16_t best_b = 0;
	uint16_t best_mode = 0;

	for (int i = 1; modes[i] != 0xFFFF; ++i) {
		emu->AX.W = 0x4F01;
		emu->CX.W = modes[i];
		emu->ES = 0x0900;
		emu->DI.W = 0x0000;
		RME_CallInt(emu, 0x10);
#if PROMPT_FOR_MODE
		SkyConsole::Print("%d = %dx%d:%d %d\n", i, modeinfo->Xres, modeinfo->Yres, modeinfo->bpp, modeinfo->physbase);
	}

	SkyConsole::Print("Please select a mode: ");
	KeyBoard::KEYCODE key = SkyConsole::GetChar();
	char selected = KeyBoard::ConvertKeyToAscii(key);
	char buf[10];
	buf[0] = selected;
	buf[1] = '\n';

	mode = atoi(buf);
#else
		if ((abs(modeinfo->Xres - resX) < abs(best_x - resX)) && (abs(modeinfo->Yres - resY) < abs(best_y - resY))) {
			best_mode = i;
			best_x = modeinfo->Xres;
			best_y = modeinfo->Yres;
			best_b = modeinfo->bpp;
		}
}
	for (int i = 1; modes[i] != 0xFFFF; ++i) {
		emu->AX.W = 0x4F01;
		emu->CX.W = modes[i];
		emu->ES = 0x0900;
		emu->DI.W = 0x0000;
		RME_CallInt(emu, 0x10);
		if (modeinfo->Xres == best_x && modeinfo->Yres == best_y) {
			if (modeinfo->bpp > best_b) {
				best_mode = i;
				best_b = modeinfo->bpp;
			}
		}
	}

	if (best_b < 24) {
		SkyConsole::Print("!!! Rendering at this bit depth (%d) is not currently supported.\n", best_b);
		HaltSystem("");
	}

	mode = best_mode;

#endif

	emu->AX.W = 0x4F01;
	if (mode < 100) {
		emu->CX.W = modes[mode];
	}
	else {
		emu->CX.W = mode;
	}
	emu->ES = 0x0900;
	emu->DI.W = 0x0000;
	RME_CallInt(emu, 0x10);

	HaltSystem("sadsd");
	
	emu->AX.W = 0x4F02;
	emu->BX.W = modes[mode];
	RME_CallInt(emu, 0x10);
	
	uint16_t actual_x = modeinfo->Xres;
	uint16_t actual_y = modeinfo->Yres;
	uint16_t actual_b = modeinfo->bpp;

	bochs_vid_memory = (uint8_t *)modeinfo->physbase;

	/*uint32_t* lfb = (uint32_t*)bochs_vid_memory;
	for (uint32_t c = 0; c<actual_x*actual_y; c++)
		lfb[c] = 0x90;

SkyConsole::GetChar();*/

	/*int* lfb = (int*)bochs_vid_memory;
	for (int j = 0; j < actual_x; j++)
	for (int k = 0; k < actual_y; k++)
			lfb[k + (j) * actual_x] = 255;*/

	/*for (;;);

	if(bochs_vid_memory == 0)
		HaltSystem("Sdfdsdfd");*/
	
	/*if (!bochs_vid_memory) 
	{
		uint32_t * herp = (uint32_t *)0xA0000;
		herp[0] = 0xA5ADFACE;

		// Enable the higher memory 
		for (uintptr_t i = 0xE0000000; i <= 0xE0FF0000; i += 0x1000) 
		{

			dma_frame(get_page(i, 1, kernel_directory), 0, 1, i);
		}
		for (uintptr_t i = 0xF0000000; i <= 0xF0FF0000; i += 0x1000) {
			dma_frame(get_page(i, 1, kernel_directory), 0, 1, i);
		}


		// Go find it 
		for (uintptr_t x = 0xE0000000; x < 0xE0FF0000; x += 0x1000) {
			if (((uintptr_t *)x)[0] == 0xA5ADFACE) {
				bochs_vid_memory = (uint8_t *)x;
				goto mem_found;
			}
		}
		for (uintptr_t x = 0xF0000000; x < 0xF0FF0000; x += 0x1000) {
			if (((uintptr_t *)x)[0] == 0xA5ADFACE) {
				bochs_vid_memory = (uint8_t *)x;
				goto mem_found;
			}
		}
	}*/
mem_found:

	/*
	* Finalize the graphics setup with the actual selected resolution.
	*/
	finalize_graphics(actual_x, actual_y, actual_b);

	//InitGraphics(modeinfo);
}


	void rect32A(int x, int y, int w, int h, int col) {
		int* lfb = (int*)bochs_vid_memory;
		for (int k = 0; k < h; k++)
			for (int j = 0; j < w; j++)
				lfb[(j + x) + (k + y) * bochs_resolution_x] = col;
	}


void TestV8086()
{
	graphics_install_vesa(1024, 768);

	

	//HaltSystem("ff");

	int col = 0;
	bool dir = true;
	SkyConsole::Print("RectGenerate\n");
	while (1) {
		rect32A(200, 380, 100, 100, 0x80);
		if (dir) {
			if (col++ == 0xfe)
				dir = false;
		}
		else
			if (col-- == 1)
				dir = true;
	}
}