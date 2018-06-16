#include "EasyCompress.h"
#include "easyzlib.h"


EasyCompress::EasyCompress()
{
}


EasyCompress::~EasyCompress()
{
}

int EasyCompress::Compress(unsigned char* pDest, long* pnDestLen, unsigned char* pSrc, long nSrcLen)
{
	return ezcompress(pDest, pnDestLen, pSrc, nSrcLen);
}

int EasyCompress::Decompress(unsigned char* pDest, long* pnDestLen, unsigned char* pSrc, long nSrcLen)
{
	return ezuncompress(pDest, pnDestLen, pSrc, nSrcLen);
}
