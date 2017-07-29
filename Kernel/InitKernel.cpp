#include "InitKernel.h"
#include "Video.h"
#include "kybrd.h"
#include "string.h"
#include "sprintf.h"
#include "RTC.H"
#include "Console.h"
#include "flpydsk.h"
#include "fat12.h"
#include "exception.h"
#include "vesa.h"
#include "HardDisk.h"
#include "ZetPlane.h"

bool InitKeyboard()
{
	kkybrd_install(33);

	/*setvect(33, kKeyboardHandler);

	// 키보드를 활성화
	if (kInitializeKeyboard() == TRUE)
	{
	kChangeKeyboardLED(FALSE, FALSE, FALSE);
	}*/

	return true;
}

HardDiskHandler* __SysHDDHandler;

void PrintHDDInfo()
{
	int TotHDD = __SysHDDHandler->GetTotalDevices();
	__HDDInfo * HDDInfo;
	BYTE Key[3] = { 'H','0',0
	};

	for (BYTE i = 0; i<TotHDD; i++)
	{
		HDDInfo = (struct __HDDInfo *)__SysHDDHandler->GetHDDInfo(Key);
		if (HDDInfo != NULL)
		{
			UINT16 i;
			SkyConsole::Print("\n%s Device ( %s ) :: ", HDDInfo->DeviceNumber ? "Slave " : "Master", Key);
			if (HDDInfo->ModelNumber[0] == 0)
				SkyConsole::Print(" N/A ");
			else
				for (i = 0; i<20; i++)
					SkyConsole::WriteChar(HDDInfo->ModelNumber[i]);
			SkyConsole::Print(" - ");
			if (HDDInfo->SerialNumber[0] == 0)
				SkyConsole::Print(" N/A ");
			else
				for (BYTE i = 0; i<20; i++)
					SkyConsole::WriteChar(HDDInfo->SerialNumber[i]);
			SkyConsole::Print("\n\r Cylinders %d Heads %d Sectors %d. LBA Sectors %ld\n", HDDInfo->CHSCylinderCount, HDDInfo->CHSHeadCount, HDDInfo->CHSSectorCount, HDDInfo->LBACount);
		}
		Key[1]++;
	}
}

bool InitHardDrive()
{
	__SysHDDHandler = new HardDiskHandler();
	__SysHDDHandler->Initialize();

	/*if (kInitializeHDD() == TRUE)
	{
		SkyConsole::Print("AAAAAAAAAAAAAAAAAAAAAAAAA\n");
	}*/

	SkyConsole::Print("HardDisk Count : %d\n", __SysHDDHandler->GetTotalDevices());

	PrintHDDInfo();
	

	return true;
}

bool DumpSystemInfo()
{
	char str[256];
	memset(str, 0, 256);

	SYSTEMTIME sysTime;
	GetLocalTime(&sysTime);

	sprintf(str, "LocalTime : %d %d/%d %d:%d %d\n", sysTime.wYear, sysTime.wMonth, sysTime.wDay, sysTime.wHour, sysTime.wMinute, sysTime.wSecond);
	SkyConsole::Print("%s", str);

	/*for (int i = 0; i < 100; i++)
	{
	ZetPlane* pZetPlane = new ZetPlane();
	pZetPlane->m_rotation = 50;
	SkyConsole::Print("ZetPlane Address : 0x%x\n", pZetPlane);
	SkyConsole::Print("ZetPlane m_rotation : %d\n", pZetPlane->m_rotation);
	}*/

	return true;
}

bool InitGraphics(multiboot_info* pInfo)
{
	/* set video mode and map framebuffer. */
	//VbeBochsSetMode(WIDTH, HEIGHT, BPP);
	//VbeBochsMapLFB();
	//fillScreen32();

	init_lfb(pInfo);
	lfb_clear();

	return true;
}

extern bool FddInitializeDriver(VOID);
extern BOOL FsInitializeModule(VOID);

#include "flpydsk.h"
void InitFloppyDrive()
{
	//FddInitializeDriver();

	//flpydsk_install(38);

	//FsInitializeModule();
	//! set drive 0 as current drive
	flpydsk_set_working_drive(0);

	//! install floppy disk to IR 38, uses IRQ 6
	flpydsk_install(38);
	
	//! initialize FAT12 filesystem
	fsysFatInitialize();
}

__declspec(naked) void  _cdecl HandleInterrupt()
{
	_asm {
		pushad
	}

	_asm {
		mov al, 0x20
		out 0x20, al
		popad
		iretd
	}
}

void SetInterruptVector()
{
	setvect(0, (void(__cdecl &)(void))divide_by_zero_fault);
	setvect(1, (void(__cdecl &)(void))single_step_trap);
	setvect(2, (void(__cdecl &)(void))nmi_trap);
	setvect(3, (void(__cdecl &)(void))breakpoint_trap);
	setvect(4, (void(__cdecl &)(void))overflow_trap);
	setvect(5, (void(__cdecl &)(void))bounds_check_fault);
	setvect(6, (void(__cdecl &)(void))invalid_opcode_fault);
	setvect(7, (void(__cdecl &)(void))no_device_fault);
	setvect(8, (void(__cdecl &)(void))double_fault_abort);
	setvect(10, (void(__cdecl &)(void))invalid_tss_fault);
	setvect(11, (void(__cdecl &)(void))no_segment_fault);
	setvect(12, (void(__cdecl &)(void))stack_fault);
	setvect(13, (void(__cdecl &)(void))general_protection_fault);
	setvect(14, (void(__cdecl &)(void))page_fault);
	setvect(16, (void(__cdecl &)(void))fpu_fault);
	setvect(17, (void(__cdecl &)(void))alignment_check_fault);
	setvect(18, (void(__cdecl &)(void))machine_check_abort);
	setvect(19, (void(__cdecl &)(void))simd_fpu_fault);

	setvect(33, (void(__cdecl &)(void))HandleInterrupt);
	setvect(38, (void(__cdecl &)(void))HandleInterrupt);

	//i86_install_ir(SYSTEM_TMR_INT_NUMBER, I86_IDT_DESC_PRESENT | I86_IDT_DESC_BIT32 | 0x0500, 0x8, (I86_IRQ_HANDLER)TMR_TSS_SEG);
}