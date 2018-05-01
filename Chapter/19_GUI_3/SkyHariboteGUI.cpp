#include "SkyHariboteGUI.h"
#include "HariboteGUI.h"
#include "FontData.h"
#include "memory.h"
#include "SkySimpleGUI.h"

extern char hankaku[4096];

SkyHariboteGUI::SkyHariboteGUI()
{
}


SkyHariboteGUI::~SkyHariboteGUI()
{
}

bool SkyHariboteGUI::Initialize(void* pVideoRamPtr, int width, int height, int bpp)
{
	m_pVideoRamPtr = (ULONG*)pVideoRamPtr;
	m_width = width;
	m_height = height;
	m_bpp = bpp;

	LoadFontFromMemory();

	m_pHaribote = new HariboteGUI();
	m_pHaribote->Init((char*)0xFD000000, 1024, 768);
	

	return true;
}

bool SkyHariboteGUI::LoadFontFromMemory()
{
	unsigned char* buffer = (unsigned char*)hankaku;
	int bufferIndex = 0;
	int charIndex = 0;

	memset(buffer, 0, 4096);

	int readIndex = 0;

	while (readIndex < 32768) {


		for (int i = 0; i < 512; i++)
		{
			if (fontData[readIndex + i] == '*')
			{

				if (charIndex < 8)
				{
					char a = (char)(1 << (8 - 1 - charIndex));
					buffer[bufferIndex] |= a;
				}



			}

			if ((fontData[readIndex + i] == '*') || (fontData[readIndex + i] == '.'))
			{
				charIndex++;

				if (charIndex >= 8)
				{
					bufferIndex++;
					charIndex = 0;
				}
			}
		}

		readIndex += 512;

	}

	return true;
}

bool SkyHariboteGUI::Run()
{
	m_pHaribote->Run();

	for (;;);
	return false;
}