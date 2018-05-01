#include "SkyGUISystem.h"
#include "VideoRam.h"
#include "VirtualMemoryManager.h"
#include "string.h"

SkyGUISystem* SkyGUISystem::m_GUISystem = nullptr;

SkyGUISystem::SkyGUISystem()
{
#ifdef SKY_GUI
	m_GUIEnable = true;
#else
	m_GUIEnable = false;
#endif	
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
			VirtualMemoryManager::CreateVideoDMAVirtualAddress(VirtualMemoryManager::GetCurPageDirectory(), VIDEO_RAM_LOGICAL_ADDRESS, pBootInfo->framebuffer_addr, 0xFE000000);			

			VideoRamInfo ramInfo = VideoRam::GetInstance()->GetVideoRamInfo();
			ramInfo._pVideoRamPtr = (void*)pBootInfo->framebuffer_addr;
			ramInfo._width = pBootInfo->framebuffer_width;
			ramInfo._height = pBootInfo->framebuffer_height;
			ramInfo._bpp = pBootInfo->framebuffer_bpp;

			VideoRam::GetInstance()->SetVideoRamInfo(ramInfo);
			return true;
		}		
	}

	m_GUIEnable = false;
	return false;
}