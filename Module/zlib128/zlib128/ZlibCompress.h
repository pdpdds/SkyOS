#pragma once
#include "I_Compress.h"

class ZlibCompress : public I_Compress
{
public:
	ZlibCompress();
	~ZlibCompress();

	virtual int Compress(unsigned char* pDest, long* pnDestLen, unsigned char* pSrc, long nSrcLen) override;
	virtual int Decompress(unsigned char* pDest, long* pnDestLen, unsigned char* pSrc, long nSrcLen) override;
};

