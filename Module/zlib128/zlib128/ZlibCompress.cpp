#include "ZlibCompress.h"
#include "SkyInterface.h"
#include "memory.h"
#include "zlib.h"

extern void printf(const char* str, ...);

ZlibCompress::ZlibCompress()
{
}


ZlibCompress::~ZlibCompress()
{
}

int ZlibCompress::Compress(unsigned char* pDest, long* pnDestLen, unsigned char* pSrc, long nSrcLen)
{
	if (pDest == nullptr)
		return 0;

	return compress(pDest, (uLongf*)pnDestLen, pSrc, nSrcLen);		
}

int ZlibCompress::Decompress(unsigned char* pDest, long* pnDestLen, unsigned char* pSrc, long nSrcLen)
{
	if (pDest == nullptr)
		return 0;
	
	return uncompress(pDest, (uLongf*)pnDestLen, pSrc, nSrcLen);
}

