#pragma once
#include "windef.h"
#include "multiboot.h"

#ifdef __cplusplus
extern "C" {
#endif

	void init_lfb();
	void lfb_clear();
	ULONG getDepth();
	static ULONG *lfb = 0;
	void lfb_update(unsigned long *buf);
	unsigned long *getlfb();
	extern void putp(ULONG i, ULONG col);

#ifdef __cplusplus
}

#endif