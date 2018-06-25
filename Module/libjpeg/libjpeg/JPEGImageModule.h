#pragma once
#include "I_ImageInterface.h"
#include "SkyInterface.h"
#include "memory.h"


class JPEGImageModule : public I_ImageInterface
{
public:
	JPEGImageModule();
	~JPEGImageModule();

	virtual bool Initialize() override;
	virtual SkyImageBuffer* GetPixelDataFromFile(char* szFileName)  override;
	virtual SkyImageBuffer* GetPixelDataFromBuffer(char* pBuffer, int size)  override;
	virtual bool SavePixelData(char* szFileName, char* pBuffer, int size)  override;

private:
	SkyImageBuffer m_imageBuffer;
	
};

