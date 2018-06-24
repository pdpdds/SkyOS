#include "BMPImageModule.h"

extern void printf(const char* str, ...);

BMPImageModule::BMPImageModule()
{
}


BMPImageModule::~BMPImageModule()
{
	bmp_img_free(&m_img);
}

bool BMPImageModule::Initialize()
{
	return true;
}

SkyImageBuffer* BMPImageModule::GetPixelDataFromFile(char* szFileName)
{
	//Test();

	bmp_error error = bmp_img_read(&m_img, "sample1.bmp");

	if (BMP_OK != error)
		return nullptr;

	m_imageBuffer._bufferType = IAMGE_BUFFER_2D_ARRAY;
	m_imageBuffer._pBuffer = (char**)m_img.img_pixels;
	m_imageBuffer._height = m_img.img_header.biHeight;
	m_imageBuffer._width = m_img.img_header.biWidth;
	m_imageBuffer._bpp = m_img.img_header.biBitCount;
	m_imageBuffer._colorType = 0;

	return &m_imageBuffer;
}

SkyImageBuffer* BMPImageModule::GetPixelDataFromBuffer(char* pBuffer, int size)
{
	//not implemented
	return nullptr;
}

bool BMPImageModule::SavePixelData(char* szFileName, char* pBuffer, int size)
{
	bmp_img_write(&m_img, szFileName);

	return true;
}

void BMPImageModule::Test()
{
	bmp_img img;
	bmp_img_init_df(&img, 512, 512);

	// Draw a checkerboard pattern:
	for (size_t y = 0, x; y < 512; y++)
	{
		for (x = 0; x < 512; x++)
		{
			if ((y % 128 < 64 && x % 128 < 64) ||
				(y % 128 >= 64 && x % 128 >= 64))
			{
				bmp_pixel_init(&img.img_pixels[y][x], 250, 250, 250);
			}
			else
			{
				bmp_pixel_init(&img.img_pixels[y][x], 0, 0, 0);
			}
		}
	}

	bmp_img_write(&img, "test.bmp");
	bmp_img_free(&img);
	
}