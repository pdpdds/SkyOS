#pragma once

#define IAMGE_BUFFER_LINEAR 0
#define IAMGE_BUFFER_2D_ARRAY 1

typedef struct tag_SkyImageBuffer
{
	int _width;
	int _height;
	int _bpp;
	int _colorType;
	int _bufferType;
	char** _pBuffer;

}SkyImageBuffer;

class I_ImageInterface
{
public:
	virtual bool Initialize() { return false; }
	virtual SkyImageBuffer* GetPixelDataFromFile(char* szFileName) = 0;
	virtual SkyImageBuffer* GetPixelDataFromBuffer(char* pBuffer, int size) = 0;
	virtual bool SavePixelData(char* szFileName, char* pBuffer, int size) = 0;
};