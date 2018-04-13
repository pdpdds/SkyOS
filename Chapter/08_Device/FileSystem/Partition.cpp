
#include "Partition.h"
#include "string.h"
#include "memory.h"
#include "ctype.h"
#include "SysInfo.h"


#define MAX_DEVICE_PATH 100

#define DEVICE_KEY_SIZE 20


#ifndef _GSH_IO_PARAMETER_DEFINED
#define _GSH_IO_PARAMETER_DEFINED
#endif   //this structure is used by kernel32.h so modify it also if you do any changes here

#ifdef _MSC_VER
#pragma pack (push, 1)
#endif

typedef struct tag_GSH_IO_Parameter
{
	UINT32 dwCylinder;
	UINT32 dwHead;
	UINT32 dwSector;

	UINT32 dwLBASector;

	UINT32 dwSectorCount;

	BYTE   bMode;		//if bit 0 is set LBA mode

	UINT32 Reserved[20];
}_GSH_IO_Parameter;

#ifdef _MSC_VER
#pragma pack (pop)
#endif

typedef _GSH_IO_Parameter GSHIOPARA;
typedef GSHIOPARA *LPGSHIOPARA;




DEVICE_PATH_LOOKUP sysDevicePathLookupTable[MAX_DRIVE_LETTERS];
int bTotalDrives=0;
int GetTotalDrives()
{
    return bTotalDrives;
}
LPDEVICE_PATH_LOOKUP GetDevicePathFromDriveLetter(char DriveLetter)
{
    if ( isalpha(DriveLetter) )
        return &sysDevicePathLookupTable[ toupper(DriveLetter)-'A' ];
    return NULL;
}

/*This function should use GSH to loop through all the storage devices and find partitions*/
/*void FillDevicePathLookupTable()
{
    int i;
    //initialize all to zero
    for(i=0;i<MAX_DRIVE_LETTERS;i++)
    {
        sysDevicePathLookupTable[i].szDevicePath[0]=0;
        memset( &sysDevicePathLookupTable[i].Part, 0, sizeof(PARTITION) );   
    }
    //reserve the first 2 entries for floppy
    strcpy(sysDevicePathLookupTable[0].szDevicePath,"Floppy0\\Partition0");
    memset( &sysDevicePathLookupTable[0].Part, 0, sizeof(PARTITION) );
    strcpy(sysDevicePathLookupTable[1].szDevicePath,"Floppy1\\Partition0");
    memset( &sysDevicePathLookupTable[1].Part, 0, sizeof(PARTITION) );
    bTotalDrives=2;
    
    GSH_GetRegisteredDeviceTypes( FillDevicePathLookupTableForDevice );
}
BYTE FillDevicePathLookupTableForDevice(LPCSTR szDeviceKey)
{
    char szDevicePath[50];
    int i;

    int iTotalDevices = GSH_GetNoOfDevices(szDeviceKey);
    for( i=0;i<iTotalDevices; i++ )
    {
        strcpy(szDevicePath, szDeviceKey);
        szDevicePath[strlen(szDevicePath)+1]=0;
        szDevicePath[strlen(szDevicePath)] = i + '0';
        //printf("\n\Adding device %s", szDevicePath);
        FillDriveLetters(szDevicePath, 0, 0);
    }
    return 1;
}*/

