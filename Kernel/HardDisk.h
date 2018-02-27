#pragma once
#include "windef.h"
#include "stdint.h"
#include "GSH.H"

enum HDD_Errors
{
    HDD_NO_ERROR,
    HDD_NOT_FOUND,
    HDD_CONTROLLER_BUSY,
    HDD_DATA_NOT_READY,
    HDD_DATA_COMMAND_NOT_READY,

    HDD_ERR_UNC, 	//Uncorrectable ECC
    HDD_ERR_MC,     //Media Changed
    HDD_ERR_IDNF,   //ID Not Found
    HDD_ERR_MCR,    //Media Change Request
    HDD_ERR_ABRT,   //Command Aborted
    HDD_ERR_TK0NF   //Track 0 Not Found
};

//#warning The following definition should be only 20 but if you set anything below 25 it causes page fault. I suspect Heap module.
#define IDE_STRING_NUMBER_SIZE 25
struct _HDD
{
    BYTE bIORegisterIdx;        /* Index Number of IO Resources and IRQ */
    BYTE bIRQ;

    BYTE bDeviceNumber;		/* 0 - Master, 1 - Slave */
    
    BYTE bDeviceID[512];	/* Device ID bytes Returned on IDENTIFY DEVICE Command*/

    BYTE bMode;			/* 0 - CHS Mode, 1-LBA Mode */

    UINT32 dwFeatures;          /* bit  0 LBA 
                                        1 DMA
                                */
    UINT16 wCHSHeadCount;
    UINT16 wCHSCylinderCount;
    UINT16 wCHSSectorCount;
    UINT32 dwLBACount;   	/* Used only in LBA mode */

    UINT16 wBytesPerSector;
    
    char szSerialNumber[IDE_STRING_NUMBER_SIZE];
    char szModelNumber[IDE_STRING_NUMBER_SIZE];
    char szFirmwareRevision[IDE_STRING_NUMBER_SIZE];
};
typedef struct _HDD HDD;
typedef HDD * LPHDD;

#ifdef __cplusplus
	extern "C" {
#endif
    extern LPHDD sysHDDArray[];
    BYTE ReadErrorRegister(BYTE bDeviceController);
    BOOLEAN IsDeviceDataReady(BYTE bDeviceController, DWORD dwWaitUpToms,BOOLEAN bCheckDataRequest);
    BOOLEAN IsDeviceControllerBusy(BYTE bDeviceController, DWORD dwWaitUpToms);
    BYTE HardDisk_DoSoftwareReset(BYTE bDeviceController);
    BOOLEAN HardDisk_IsRemovableDevice(BYTE bDeviceNo);
    BOOLEAN HardDisk_IsRemovableMedia(BYTE bDeviceNo);
    
    UINT32 HardDisk_CHSToLBA(BYTE bDeviceNo, UINT32 dwCylinder, UINT32 dwHead, UINT32 dwSector);
    UINT32 HardDisk_LBAToCHS(BYTE bDeviceNo,UINT32 dwLBA, UINT32 * lpCylinder, UINT32 * lpHead, UINT32 * lpSector);
    void HardDisk_Initialize();
    UINT32 HardDisk_Reset(BYTE bDeviceNo);
    UINT32 HardDisk_ReadSectors(BYTE bDeviceNo, UINT32 dwStartLBASector, BYTE bNoOfSectors, BYTE * lpBuffer);
    UINT32 HardDisk_WriteSectors(BYTE bDeviceNo, UINT32 dwStartLBASector, BYTE bNoOfSectors, BYTE * lpBuffer);

    //--------Implementation of GSH functions
    BYTE   HDD_GetNoOfDevices();
    UINT32 HDD_GetDeviceParameters(BYTE bDeviceNo, BYTE * lpBuffer);

    UINT32 HDD_Reset(BYTE bDeviceNo);
    UINT32 HDD_Status(BYTE bDeviceNo);

    UINT32 HDD_Verify(BYTE bDeviceNo,  LPGSHIOPARA lpIOPara);
    UINT32 HDD_Read(BYTE bDeviceNo,  LPGSHIOPARA lpIOPara, BYTE * lpBuffer);
    UINT32 HDD_Write(BYTE bDeviceNo,  LPGSHIOPARA lpIOPara, BYTE * lpBuffer);
#ifdef __cplusplus
    }
#endif
