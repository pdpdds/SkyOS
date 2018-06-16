#pragma once
#include "I_Compress.h"

class EasyCompress : public I_Compress
{
public:
	EasyCompress();
	~EasyCompress();

	virtual int Compress(unsigned char* pDest, long* pnDestLen, unsigned char* pSrc, long nSrcLen) override;
	virtual int Decompress(unsigned char* pDest, long* pnDestLen, unsigned char* pSrc, long nSrcLen) override;
};