/*BYTE FillDriveLetters(LPCSTR szDevicePath, UINT32 dwStartLBASector, BYTE bPartitionNo)
{
    BYTE PT[512];
    GSHIOPARA m_IOPara;
    
    m_IOPara.bMode=1;	//LBA Mode
    m_IOPara.dwLBASector=dwStartLBASector;
    m_IOPara.dwSectorCount=1;
    
    GSH_Read(szDevicePath, &m_IOPara , (BYTE *)&PT);
    if ( GetLastError() != ERROR_SUCCESS )
    {
        //printf("\n\r Partition :: Read Error - %d", GetLastError() );
        return 0;
    }
    else
    {
        LPPARTITION lpPart;
        #ifdef _DEBUG
            lpPart = ((PARTITION *)&PT[446+(0*sizeof(PARTITION))]);
            dbgprintf("\n Partition 0 : Type %X Beg %ld Tot %ld ", PART_TYPE(lpPart), PART_BEGINING(lpPart), PART_NUMBEROFSECTORS(lpPart));
            lpPart = ((PARTITION *)&PT[446+(1*sizeof(PARTITION))]);
            dbgprintf("\n Partition 1 : Type %X Beg %ld Tot %ld ", PART_TYPE(lpPart), PART_BEGINING(lpPart), PART_NUMBEROFSECTORS(lpPart));
            lpPart = ((PARTITION *)&PT[446+(2*sizeof(PARTITION))]);
            dbgprintf("\n Partition 2 : Type %X Beg %ld Tot %ld ", PART_TYPE(lpPart), PART_BEGINING(lpPart), PART_NUMBEROFSECTORS(lpPart));
            lpPart = ((PARTITION *)&PT[446+(3*sizeof(PARTITION))]);
            dbgprintf("\n Partition 3 : Type %X Beg %ld Tot %ld ", PART_TYPE(lpPart), PART_BEGINING(lpPart), PART_NUMBEROFSECTORS(lpPart));
        #endif
        int i;
        for (i=0;i<4;i++)
        {
            lpPart =((PARTITION *)&PT[446+(i*sizeof(PARTITION))]);
            //printf("\n\r Partition Type [%X]", lpPart->PartType);
            
            if ( lpPart->PartType == 0x5 || lpPart->PartType == 0xF ) //Exteneded Partition
            {
                #ifdef _DEBUG
                    dbgprintf("\n\r Extened Partition ");
                #endif
                //printf("\n\r \t***** Extened part %X", lpPart->PartType);
                BYTE bResult;
                bResult = FillDriveLetters(szDevicePath, lpPart->PartitionBegining+dwStartLBASector, bPartitionNo+1);
                if ( bResult != 0 )
                    bPartitionNo = bResult;
            }
            else if ( lpPart->PartType != 0 )
            {
                char szPartitionPath[50];
                lpPart->PartitionBegining += dwStartLBASector;
                
                strcpy(szPartitionPath, szDevicePath);
                strcat(szPartitionPath, "\\Partition ");
                szPartitionPath[ strlen(szPartitionPath)-1] = bPartitionNo+'0';
                strcpy( sysDevicePathLookupTable[ bTotalDrives ].szDevicePath, szPartitionPath );
                memcpy( &sysDevicePathLookupTable[ bTotalDrives ].Part, lpPart, sizeof(PARTITION) );
                bTotalDrives++;
                //printf("\n\r\tAdding Partition %s", szPartitionPath);
            }
            bPartitionNo++;
        }
    }
    return bPartitionNo;
    
}*/
char * GetPartititionTypeString(BYTE PartitionType)
{
    return szPartitionTypes[PartitionType-1];
}

