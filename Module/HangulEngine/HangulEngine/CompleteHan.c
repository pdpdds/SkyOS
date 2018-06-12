//------------------------------------------------------------------------------
#ifdef __BORLANDC__
#pragma warn -8004
#endif

#include "hanlib.h"
#include "CompleteHan.h"
//-----------------------------------------------------------------------------
void HanComplete(bool mode, unsigned char* dest, unsigned char* src, int nums)
{
	int i;
	if (mode == true) memcpy(dest, src, nums);
	else for (i = 0; i < nums; i++) 
		dest[i] |= src[i];
}
//------------------------------------------------------------------------------
