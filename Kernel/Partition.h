#ifndef __PARTITION_H
#define __PARTITION_H
/*
  Name: 		Partiton Class for Ace OS
  Author: 				Samuel ( samuelhard@yahoo.com)
  Description: 		This class makes easy to read a partition providing the 1st Sector information
  Date: 08-Sep-02 21:42
*/

#include "windef.h"
#include <Collect.h> 

#pragma pack(1)

struct _Partition
	{BYTE Bootable;            // Boot indicator. 0x80 for the  active partition and 0 for others
	BYTE StartSide;            // Side where the partition begins
	UINT16 StartSecCyl;          // The low 6 bits are the sector where the partition begins. The high 2 bits are the 2 high bits of the track where the partition begins.
	BYTE PartType;             // 0 - Unused partition   1 - DOS with 12-bit FAT   2 - Xenix partition   4 - DOS with 16-bit FAT (smaller than 32 mb) 5 - Extented partition   6 - 16 bit (larger than 32 mb)   0xB - 32 bit upto 2048 mb 0xC- same as 0xB but used LBA1 int 13 extensions 0xE - same as 6 but uses LBA1 int 13 extensions 0xF - Same as 5 but uses LBA1 int 13 extensions */
	BYTE EndSide;        	   // Low 8 bits of the track where the partitions begins.
	UINT16 EndSecCyl;            // The low 6 bits are the sector where the partition ends. The high 2 bits are the 2 high bits of the track where the partition ends.
	UINT32 PartitionBegining;  // This is very usefull when you calculate logical cluster number in FAT. Just add this number with the corresponding FATEntry's Cluster Number
	UINT32 NumberOfSectors;    // Number of sectors in the partition
	};

struct _PartitionTable
	{BYTE MBR[446];
	struct _Partition Part[4];
	UINT16 LastTwo;
	};

/* It contains the FAT Information for a specific partition */
class Partition
	{
	public:
		//member functions
		Partition(_Partition * Part);	
		Partition();
		~Partition();

		_Partition * Part;

		void Initialize(_Partition * Part);
		BYTE IsBootable();
		BYTE PartType();
		UINT32 PartitionBegining();
		UINT32 NumberOfSectors();
		UINT16 StartSide();
		UINT16 EndSide();
		UINT16 StartTrack();
		UINT16 EndTrack();
		UINT16 StartSector();
		UINT16 EndSector();
	};
BYTE EnumeratePartitions(BYTE *StorageKey, void (*CallBackFunction)(BYTE *DPF,Partition * Part), UINT32 LBASector=0, UINT32 PartitionDeepth='0' );

#pragma pack(1)

#endif