char szPartitionTypes[255][50]=
{
    "DOS 12-bit FAT",
    "XENIX root",
    "XENIX /usr",
    "DOS 3.0+ 16-bit FAT (up to 32M)",
    "DOS 3.3+ Extended Partition",
    "DOS 3.31+ 16-bit FAT (over 32M)",
    "NTFS / HPFS",
    "QNX / OS-2 / AIX / SplitDrive",
    "QNX / AIX / Coherent filesystem",
    "OS/2 Boot Manager",
    "WIN95 OSR2 32-bit FAT",
    "WIN95 OSR2 32-bit FAT, LBA-mapped",
    "Unused",
    "WIN95: DOS 16-bit FAT, LBA-mapped",
    "WIN95: Extended partition, LBA-mapped",
    "Unused",
    "Hidden DOS 12-bit FAT",
    "Compaq config partition",
    "Unused",
    "Hidden DOS 16-bit FAT <32M",
    "Unused",
    "Hidden DOS 16-bit FAT >=32M",
    "Hidden IFS",
    "AST SmartSleep Partition",
    "Unused  (Claimed for Willowtech Photon COS)",
    "Unused",
    "Hidden WIN95 OSR2 32-bit FAT",
    "Hidden WIN95 OSR2 32-bit FAT, LBA-mapped",
    "Unused",
    "Hidden WIN95 16-bit FAT, LBA-mapped",
    "Unused",
    "Unused",
    "Unused",
    "Unused",
    "Reserved",
    "NEC DOS 3.x",
    "Unused",
    "Reserved",
    "Unused",
    "Unused",
    "Unused",
    "Unused",
    "Unused",
    "Unused",
    "Unused",
    "Unused",
    "Unused",
    "Unused",
    "Reserved",
    "NOS",
    "Reserved",
    "Reserved",
    "JFS on OS/2 or eCS",
    "Reserved",
    "Unused",
    "THEOS",
    "Plan 9 partition",
    "THEOS ver 4 4gb partition",
    "THEOS ver 4 extended partition",
    "PartitionMagic recovery partition",
    "Hidden NetWare",
    "Unused",
    "Unused",
    "Venix 80286",
    "Linux/MINIX (sharing disk with DRDOS)",
    "Linux swap (sharing disk with DRDOS)",
    "Linux native (sharing disk with DRDOS)",
    "GoBack partition",
    "Boot-US boot manager",
    "EUMEL/Elan",
    "EUMEL/Elan",
    "EUMEL/Elan",
    "Unused",
    "AdaOS Aquila (Default)",
    "Unused",
    "Oberon partition",
    "QNX4.x",
    "QNX4.x 2nd part",
    "QNX4.x 3rd part",
    "Lynx RTOS",
    "OnTrack Disk Manager RW (DM6 Aux1)",
    "CP/M",
    "Disk Manager 6.0 Aux3",
    "Disk Manager 6.0 Dynamic Drive Overlay",
    "EZ-Drive",
    "Golden Bow VFeature Partitioned Volume.",
    "DrivePro",
    "Unused",
    "Unused",
    "Unused",
    "Unused",
    "Priam EDisk",
    "Unused",
    "Unused",
    "Unused",
    "Unused",
    "SpeedStor",
    "Unused",
    "Unix System V Mach, GNU Hurd",
    "Novell Netware 286, 2.xx",
    "Novell Netware 386, 3.xx or 4.xx",
    "Novell Netware SMS Partition",
    "Novell",
    "Novell",
    "Novell Netware 5+, Novell Netware NSS Partition",
    "Unused",
    "Unused",
    "Unused",
    "Unused",
    "Unused",
    "Unused",
    "DiskSecure Multi-Boot",
    "Reserved",
    "Unused",
    "Reserved",
    "Reserved",
    "IBM PC/IX",
    "Reserved",
    "M2FS/M2CS partition",
    "XOSL FS",
    "Unused",
    "Unused",
    "Unused",
    "Unused",
    "Unused",
    "Unused",
    "Unused",
    "MINIX until 1.4a",
    "MINIX since 1.4b, early Linux",
    "Solaris x86 / Linux swap",
    "Linux native (usually ext2fs)",
    "OS/2 hidden C: drive",
    "Linux extended partition",
    "NTFS volume set",
    "NTFS volume set",
    "Unused",
    "Unused",
    "Linux Kernel Partition (used by AiR-BOOT)",
    "Legacy Fault Tolerant FAT32 volume",
    "Legacy Fault Tolerant FAT32 volume",
    "Free FDISK hidden Primary DOS FAT12 partitition",
    "Linux Logical Volume Manager partition",
    "Unused",
    "Free FDISK hidden Primary DOS FAT16 partitition",
    "Free FDISK hidden DOS extended partitition",
    "Hidden Primary DOS large FAT16 partitition",
    "Hidden Linux native partition",
    "Amoeba bad block table",
    "MIT EXOPC native partitions",
    "Unused",
    "Hidden Primary DOS FAT32 partitition",
    "Hidden Primary DOS FAT32 partitition (LBA)",
    "DCE376 logical drive",
    "Hidden Primary DOS FAT16 partitition (LBA)",
    "Hidden DOS extended partitition (LBA)",
    "Unused",
    "Unused",
    "Unused",
    "BSD/OS",
    "Laptop hibernation partition",
    "HP Volume Expansion (SpeedStor variant)",
    "Unused",
    "Reserved",
    "Reserved",
    "BSD/386, 386BSD, NetBSD, FreeBSD",
    "OpenBSD",
    "NEXTSTEP",
    "Mac OS-X",
    "NetBSD",
    "Olivetti FAT 12 1.44Mb Service Partition",
    "Mac OS-X Boot partition",
    "Unused",
    "Unused",
    "ShagOS filesystem",
    "ShagOS swap partition",
    "BootStar Dummy",
    "Reserved",
    "Unused",
    "Reserved",
    "Reserved",
    "Unused",
    "Reserved",
    "BSDI BSD/386 filesystem",
    "BSDI BSD/386 swap partition",
    "Boot Wizard hidden",
    "Solaris 8 boot partition",
    "Unused",
    "CTOS",
    "DRDOS/secured (FAT-12)",
    "Hidden Linux",
    "Hidden Linux swap",
    "DRDOS/secured (FAT-16, < 32M)",
    "DRDOS/secured (extended)",
    "DRDOS/secured (FAT-16, >= 32M)",
    "Windows NT corrupted NTFS volume/stripe set",
    "Unused",
    "Unused",
    "Unused",
    "reserved for DRDOS/secured (FAT32)",
    "reserved for DRDOS/secured (FAT32, LBA)",
    "CTOS Memdump?",
    "reserved for DRDOS/secured (FAT16, LBA)",
    "Unused",
    "REAL/32 secure big partition",
    "Old Multiuser DOS secured FAT12",
    "Unused",
    "Unused",
    "Old Multiuser DOS secured FAT16 <32M",
    "Old Multiuser DOS secured extended partition",
    "Old Multiuser DOS secured FAT16 >=32M",
    "Unused",
    "CP/M-86",
    "Unused",
    "Non-FS Data",
    "Digital Research CP/M, Concurrent CP/M, DOS",
    "Unused",
    "Hidden CTOS Memdump?",
    "Dell PowerEdge Server utilities (FAT FS)",
    "DG/UX virtual disk manager partition",
    "ST AVFS.",
    "SpeedStor 12-bit FAT extended partition",
    "Unused",
    "DOS R/O or SpeedStor",
    "SpeedStor 16-bit FAT extended partition",
    "Reserved",
    "Reserved",
    "Unused",
    "Unused",
    "Unused",
    "Unused",
    "BFS  (aka BeFS)",
    "Unused",
    "Reserved for Matthias Paul's Sprytix",
    "Unused",
    "Partition that contains an EFI file system",
    "Linux/PA-RISC boot loader",
    "SpeedStor",
    "DOS 3.3+ secondary partition",
    "Reserved",
    "SpeedStor large partition",
    "Prologue multi-volume partition",
    "Reserved",
    "Unused",
    "Unused",
    "Unused",
    "Bochs",
    "VMware File System partition",
    "VMware Swap partition",
    "Linux RAID",
    "SpeedStor > 1024 cyl.",
    "LANstep",
    "IBM PS/2 IML (Initial Microcode Load) ",
    "Windows NT Disk Administrator hidden partition",
    "Linux Logical Volume Manager partition",
    "Xenix Bad Block Table",
};


