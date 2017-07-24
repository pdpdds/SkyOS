#include "InitKernel.h"
#include "Video.h"
#include "kybrd.h"
#include "Keyboard.h"
#include "string.h"
#include "sprintf.h"
#include "RTC.H"
#include "Console.h"

bool InitKeyboard()
{
	kkybrd_install(33);

	//InitializeFloppyDrive();

	/*setvect(33, kKeyboardHandler);

	// 키보드를 활성화
	if (kInitializeKeyboard() == TRUE)
	{
	kChangeKeyboardLED(FALSE, FALSE, FALSE);
	}*/

	return true;
}

bool InitHardDrive()
{
	//HardDiskHandler hardHandler;
	//hardHandler.Initialize();		

	/*if (kInitializeHDD() == TRUE)
	{
	SkyConsole::Print("AAAAAAAAAAAAAAAAAAAAAAAAA\n");
	}*/

	//SkyConsole::Print("HardDisk Count : %d\n", hardHandler.GetTotalDevices());
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

bool InitGraphics()
{
	/* set video mode and map framebuffer. */
	/*VbeBochsSetMode(WIDTH, HEIGHT, BPP);
	VbeBochsMapLFB();
	fillScreen32();*/

	return true;
}