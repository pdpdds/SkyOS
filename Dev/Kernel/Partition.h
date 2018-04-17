#pragma once

#include "windef.h"
//#include "ACEList.h"
#include "GSH.H"

#define PART_ISBOOTABLE(Part)  ( ((struct _Partition * )Part)->Bootable==0x80?TRUE:FALSE)

#define PART_TYPE(Part)        (((struct _Partition * )Part)->PartType)

#define PART_BEGINING(Part)    (((struct _Partition * )Part)->PartitionBegining)

#define PART_NUMBEROFSECTORS(Part) (((struct _Partition * )Part)->NumberOfSectors)

#define PART_STARTSIDE(Part)   (((struct _Partition * )Part)->StartSide)

#define PART_ENDSIDE(Part)     (((struct _Partition * )Part)->EndSide)

#define PART_STARTTRACK(Part)  ( ( ((struct _Partition * )Part)->StartSecCyl & 0xFF00 ) >> 8 ) | ( ( ((struct _Partition * )Part)->StartSecCyl & 0x00C0 ) << 2 )

#define PART_ENDTRACK(Part)    ( (((struct _Partition * )Part)->EndSecCyl & 0xFF00 ) >> 8 ) | ( ( ((struct _Partition * )Part)->EndSecCyl & 0x00C0 ) << 2 )

#define PART_STARTSECTOR(Part) (((struct _Partition * )Part)->StartSecCyl & 0x3F)

#define PART_ENDSECTOR(Part)   (((struct _Partition * )Part)->EndSecCyl & 0x3F)


#ifdef _MSC_VER
#pragma pack (push, 1)
#endif
struct _Partition
{
    BYTE Bootable;              // Boot indicator. 0x80 for the  active partition and 0 for others
    BYTE StartSide;             // Side where the partition begins
    UINT16 StartSecCyl;         // The low 6 bits are the sector where the partition begins. The high 2 bits are the 2 high bits of the track where the partition begins.
    BYTE PartType;              // 0 - Unused partition   1 - DOS with 12-bit FAT   2 - Xenix partition   4 - DOS with 16-bit FAT (smaller than 32 mb) 5 - Extented partition   6 - 16 bit (larger than 32 mb)   0xB - 32 bit upto 2048 mb 0xC- same as 0xB but used LBA1 int 13 extensions 0xE - same as 6 but uses LBA1 int 13 extensions 0xF - Same as 5 but uses LBA1 int 13 extensions */
    BYTE EndSide;        	// Low 8 bits of the track where the partitions begins.
    UINT16 EndSecCyl;           // The low 6 bits are the sector where the partition ends. The high 2 bits are the 2 high bits of the track where the partition ends.
    UINT32 PartitionBegining;   // This is very usefull when you calculate logical cluster number in FAT. Just add this number with the corresponding FATEntry's Cluster Number
    UINT32 NumberOfSectors;     // Number of sectors in the partition
};

typedef struct _Partition PARTITION;
typedef PARTITION * LPPARTITION;

struct _PartitionTable
{
    BYTE MBR[446];
    struct _Partition Part[4];
    UINT16 LastTwo;
};

#ifdef _MSC_VER
#pragma pack (pop)
#endif

#define MAX_DRIVE_LETTERS 26
struct _DevicePathLookup
{
    char szDevicePath[MAX_DEVICE_PATH];
    PARTITION Part;
};

typedef struct _DevicePathLookup DEVICE_PATH_LOOKUP;
typedef DEVICE_PATH_LOOKUP * LPDEVICE_PATH_LOOKUP;

extern DEVICE_PATH_LOOKUP sysDevicePathLookupTable[MAX_DRIVE_LETTERS];

extern char szPartitionTypes[255][50];

LPDEVICE_PATH_LOOKUP GetDevicePathFromDriveLetter(char DriveLetter);
void FillDevicePathLookupTable();
BYTE FillDevicePathLookupTableForDevice(LPCSTR szDeviceKey);
BYTE FillDriveLetters(LPCSTR szDevicePath, UINT32 dwStartLBASector, BYTE bPartitionNo);
int GetTotalDrives();
char * GetPartititionTypeString(BYTE PartitionType);
