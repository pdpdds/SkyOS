#pragma once
//콘솔모드로 시작할지 그래픽 모드로 시작할지 결정
//이값이 1로 설정되면 아래 값들은 모두 무시된다.
#define SKY_CONSOLE_MODE	0
//SkyGUIConsole
#define SKY_GUI_SYSTEM	SkyGUIConsole
#define SKY_WIDTH		1024
#define SKY_HEIGHT		768
#define SKY_BPP			32

//세가지 GUI 모드
//SkyGUI
/*#define SKY_GUI_SYSTEM	SkyGUI
#define SKY_WIDTH		1024
#define SKY_HEIGHT		768	
#define SKY_BPP			8*/

//SVGA GUI
/*#define SKY_GUI_SYSTEM	SkySVGA
#define SKY_WIDTH		1024
#define SKY_HEIGHT		768
#define SKY_BPP			32*/

//GUI MINT64
/*#define SKY_GUI_SYSTEM	GUIMint64
#define SKY_WIDTH		1024
#define SKY_HEIGHT		768
#define SKY_BPP			16*/


#define KERNEL32_NAME		"SKYOS32_EXE"
#define KERNEL64_NAME		"SKYOS64_SYS"