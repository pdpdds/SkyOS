#pragma once

#include "windef.h"
#include "Partition.h"
#include "GFS.H"

#define FAT_DPF_SIZE 40

//---------------------------STRUCTURES DEFINED HERE---------------------------

#ifdef _MSC_VER
#pragma pack (push, 1)
#endif
/* Starting of a Boot Sector Size=11*/
struct BootSectorStart
{
    BYTE bJumpInstruction[3];    /* 1-0xEB or 0xE9 2-anything 3-anything */
    BYTE bOEMName[8];             /* OEMName IBM3.0, MSWin4.1 or MSDos5.0 */
};
/* BIOS Parameter Block definition 
Size of this structure is 25 */
struct BPB
{
    UINT16  wBytesPerSector;    /* Bytes per sector may be 512,1024,2048 or 4096*/
    BYTE    bSectorsPerCluster; /* Must be greater than 0 and power of 2 that is 1,2,4,8,16,32,64 and 128*/
    UINT16  wReservedSectors;   /* Must not 0. For FAT12 and FAT16 it may be 1 and for FAT32 it may be 32 */
    BYTE    bNumberFATs;        /* 1 or 2. */
    UINT16  wRootDirEntries;    /* Number of root dir entries for FAT32 it is 0*/
    UINT16  wTotalSectors16;    /* Total Number of Sectors */
    BYTE    bMediaDescriptor;   /* F0 - For removable media. F8,F9,FA,FB,FC,FD,FF are valid*/
    UINT16  wFATSize16;         /* Sectors per FAT(Size of FAT). 0 for FAT32 */
    UINT16  wSectorsPerTrack;   /* Sectors per track used by INT 13*/
    UINT16  wNumberOfHeads;     /* Number of heads used by INT 13*/
    UINT32  dwHiddenSectors;    /* For partitioned media it is setted */
    UINT32  dwTotalSectors32;   /* Number of sectors if TotalSectors16 == 0 */
};
/*  FAT 12 or FAT 16 file system 
Size of this struture is 26 */
struct FAT12_16
{
    BYTE    bDriveNumber;       /* 80h = harddrive else for floppy 00h */
    BYTE    bReserved;          /* set to 0 */
    BYTE    bBootSignature;     /* 29h */
    UINT32  dwVolumeID;         /* Volume serial number */
    BYTE    bVolumeLabel[11];   /* Volume label */
    BYTE    bFileSystem[8];     /* File system type - FAT12 , FAT16  */
};

/* FAT 32 file system 
Size of this struture is 52 */
struct FAT32
{
    UINT32 dwFATSize32;         /* Sectors per FAT(Size of FAT) */
    UINT16 wExtFlag;            /* bits 0-3 - contains the number of active FAT only if Mirroring is disabled. 4-6 - Reserved. 7-if 0 Mirrored to all FATs else Mirror disabled. 8-15 - reserved*/
    UINT16 wFileSystemVersion;  /* Low byte minor version. High byte major version my version is 0:0 check */
    UINT32 dwRootCluster;       /* Root cluster starting number */
    UINT16 wFileSystemInfo;     /* 1 */
    UINT16 wBackUpBootSector;   /* 6 Indicates the Sector where back up copy of the Boot Record is stored. */
    BYTE   bReserved[12];       /* Set to 0 */
    BYTE   bDriveNumber;	/* Same as FAT12_16 */
    BYTE   bReserved1;
    BYTE   bBootSignature;      /* 29h */
    UINT32 dwVolumeID;          /* Volume serial number */
    BYTE   bVolumeLabel[11];    /* Volume label */
    BYTE   bFileSystem[8];      /* File system FAT32 */
};

