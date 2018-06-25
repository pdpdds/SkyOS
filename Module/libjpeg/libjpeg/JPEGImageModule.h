#pragma once
#include "I_ImageInterface.h"
#include "SkyInterface.h"
#include "memory.h"
#include "libbmp.h"

class BMPImageModule : public I_ImageInterface
{
public:
	BMPImageModule();
	~BMPImageModule();

	virtual bool Initialize() override;
	virtual SkyImageBuffer* GetPixelDataFromFile(char* szFileName)  override;
	virtual SkyImageBuffer* GetPixelDataFromBuffer(char* pBuffer, int size)  override;
	virtual bool SavePixelData(char* szFileName, char* pBuffer, int size)  override;

	void Test();

private:
	SkyImageBuffer m_imageBuffer;
	bmp_img m_img;
};

