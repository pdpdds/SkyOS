/* This file was contain all functions and structures needed to handle
a FAT file system (FAT-12,16 and 32 are Supported).
					-Samuel ( 28-Jan-2001)
	Modified 13-Nov-2001 (static Disk and Partion classes)
	Modified 09-Sep-2002 for Ace OS

	Author : Sam ( Samuelhard@yahoo.com)
*/

#ifndef FAT__H
#define FAT__H

#include "windef.h"
#include <Collect.h> 
#include <Partition.h> 


#define MAX_DIR 500
#define MAX_FILENAME 9
#define MAX_EXTENSION 4

//----------------FAT File Attribute Constants------------------------
int const ATTR_READONLY = 0x01;
int const ATTR_HIDDEN = 0x02;
int const ATTR_SYSTEM = 0x04;
int const ATTR_VOLUMEID = 0x08;
int const ATTR_DIRECTORY = 0x10;
int const ATTR_ARCHIVE = 0x20;



//---------------------------STRUCTURES DEFINED HERE---------------------------

/* Starting of a Boot Sector Size=11*/
#pragma pack(push,1)
struct BootSectorStart
{
	BYTE JumpInstruction[3];    /* 1-0xEB or 0xE9 2-anything 3-anything */
	BYTE OEMName[8];             /* OEMName IBM3.0, MSWin4.1 or MSDos5.0 */
};
/* BIOS Parameter Block definition */
/* Size of this structure is 25 */
struct BPB
{
	UINT16  BytesPerSector;      /* Bytes per sector may be 512,1024,2048 or 4096*/
	BYTE    SectorsPerCluster;   /* Must be greater than 0 and power of 2 that is 1,2,4,8,16,32,64 and 128*/
	UINT16  ReservedSectors;     /* Must not 0. For FAT12 and FAT16 it may be 1 and for FAT32 it may be 32 */
	BYTE    NumberFATs;          /* 1 or 2. */
	UINT16  RootDirEntries;      /* Number of root dir entries for FAT32 it is 0*/
	UINT16  TotalSectors16;      /* Total Number of Sectors */
	BYTE    MediaDescriptor;     /* F0 - For removable media. F8,F9,FA,FB,FC,FD,FF are valid*/
	UINT16  FATSize16;           /* Sectors per FAT(Size of FAT). 0 for FAT32 */
	UINT16  SectorsPerTrack;     /* Sectors per track used by INT 13*/
	UINT16  NumberOfHeads;       /* Number of heads used by INT 13*/
	UINT32  HiddenSectors;       /* For partitioned media it is setted */
	UINT32  TotalSectors32;	     /* Number of sectors if TotalSectors16 == 0 */
};
/*  FAT 12 or FAT 16 file system */
/*  Size of this struture is 26 */
struct FAT12_16
{
	BYTE    DriveNumber;       /* 80h = harddrive else for floppy 00h */
	BYTE    Reserved;          /* set to 0 */
	BYTE    BootSignature;     /* 29h */
	UINT32  VolumeID;          /* Volume serial number */
	BYTE    VolumeLabel[11];   /* Volume label */
	BYTE    FileSystem[8];     /* File system type - FAT12 , FAT16  */
};

/* FAT 32 file system */
/*  Size of this struture is 52 */
struct FAT32
{
	UINT32 FATSize32;            /* Sectors per FAT(Size of FAT) */
	UINT16 ExtFlag;              /* bits 0-3 - contains the number of active FAT only if Mirroring is disabled. 4-6 - Reserved. 7-if 0 Mirrored to all FATs else Mirror disabled. 8-15 - reserved*/
	UINT16 FileSystemVersion;    /* Low byte minor version. High byte major version my version is 0:0 check */
	UINT32 RootCluster;	     /* Root cluster starting number */
	UINT16 FileSystemInfo;	     /* 1 */
	UINT16 BackUpBootSector;     /* 6 Indicates the Sector where back up copy of the Boot Record is stored. */
	BYTE   Reserved[12];         /* Set to 0 */
	BYTE   DriveNumber;	     /* Same as FAT12_16 */
	BYTE   Reserved1;
	BYTE   BootSignature;        /* 29h */
	UINT32 VolumeID;             /* Volume serial number */
	BYTE   VolumeLabel[11];      /* Volume label */
	BYTE   FileSystem[8];        /* File system FAT32 */
};

/* Total of this structure is 32 byte */
struct DirEntry
{
	BYTE  FileName[8];                /* File name if Name[0]=0xE5 then no file in this entry if Name[0]=0 then no files in this entry and there is no more after this one*/
	BYTE   Extension[3];               /* Extension */
	BYTE   Attribute;                  /* Bit coded */
	BYTE   Reserved;                   /* For FAT12_16=0 Reseved for NT */
	BYTE   CreationTimeTenth;	   /* For FAT12_16=0 Millisecond Time for Tenth 0-199 */
	UINT16 CreationTime;               /* For FAT12_16=0 Encoded   */
	UINT16 CreationDate;               /* For FAT12_16=0 Encoded   */
	UINT16 LastAccessDate;             /* For FAT12_16=0 */
	UINT16 FirstClusterHigh;           /* For FAT12_16=0 high word of first cluster number */
	UINT16 AccessTime;		   /* Last Accessed Time - This field is used by Dos*/
	UINT16 AccessDate;                 /* Last Accessed Time - This field is used by Dos*/
	UINT16 FirstClusterLow;            /* Low word of first cluster number */
	UINT32 FileSize;                   /* Size of file must be zero for Directory */
};
struct __FATInfo
{
	BYTE _FATType;        //This is used for union
	BootSectorStart _BS;
	BPB _BPB;

