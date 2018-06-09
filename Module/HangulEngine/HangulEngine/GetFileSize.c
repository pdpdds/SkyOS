//---------------------------------------------------------------------------
#include <stdio.h>
#include "windef.h"
#include "fileio.h"
//---------------------------------------------------------------------------
int GetFileSize(FILE *fp)
{
	int curpos, length;

	curpos = ftell(fp);
	fseek(fp, 0L, SEEK_END);
	length = ftell(fp);
	fseek(fp, curpos, SEEK_SET);

	return length;
}
//---------------------------------------------------------------------------
