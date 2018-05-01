#pragma once
#include "MultiBoot.h"

class SkyGUISystem
{
public:	
	~SkyGUISystem();

	bool Initialize(multiboot_info* pBootInfo);

	static SkyGUISystem* GetInstance()
	{
		if (m_GUISystem == nullptr)
			m_GUISystem = new SkyGUISystem();

		return m_GUISystem;
	}

	bool GUIEnable() { return m_GUIEnable; }

private:
	SkyGUISystem();
	static SkyGUISystem* m_GUISystem;
	bool m_GUIEnable;
};
