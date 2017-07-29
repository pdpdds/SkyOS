#include "fatfs.h"
#include "Console.h"
#include "string.h"
#include "fdddrv.h"
#include "kheap.h"

/*
 * GLOBAL FUNCTIONS 
 */
BOOL FsInitializeModule(VOID);

/*
 * DEFINITIONS
 */
#define BYTES_PER_SECTOR				512
#define NUMBER_OF_DIR_ENTRIES			0xe0
#define CLUSTER_OFFSET					(0x21-2)

typedef struct _BPB {
	BYTE			BS_jmpBoot[3];
	BYTE			BS_OEMName[8];
	WORD			BPB_BytesPerSec;
	BYTE			BPB_SecPerClus;
	WORD			BPB_RsvdSecCnt;
	BYTE			BPB_NumFATs;
	WORD			BPB_RootEntCnt;
	WORD			BPB_TotSec16;
	BYTE			BPB_Media;
	WORD			BPB_FATSz16;
	WORD			BPB_SecPerTrk;
	WORD			BPB_NumHeads;
	DWORD			BPB_HiddSec;
	DWORD			BPB_TotSec32;
	BYTE			BS_DrvNum;
	BYTE			BS_Reserved1;
	BYTE			BS_BootSig;
	DWORD			BS_VolID;
	BYTE			BS_VolLab[11];
	BYTE			BS_FilSysType[8];
} BPB, *PBPB;

typedef struct _DIRECTORY_ENTRY {
	BYTE			filename[8];
	BYTE			extension[3];
	BYTE			attribute;
	BYTE			reserved[10];
	WORD			time;
	WORD			date;
	WORD			cluster;
	DWORD			filesize;
} DIRECTORY_ENTRY, *PDIRECTORY_ENTRY;

typedef struct _FILE_CONTROL_BLOCK {
	BOOL			found;

	BYTE			*pt_filename;
	DWORD			attribute;
	DWORD			filesize;

	WORD			cur_cluster;
	DWORD			bytesread;
	BYTE			buffer[BYTES_PER_SECTOR];
} FILE_CONTROL_BLOCK, *PFILE_CONTROL_BLOCK;

/*
 * INTERNEL FUNCTIONS
 */
static BOOL FspLoadFAT(void);
static BOOL FspLoadDirEntry(void);
static WORD FspGetNextCluster(WORD CurCluster);

/*
 * GLOBAL VARIABLES
 */
#define FAT_SIZE		(BYTES_PER_SECTOR*9)
static BYTE m_FAT[FAT_SIZE];
static DIRECTORY_ENTRY m_DirEntry[NUMBER_OF_DIR_ENTRIES];


/**********************************************************************************************************
 *                                             GLOBAL FUNTIONS                                            *
 **********************************************************************************************************/
BOOL FsInitializeModule(VOID)
{
	if(!FspLoadFAT()) {
		SkyConsole::Print("FspLoadFAT() returned an error.\r\n");
		return FALSE;
	}
	if(!FspLoadDirEntry()) {
		SkyConsole::Print("FspLoadDirEntry() returned an error.\r\n");
		return FALSE;
	}

	return TRUE;
}


/**********************************************************************************************************
 *                                           EXTERNEL FUNTIONS                                            *
 **********************************************************************************************************/
VOID FsGetFileList(FS_FILE_INFO_CALLBACK CallBack, PVOID Context)
{
	FILE_INFO FileInfo;
	int i, j, k;

	for(i=0; i<NUMBER_OF_DIR_ENTRIES; i++) {
		if(m_DirEntry[i].filename[0] == 0xe5 || m_DirEntry[i].filename[0]==NULL)
			continue;

		/* filename */
		j=0;
		while(m_DirEntry[i].filename[j] != ' ' && j<8) {
			FileInfo.filename[j] = m_DirEntry[i].filename[j];
			j++;
		}
		FileInfo.filename[j++] = '.';

		/* extension */
		k=j;
		j=0;
		while(m_DirEntry[i].extension[j] != ' ' && j<3) {
			FileInfo.filename[j+k] = m_DirEntry[i].extension[j];
			j++;
		}
		FileInfo.filename[j+k] = '\0';

		/* other info */
		FileInfo.attribute = m_DirEntry[i].attribute;
		FileInfo.time = m_DirEntry[i].time;
		FileInfo.date = m_DirEntry[i].date;
		FileInfo.filesize = m_DirEntry[i].filesize;
		FileInfo.start_cluster = m_DirEntry[i].cluster;

		/* call */
		if(!CallBack(&FileInfo, Context))
			break;
	}
}

