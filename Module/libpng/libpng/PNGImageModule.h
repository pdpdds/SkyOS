#pragma once
#include "I_ImageInterface.h"
#include "png.h"

class PNGImageModule : public I_ImageInterface
{
public:
	PNGImageModule();
	~PNGImageModule();

	virtual bool Initialize() override;
	virtual SkyImageBuffer* GetPixelDataFromFile(char* szFileName)  override;
	virtual SkyImageBuffer* GetPixelDataFromBuffer(char* pBuffer, int size)  override;
	virtual bool SavePixelData(char* szFileName, char* pBuffer, int size)  override;

protected:
	void ProcessFile(void);

private:
	int x, y;

	int width, height;
	png_byte color_type;
	png_byte bit_depth;

	png_structp png_ptr;
	png_infop info_ptr;
	int number_of_passes;
	png_bytep * row_pointers;

	SkyImageBuffer m_imageBuffer;
};

