#pragma once

class I_Compress
{
public:
	virtual int Compress(unsigned char* pDest, long* pnDestLen, unsigned char* pSrc, long nSrcLen) = 0;
	virtual int Decompress(unsigned char* pDest, long* pnDestLen, unsigned char* pSrc, long nSrcLen) = 0;
};