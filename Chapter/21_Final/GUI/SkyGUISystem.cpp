#include "SkyOS.h"

SkyGUISystem* SkyGUISystem::m_GUISystem = nullptr;

SkyGUISystem::SkyGUISystem()
{
	m_GUIEnable = false;
}

SkyGUISystem::~SkyGUISystem()
{
}

bool SkyGUISystem::Initialize(multiboot_info* pBootInfo)
{
	if (strcmp(pBootInfo->boot_loader_name, "GNU GRUB 0.95") == 0)
		return false;
	else
	{			
		if (pBootInfo->framebuffer_addr != 0)
		{
			VirtualMemoryManager::CreateVideoDMAVirtualAddress(VirtualMemoryManager::GetCurPageDirectory(), pBootInfo->framebuffer_addr, pBootInfo->framebuffer_addr, pBootInfo->framebuffer_addr + VIDEO_RAM_LOGICAL_ADDRESS_OFFSET);

			m_videoRamInfo._pVideoRamPtr = (void*)pBootInfo->framebuffer_addr;
			m_videoRamInfo._width = pBootInfo->framebuffer_width;
			m_videoRamInfo._height = pBootInfo->framebuffer_height;
			m_videoRamInfo._bpp = pBootInfo->framebuffer_bpp;
			m_videoRamInfo._framebuffer_type = pBootInfo->framebuffer_type;

			m_pWindow = new SkyWindow<SKY_GUI_SYSTEM>();
			if (m_pWindow == nullptr)
				return false;

			m_pWindow->Initialize(m_videoRamInfo._pVideoRamPtr, m_videoRamInfo._width, m_videoRamInfo._height, m_videoRamInfo._bpp, m_videoRamInfo._framebuffer_type);
			m_GUIEnable = true;
			
			return true;
		}
	}

	m_GUIEnable = false;
	return false;
}

bool SkyGUISystem::Run()
{	
	if(m_pWindow)
		m_pWindow->Run();

	return true;
}

bool SkyGUISystem::Print(char* pMsg)
{
	bool result = false;
	if (m_pWindow)
		result = m_pWindow->Print(pMsg);

	return result;
}