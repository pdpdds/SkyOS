#pragma once
#include "MultiBoot.h"
#include "SkyGUI.h"
#include "SkyGUIConsole.h"
#include "SkySVGA.h"
#include "GUIMint64.h"
#include "SkyMockInterface.h"
#include "I_HangulEngine.h"
#include "I_Hangul.h"

typedef I_HangulEngine*(*PHangulInput)();
typedef I_Hangul*(*PGetHangulEngine)();

typedef struct tagVideoRamInfo
{
	void* _pVideoRamPtr;
	int _width;
	int _height;
	int _bpp;
	uint8_t _framebuffer_type;

	tagVideoRamInfo()
	{
		_pVideoRamPtr = nullptr;
		_width = 0;
		_height = 0;
		_bpp = 0;
		_framebuffer_type = 0;
	}

}VideoRamInfo;

class SkyGUISystem
{
public:	
	~SkyGUISystem();

	bool Initialize(multiboot_info* pBootInfo);
	bool InitializeWithConsole();
	bool InitGUIModule();
	bool Run();
	bool Print(char* pMsg);
	bool Clear();

	static SkyGUISystem* GetInstance()
	{
		if (m_GUISystem == nullptr)
			m_GUISystem = new SkyGUISystem();

		return m_GUISystem;
	}

	bool GUIEnable() { return m_GUIEnable; }

	VideoRamInfo& GetVideoRamInfo() { return m_videoRamInfo; }
	void  SetVideoRamInfo(VideoRamInfo& info) { m_videoRamInfo = info; }

	I_HangulEngine* GetIMEEngine() { return m_pInputEngine; }
	I_Hangul* GetUnicodeEngine() { return m_pEngine; }

	bool LoadGUIModule();
protected:
	

private:
	SkyGUISystem();
	static SkyGUISystem* m_GUISystem;

	bool m_GUIEnable;
	VideoRamInfo m_videoRamInfo;

	SkyWindow<SKY_GUI_SYSTEM>* m_pWindow;

	I_HangulEngine* m_pInputEngine;
	I_Hangul* m_pEngine;

};
