/*
  Name: 		Partiton Class for Ace OS
  Author: 				Samuel ( samuelhard@yahoo.com)
  Description: 		This class makes easy to read a partition providing the 1st Sector information
  Date: 08-Sep-02 21:42
*/

#include <Partition.h>
#include <HardDisk.h>
#include "kheap.h"
#include "MultiBoot.h"
#include "SkyConsole.h"
#include "string.h"

extern HardDiskHandler * g_pHDDHandler;

//----------------Partition class functions------------------------
Partition::Partition(_Partition * Part)

{
	Initialize(Part);
}

Partition::Partition()
{
}
Partition::~Partition()
{
}

void Partition::Initialize(_Partition * part)
{
	this->Part = (_Partition *)kmalloc(sizeof(_Partition));

	//MemCpy((BYTE *)this->Part,(BYTE *)Part,sizeof(this->Part) );

	this->Part->Bootable= part->Bootable;
	this->Part->StartSide= part->StartSide;
	this->Part->StartSecCyl= part->StartSecCyl;
	this->Part->PartType= part->PartType;
	this->Part->EndSide= part->EndSide;
	this->Part->EndSecCyl= part->EndSecCyl;
	this->Part->PartitionBegining= part->PartitionBegining;
	this->Part->NumberOfSectors= part->NumberOfSectors;
	}
BYTE Partition::IsBootable()
	{return Part->Bootable==0x80?TRUE:FALSE;
	}
BYTE Partition::PartType()
	{return Part->PartType;
	}
UINT32 Partition::PartitionBegining()
	{return Part->PartitionBegining;
	}
UINT32 Partition::NumberOfSectors()
	{return Part->NumberOfSectors;
	}
UINT16 Partition::StartSide()
	{return Part->StartSide;
	}
UINT16 Partition::EndSide()
	{return Part->EndSide;
	}
UINT16 Partition::StartTrack()
	{return ( ( Part->StartSecCyl & 0xFF00 ) >> 8 ) | ( ( Part->StartSecCyl & 0x00C0 ) << 2 );
	}
UINT16 Partition::EndTrack()
	{return ( ( Part->EndSecCyl & 0xFF00 ) >> 8 ) | ( ( Part->EndSecCyl & 0x00C0 ) << 2 );
	}
UINT16 Partition::StartSector()
	{return Part->StartSecCyl & 0x3F;
	}
UINT16 Partition::EndSector()
	{return Part->EndSecCyl & 0x3F;
	}
//--------------------------------- General Functions ----------------------------------
/* returns the result of the harddisk read operation */
BYTE PartitionDPF[20];
extern void CallBackPartition(BYTE * DPF, Partition * Part);
BYTE EnumeratePartitions(BYTE *StorageKey, void(*CallBackFunction)(BYTE * DPF, Partition * Part), UINT32 LBASector, UINT32 PartitionDeepth)
{
	BYTE PT[512];
	BYTE ReadResult;
	void(*CallBackFn)(BYTE * DPF, Partition * Part);

	strcpy((char *)PartitionDPF, (char *)StorageKey);
	BYTE Len = (BYTE)strlen((char *)PartitionDPF);
	PartitionDPF[Len] = '\\';
	PartitionDPF[Len + 1] = PartitionDeepth;
	PartitionDPF[Len + 2] = 0;

	//CallBackFn = (void(*)(BYTE *, Partition *)) ((UINT32)CallBackFunction - KERNEL_LOAD_ADDRESS);
	CallBackFn = (void(*)(BYTE *, Partition *)) ((UINT32)CallBackPartition);
	ReadResult = g_pHDDHandler->ReadSectors(StorageKey, LBASector, 1, (BYTE *)&PT, TRUE);

	if (ReadResult != HDD_NO_ERROR)
	{		
		return ReadResult;
	}
	else
	{		
		for (int i = 0; i < 4; i++)
		{
			Partition Part((struct _Partition *)&PT[446 + (i * sizeof(struct _Partition))]);
			if (Part.PartType() != 0)
				CallBackFn(PartitionDPF, &Part);
			if (Part.PartType() == 0x5 || Part.PartType() == 0xF) //Exteneded Partition
				EnumeratePartitions(StorageKey, CallBackFunction, Part.PartitionBegining(), ++PartitionDeepth);
		}
	}
	return HDD_NO_ERROR;
}

