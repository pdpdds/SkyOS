#ifndef _FATFS_HEADER_FILE_
#define _FATFS_HEADER_FILE_


/**********************************************************************************************************
 *                                                INCLUDE FILES                                           *
 **********************************************************************************************************/
#include "windef.h"
#include "hal.h"


/**********************************************************************************************************
 *                                                  DEFINITIONS                                           *
 **********************************************************************************************************/
#define FILE_ATTR_READ_ONLY				0x01
#define FILE_ATTR_HIDDEN				0x02
#define FILE_ATTR_SYSTEM				0x04
#define FILE_ATTR_VOLUME_ID				0x08
#define FILE_ATTR_DIRECTORY				0x10
#define FILE_ATTR_ARCHIVE				0x20
#define FILE_ATTR_LONG_NAME				(FILE_ATTR_READ_ONLY | FILE_ATTR_HIDDEN | FILE_ATTR_SYSTEM | FILE_ATTR_VOLUME_ID)

typedef struct _FILE_INFO {
	BYTE			filename[8+3+1+1]; /* filename+ext+dot+null, max 12 */
	BYTE			attribute;
	WORD			time;
	WORD			date;
	DWORD			filesize;
	WORD			start_cluster;
} FILE_INFO, *PFILE_INFO;

typedef BOOL (*FS_FILE_INFO_CALLBACK)(FILE_INFO *pFileInfo, PVOID Context);

#define OF_READ_ONLY					0x01


/**********************************************************************************************************
 *                                             EXPORTED FUNCTIONS                                         *
 **********************************************************************************************************/
VOID FsGetFileList(FS_FILE_INFO_CALLBACK CallBack, PVOID Context);

HANDLE FsOpenFile(BYTE *pFilename, DWORD Attribute);
BOOL   FsCloseFile(HANDLE FileHandle);
DWORD  FsReadFile(HANDLE FileHandle, BYTE *pData, DWORD NumberOfBytesToRead);
BYTE   FsReadChar(HANDLE FileHandle);


#endif /* #ifndef _FATFS_HEADER_FILE_ */