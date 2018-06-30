#pragma once
#include "windef.h"

#ifdef __cplusplus
extern "C" {
#endif

	void init_lfb(ULONG* lfb_ptr, ULONG width, ULONG height, ULONG depth, ULONG type);
	void lfb_clear();
	ULONG getDepth();
	static ULONG *lfb = 0;
	void lfb_update(unsigned long *buf);
	unsigned long *getlfb();
	void putp(ULONG i, ULONG col);
	unsigned long *getOffscreenBuffer();
	void pp(ULONG i, unsigned char r, unsigned char g, unsigned char b) ;

	ULONG getp(ULONG i);

#ifdef __cplusplus
}

#endif