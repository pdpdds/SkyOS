#include "JPEGImageModule.h"
#include "jpgreader.h"
extern void printf(const char* str, ...);

//#include "IOStream.h"

//#include <MimeParser.h>

//#include "JpegDec.h"

//using namespace jpegutil;
//using namespace mimeutil;

JPEGImageModule::JPEGImageModule()
{
}


JPEGImageModule::~JPEGImageModule()
{
	
}

bool JPEGImageModule::Initialize()
{
	return true;
}

SkyImageBuffer* JPEGImageModule::GetPixelDataFromFile(char* szFileName)
{
	jpgReader* reader = new jpgReader();
	laz_img img = reader->readJPG(szFileName);
	

	m_imageBuffer._pBuffer = (char**)img.getData();
	m_imageBuffer._height = img.getHeight();
	m_imageBuffer._width = img.getWidth();
	m_imageBuffer._bufferType = IAMGE_BUFFER_LINEAR;

	/*FILE* pFile = fopen(szFileName, "rb");
	//assert(testFile != nullptr);

	MimeParser* parser = new MimeParser();
	JpegDec* decoder = new JpegDec();

	size_t frameNum = 0;
	while (true) {
		uint8_t* img = nullptr;
		int width, height;

		//auto start = std::chrono::steady_clock::now();

		if (!parser->readNext(pFile)) {
			break;
		}
		frameNum++;

		if (parser->getContentType() == "image/jpeg") {
			decoder->decode(parser->getContent(), parser->getContentLength(), img, width, height);
		}

		//auto end = std::chrono::steady_clock::now();

//		std::cout << "frame #" << frameNum;
	//	std::cout << " size: " << parser->getContentLength();
		//std::cout << " time: " << std::chrono::duration<double, std::milli>(end - start).count() << " ms";
	//	std::cout << std::endl;

		delete[] img;
	}

	delete decoder;
	delete parser;*/
	
	return &m_imageBuffer;
}

SkyImageBuffer* JPEGImageModule::GetPixelDataFromBuffer(char* pBuffer, int size)
{
	//not implemented
	return nullptr;
}

bool JPEGImageModule::SavePixelData(char* szFileName, char* pBuffer, int size)
{
	//reader.writeJPG(img, "test_copy.jpg");
	return true;
}