	FAT12_16 _FAT12_16;
	FAT32 _FAT32;

	UINT32 FATSize;
	UINT32 TotRootDirSectors;
	UINT32 DataSectors;
	UINT32 CountOfClusters;
};

class FATInfo
{

private:
	BYTE m_DPF[7];
	Partition Part;
	struct __FATInfo FDI;
public:
	FATInfo(BYTE *DPF, struct _Partition * DiskPart, BYTE * FirstSector);

	void Initialize(BYTE *DPF, struct _Partition * DiskPart, BYTE * FirstSector);

	BYTE * GetDPF();
	Partition * GetPartition();
	BYTE FATType();                  //12,16 and 32
	UINT32 GetFATSize();
	BYTE GetClusterSize();           //Returns Sectors per Cluster
	UINT16 GetSectorSize();          //Returns Bytes per Sectors
	UINT32 GetTotalSectors();        //Total Sectors in the File System
	UINT32 GetTotalRootDirSectors(); //Total Sectors occupied by the Root Directory only for FAT 12 and FAT 16 for FAT 32 it returns 0
	UINT32 GetTotalDataSectors();    //Returns Data occupied by the Data(Files and Sub directories)
	UINT32 GetTotalClusters();	 //Returns Total Clusters
	UINT32 GetReservedSectors();     //Returns Reserved Sectors
	// This routine will give the Logical Starting Sector number of
	// the N the FAT. Normally 2 FAT's are stored. To get the
	// starting sector number for the first FAT use 0 and to get
	// the 2 FAT address use 1.
	UINT16 GetFATStartSector(BYTE FATNo);
	UINT16 RootDirStartSector();
	UINT16 RootDirStartCluster();
	UINT32 DataAreaStartSector();

	/* returns the  Sector Number of the Cluster in FAT area*/
	UINT32 FATSectorNumber(UINT32 Cluster);
	/* returns the byte offset in Sector  FAT area*/
	UINT32 FATSectorOffset(UINT32 Cluster);

	/* this function will return the FirstLBASector number of the given cluster in the
	data area you may then use that number to read the file contents*/
	UINT32 FirstSectorOfCluster(UINT32 Cluster);


	/* FAT Entry Retrival and Verification */
	UINT32 GetFATEntry(UINT32 Cluster, BYTE * Sector);
	BYTE IsEndOfClusterChain(UINT32 FAT_Content);
	BYTE IsBadCluster(UINT32 FAT_Content);

	UINT16 RootDirEntries();  /* Maximum No of Root Dir Entries 512 */

	/* Logical Cluster <---> Absoulte Sector Conversion */
	UINT32 GetLogicalSector(UINT32 Cluster, UINT16 Sector);
	UINT32 GetClusterCluster(UINT32 LogicalSector);
	UINT32 GetClusterSector(UINT32 LogicalSector);
	/* (CHS Translation) LogicalSector  <----> Head,Track,Sector Conversion */
	UINT32 GetPhysicalSector(UINT32 LogicalSector);
	UINT32 GetPhysicalTrack(UINT32 LogicalSector);
	UINT32 GetPhysicalHead(UINT32 LogicalSector);
	UINT32 GetRelativeSector(UINT32 Head, UINT32 Track, UINT32 Sector);
};
#pragma pack(pop)

extern "C" Collection <class FATInfo *>*  __SysFATInternal;
//----------------------------------System Init Functions-----------------------------------
void InitFATFileSystem();
//------------------------------------Other functions-----------------------------------
/*This function returns the FATInformation for a particular drive*/
FATInfo * GetFATInfo(BYTE DriveLetter);

KERNELDLL DirEntry  * GetDirectoryEntry(const char * FilePath, struct DirEntry  * DEInfo);
/*Following two functions enumerate  valid Directory Entries in the root directory or in
a specified directory. They will stop when the end of folder or root is reached or when
the CallBackFn canceled the enumeration by returning 0.

Thsee functions will return 0 upon  unsuccessfull completion(memory error,FAT error,HDD error, etc)
return 1 on list completion(end of root / folder is reached.)
returns 2 when the CallBackFun caceled further listing
*/
KERNELDLL BYTE EnumerateFilesInRoot(BYTE DriveLetter, BYTE(*CallBackFn)(struct DirEntry  *, void *, struct DirEntry  *), void * CallBackPara, struct DirEntry  * Result);
KERNELDLL BYTE EnumerateFilesInFolder(BYTE DriveLetter, DirEntry * FolderDE, BYTE(*CallBackFn)(struct DirEntry  *, void *, struct DirEntry  *), void * CallBackPara, struct DirEntry  * Result);

#define OPEN_READ    0x1
#define OPEN_WRITE   0x2
#define OPEN_APPEND  0x4
#define OPEN_BINARY  0x8
#define OPEN_TEXT    0x10

KERNELDLL UINT16 FATFileOpen(BYTE driveLetter, const char * FilePath, BYTE Mode);
KERNELDLL bool FATFileClose(UINT16 HandleID);
KERNELDLL UINT16 FATReadFile(UINT16 HandleID, UINT32 SizeInBytes, BYTE * Buffer);
KERNELDLL BYTE FATIsEndOfFile(UINT16 HandleID);

/* These functions usefull to split and merge file paths
they will not work on actual paths, these paths are just string*/
KERNELDLL int SplitPath(const char *path, char *drive, char *dir, char *name, char *ext);
KERNELDLL void MergePath(char *path, const char *drive, const char *dir, const char *name, const char *ext);

#endif