/* Total of this structure is 32 byte */
struct DirEntry
{
    BYTE   bFileName[8];        /* File name if Name[0]=0xE5 then no file in this entry if Name[0]=0 then no files in this entry and there is no more after this one*/
    BYTE   bExtension[3];       /* Extension */
    BYTE   bAttribute;          /* Bit coded */
    BYTE   bReserved;           /* For FAT12_16=0 Reseved for NT */
    BYTE   bCreationTimeTenth;	/* For FAT12_16=0 Millisecond Time for Tenth 0-199 */
    UINT16 wCreationTime;       /* For FAT12_16=0 Encoded   */
    UINT16 wCreationDate;       /* For FAT12_16=0 Encoded   */
    UINT16 wLastAccessDate;     /* For FAT12_16=0 */
    UINT16 wFirstClusterHigh;   /* For FAT12_16=0 high word of first cluster number */
    UINT16 wAccessTime;		/* Last Accessed Time - This field is used by Dos*/
    UINT16 wAccessDate;         /* Last Accessed Time - This field is used by Dos*/
    UINT16 wFirstClusterLow;    /* Low word of first cluster number */
    UINT32 dwFileSize;          /* Size of file must be zero for Directory */
};

typedef DirEntry DIRENTRY;
typedef DIRENTRY * LPDIRENTRY;

#ifdef _MSC_VER
#pragma pack (pop)
#endif

struct __FATInfo
{
    BYTE bFATType;
    struct BootSectorStart _BS;
    struct BPB _BPB;
    
    struct FAT12_16 _FAT12_16;
    struct FAT32 _FAT32;
    
    UINT32 dwFATSize;
    UINT32 dwTotRootDirSectors;
    UINT32 dwDataSectors;
    UINT32 dwCountOfClusters;
};

struct _FAT
{
    BYTE szDevicePath[FAT_DPF_SIZE];
    struct _Partition Part;
    struct __FATInfo FDI;
};
typedef struct _FAT FAT;
typedef FAT * LPFAT;

struct FATOpenedFileInfo
{
    BYTE lpSectorBuffer[1024];	        //this is the internal FAT cache for the particular file
	
    LPFAT lpFATInfo;	

    UINT32 dwBufferedFATSector;         //this is related to the above field SectorBuffer - this indicates the buffered FAT's sector number
    BYTE * lpBuffer;		        //data cache buffer
    UINT16 wBufferOffset;

    BYTE bBufferIsValid;		//intially FALSE after a read TRUE.
    BYTE bMode;				//File open mode not implemented
    BYTE bShareMode;
    BYTE bDriveLetter;			//drive letter
    DIRENTRY DEInfo;		        //DirEntry of the opened file
    UINT32 dwTotalBytesPassed;	        //Total bytes read or writen from the begining
    UINT32 dwCurrentCluster;		//Current cluster number in the linked list of clusters
};
typedef struct FATOpenedFileInfo FATOPENEDFILEINFO;
typedef FATOPENEDFILEINFO * LPFATOPENEDFILEINFO;


struct FATFindFileInfo
{
    char szReqPath[_MAX_PATH];          //Requested Path 
    char szFilePattern[_MAX_FNAME];     //Requested File pattern
    
    LPFAT lpFATInfo;            
    DIRENTRY DEInfo;                    //Directory Entry of the current file
    
    UINT32 dwCurrentCluster;
    UINT32 dwCurrentLBASector;
    
    BYTE * lpBuffer;
    UINT32 dwBufferedSecctor;           //Buffered Sector or Cluster
    UINT32 dwBufferIndex;               //Current Directory Entry in the buffer
    UINT32 dwBufferTotal;               //Total Directory Entries in the buffer 
    
    BYTE bRoot;                         //Searching in Root Directory ( Set to true is searching in root directory, for FAT32 it is always set to false)
    
};

typedef struct FATFindFileInfo FATFINDFILEINFO;
typedef FATFINDFILEINFO * LPFATFINDFILEINFO;
    
extern LPLISTNODE sysFATOpenFileInfo;

int FAT_Init();

//GFS function implementation
UINT32 FAT_GetVolumeCapacity(LPFSHANDLE lpFS);
UINT32 FAT_GetVolumeFree(LPFSHANDLE lpFS);
UINT32 FAT_GetVolumeBad(LPFSHANDLE lpFS);