static BOOL Fsp_of_callback(FILE_INFO *pFileInfo, PVOID Context)
{
	PFILE_CONTROL_BLOCK pContext = (PFILE_CONTROL_BLOCK)Context;

	if(!strcmp((const char*)pFileInfo->filename, (const char*)pContext->pt_filename)) {
		pContext->cur_cluster = pFileInfo->start_cluster;
		pContext->filesize = pFileInfo->filesize;
		pContext->found = TRUE;
		return FALSE;
	}

	return TRUE;
}

HANDLE FsOpenFile(BYTE *pFilename, DWORD Attribute)
{
	PFILE_CONTROL_BLOCK pt_block;

	if(pFilename == NULL || strlen((const char*)pFilename) == 0)
		return NULL;
	strupr((char*)pFilename);

	pt_block = (PFILE_CONTROL_BLOCK)kmalloc(sizeof(FILE_CONTROL_BLOCK));
	if(pt_block == NULL) return NULL;

	pt_block->found			= FALSE;
	pt_block->pt_filename	= pFilename;
	pt_block->attribute		= Attribute;
	pt_block->bytesread		= 0;

	FsGetFileList(Fsp_of_callback, pt_block);
	if(!pt_block->found) {
		FsCloseFile((HANDLE)pt_block);
		return NULL;
	}	

	if(!FddReadSector(pt_block->cur_cluster+CLUSTER_OFFSET, 1, pt_block->buffer)) {
		FsCloseFile((HANDLE)pt_block);
		return NULL;
	}

	return (HANDLE)pt_block;
}

BOOL   FsCloseFile(HANDLE FileHandle)
{	
	kfree(FileHandle);

	return TRUE;
}

DWORD  FsReadFile(HANDLE FileHandle, BYTE *pData, DWORD NumberOfBytesToRead)
{
	PFILE_CONTROL_BLOCK pt_block = (PFILE_CONTROL_BLOCK)FileHandle;
	DWORD bytes_read, bytes_to_read, cur_read_bytes;
	DWORD pos_of_userdata, pos_of_buffer;

	/* check parameters */
	if(FileHandle == NULL || pData == NULL || NumberOfBytesToRead == 0) return 0;

	/* check filesize */
	if(pt_block->filesize <= pt_block->bytesread) {
		return 0;
	} else if(pt_block->bytesread+NumberOfBytesToRead < pt_block->filesize) {
		bytes_to_read = NumberOfBytesToRead;
	} else { /* pt_block->bytesread+NumberOfBytesToRead >= pt_block->filesize */
		bytes_to_read = pt_block->filesize - pt_block->bytesread;
	}

	/* read */
	pos_of_userdata = 0;
	bytes_read = 0;
	while(bytes_to_read != 0) {
		pos_of_buffer = pt_block->bytesread % BYTES_PER_SECTOR;
		cur_read_bytes = BYTES_PER_SECTOR-pos_of_buffer;
		if(cur_read_bytes > bytes_to_read)
			cur_read_bytes = bytes_to_read;

		memcpy((pData+pos_of_userdata), (pt_block->buffer+pos_of_buffer), cur_read_bytes);

		bytes_read += cur_read_bytes;
		bytes_to_read -= cur_read_bytes;
		pt_block->bytesread += cur_read_bytes;
		pos_of_userdata += cur_read_bytes;

		if(pt_block->bytesread%BYTES_PER_SECTOR)
			continue;

		pt_block->cur_cluster = FspGetNextCluster(pt_block->cur_cluster);
		if(pt_block->cur_cluster < 0x2 || pt_block->cur_cluster > 0xfef)
			break;
		if(!FddReadSector(pt_block->cur_cluster+CLUSTER_OFFSET, 1, pt_block->buffer))
			break;
	}

	return bytes_read;
}

BYTE   FsReadChar(HANDLE FileHandle)
{
	BYTE ch;
	FsReadFile(FileHandle, &ch, 1);
	return ch;
}


/**********************************************************************************************************
 *                                           INTERNEL FUNTIONS                                            *
 **********************************************************************************************************/
static BOOL FspLoadFAT(void)
{
	//CrtPrintText("Reading FAT Table... \r\n");
	return FddReadSector(0x01, 9, m_FAT);
}

static BOOL FspLoadDirEntry(void)
{
	//CrtPrintText("Reading Directory Entries... \r\n");
	return FddReadSector(0x13, 14, (BYTE *)m_DirEntry);
}

static WORD FspGetNextCluster(WORD CurCluster)
{
	WORD cluster, fat_entry;

	if(CurCluster%2) { /* odd */
		fat_entry = ((CurCluster>>1)*3)+1;
	} else { /* even */
		fat_entry = ((CurCluster>>1)*3);
	}

	cluster  = (WORD)(m_FAT[fat_entry]);
	cluster |= ((WORD)(m_FAT[fat_entry+1]) << 8);

	if(CurCluster % 2)
		cluster >>= 4;
	else
		cluster &= 0x0fff;

	return cluster;
}