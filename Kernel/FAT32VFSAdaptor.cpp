#include "FAT32VFSAdaptor.h"
#include "FAT32.h"
#include "string.h"

FILESYSTEM g_fat32FileSys;

FILE fsysFat32Directory(const char* DirectoryName) {

	FILE file;
	file.flags = FS_INVALID;
	return file;
}

/**
*	Reads from a file
*/
void fsysFat32Read(PFILE file, unsigned char* Buffer, unsigned int Length) {

	int readLen = 0;
	if (file) {
		readLen = FATReadFile(file->id, Length, Buffer);

		if (readLen != Length)
			file->eof = 1;
	}
}

/**
*	Closes file
*/
void fsysFat32Close(PFILE file) {

	if (file->flags == FS_FILE && file->id != 0)
		FATIsEndOfFile(file->id);
}


/**
*	Opens a file
*/
FILE fsysFat32Open(const char* FileName) {

	FILE ret;
	UINT16 handle = FATFileOpen((char*)FileName, 0);

	if (handle == 0)
		ret.flags = FS_INVALID;
	else
	{
		ret.flags = FS_FILE;
		ret.id = handle;
	}

	return ret;
}


void fsysFat32Mount() {


}


bool InitializeVFSFat32()
{
	strcpy(g_fat32FileSys.Name, "FAT32");
	g_fat32FileSys.Directory = fsysFat32Directory;
	g_fat32FileSys.Mount = fsysFat32Mount;
	g_fat32FileSys.Open = fsysFat32Open;
	g_fat32FileSys.Read = fsysFat32Read;
	g_fat32FileSys.Close = fsysFat32Close;

	volRegisterFileSystem(&g_fat32FileSys, 'c' - 'a');

	return true;
}