UINT32 FAT_CreateDirectory(LPFSHANDLE lpFS, LPCSTR lpPathName);
UINT32 FAT_RemoveDirectory(LPFSHANDLE lpFS, LPCSTR lpPathName);

UINT32 FAT_GetCurrentDirectory(LPFSHANDLE lpFS, DWORD nBufferLength,  LPTSTR lpBuffer);
UINT32 FAT_SetCurrentDirectory(LPFSHANDLE lpFS, LPTSTR lpDirPath);

HANDLE FAT_CreateFile(LPFSHANDLE lpFS, LPCTSTR lpFilePath, DWORD dwOpenMode, DWORD dwShareMode, DWORD dwCreationDisposition, DWORD dwFlags );
UINT32 FAT_CloseFile(LPFSHANDLE lpFS, HANDLE hFile);
UINT32 FAT_SetFilePointer(LPFSHANDLE lpFS, HANDLE hFile, DWORD dwNewPos, DWORD dwMoveMethod);
UINT32 FAT_GetFilePointer(LPFSHANDLE lpFS, HANDLE hFile);
UINT32 FAT_ReadFile(LPFSHANDLE lpFS, HANDLE hFile, UINT32 dwNoOfBytes, void * lpBuffer);
UINT32 FAT_WriteFile(LPFSHANDLE lpFS, HANDLE hFile, UINT32 dwNoOfBytes, void * lpBuffer);

UINT32 FAT_DeleteFile(LPFSHANDLE lpFS, LPCTSTR lpFileName);
UINT32 FAT_GetFileAttributes(LPFSHANDLE lpFS, LPCTSTR lpFileName, LPFILEINFO lpFINFO);

HANDLE FAT_FindFirstFile(LPFSHANDLE lpFS, LPCTSTR lpFileName, LPFILEINFO lpFINFO);
UINT32 FAT_FindNextFile(LPFSHANDLE lpFS, HANDLE hFile, LPFILEINFO lpFINFO );
UINT32 FAT_FindClose(LPFSHANDLE lpFS, HANDLE hFile);

//internal functions
BYTE FindFileInEnumeration(LPDIRENTRY DEInfo, void * vFileName, LPDIRENTRY Result);
LPDIRENTRY GetDirectoryEntry(LPFAT lpFAT, char * szFilePath,LPDIRENTRY lpDEInfo);
BYTE EnumerateFilesInFolder(LPFAT lpFATInfo, LPDIRENTRY FolderDE, BYTE (*CallBackFn)(LPDIRENTRY, void * ,LPDIRENTRY), void * CallBackPara , LPDIRENTRY lpResult);
BYTE EnumerateFilesInRoot(LPFAT lpFATInfo, BYTE (*CallBackFn)(LPDIRENTRY ,void *,LPDIRENTRY), void * CallBackPara, LPDIRENTRY lpResult );
BYTE FATReadFileCluster(LPFATOPENEDFILEINFO lpFileInfo, LPFAT lpFATInfo);
UINT32 DeleteFileEntry(LPFSHANDLE lpFS, LPCTSTR lpFileName);
UINT32 MoveFilePointerToNextCluster(LPFATOPENEDFILEINFO lpFileInfo, BOOLEAN bFillBuffer );
char * GetFileNameFromDEInfo(const LPDIRENTRY lpDEInfo, char * szDEFileName);
char * ConvertFileNameToProperFormat(char * szFile, char * szFileName, char * szExt);
UINT32 ReadGSHDevice(const char * szDevicePath, DWORD dwLBASector, DWORD dwSectorCount, BYTE * lpBuffer);
UINT32 WriteGSHDevice(const char * szDevicePath, DWORD dwLBASector, DWORD dwSectorCount, BYTE * lpBuffer);
BYTE ValidateDEInfo(LPDIRENTRY DEInfo);
int FAT_Init();
void CheckAndInstallFAT(LPCTSTR szDevicePath, LPPARTITION lpPart);
