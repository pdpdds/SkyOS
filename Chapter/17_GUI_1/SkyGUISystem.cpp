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
	/*uint16_t *graphics_memory = (uint16_t *)0xA0000;

	graphics_memory[0] = 0xff;
	graphics_memory[1] = 0xff;
	graphics_memory[2] = 0xff;
	*/

	if (strcmp(pBootInfo->boot_loader_name, "GNU GRUB 0.95") == 0)
		return false;
	else
	{
		if (m_GUIEnable == false)
			return false;

		if (pBootInfo->framebuffer_addr != 0)
		{
			VirtualMemoryManager::CreateVideoDMAVirtualAddress(0xFD000000, pBootInfo->framebuffer_addr, 0xFE000000);
			unsigned* framebuffer = (unsigned*)(0xFD000000);
			for (int y = 0; y < 768; ++y)
			{
				for (int x = 0; x < 1024; ++x)
				{
					framebuffer[y * 1024 + x] = 0xFFAAAA00; //³ë¶õ»ö
				}
			}
		}

		VideoRamInfo ramInfo = VideoRam::GetInstance()->GetVideoRamInfo();
		ramInfo._pVideoRamPtr = (void*)pBootInfo->framebuffer_addr;
		ramInfo._width = pBootInfo->framebuffer_width;
		ramInfo._height = pBootInfo->framebuffer_height;
		ramInfo._bpp = pBootInfo->framebuffer_bpp;

		VideoRam::GetInstance()->SetVideoRamInfo(ramInfo);
		m_GUIEnable = true;

		return true;
	}

	return false;
}