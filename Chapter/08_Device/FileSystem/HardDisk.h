#ifndef __HARDDISK_H
#define __HARDDISK_H

#include "windef.h"
#include "Collect.H"

struct _GSH_IO_Parameter
{
	UINT32 Cylinder;
	UINT16 Head;
	UINT16 Sector;

	UINT32 LBASector;

	UINT16 SectorCount;

	BYTE   Mode;		//if bit 0 is set LBA mode

	UINT16 Others[20];
};

/*--------------- Error Constants -----------*/

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


struct __HDDInfo
	{
	BYTE IORegisterIdx ;  /* Index Number of IO Resources and IRQ */
	BYTE IRQ;

	BYTE DeviceNumber;		/* 0 - Master, 1 - Slave */
	char SerialNumber[21];
	char FirmwareRevision[21];
	char ModelNumber[21];
	BYTE DMASupported;
	BYTE LBASupported;

	BYTE DeviceID[512];		/* Device ID bytes Returned on IDENTIFY DEVICE Command*/

	BYTE Mode;			/* 0 - CHS Mode, 1-LBA Mode */

	UINT16 CHSHeadCount;
	UINT16 CHSCylinderCount;
	UINT16 CHSSectorCount;
	UINT32 LBACount;   		/* Used only in LBA mode */

	UINT16 BytesPerSector;

	BYTE LastError;
	};

class HardDiskHandler
	{
	private:
		Collection <__HDDInfo *> HDDs;
		void (*InterruptHandler)();
		static BYTE DoSoftwareReset(UINT16 DeviceController);
		BYTE LastError;
	public:
		HardDiskHandler();
		~HardDiskHandler();
		
		void Initialize();

		BYTE GetTotalDevices();

		BOOLEAN IsRemovableDevice(BYTE * DPF);
		BOOLEAN IsRemovableMedia (BYTE * DPF);

		__HDDInfo * GetHDDInfo(BYTE * DPF);

		UINT32 CHSToLBA(BYTE *DPF, UINT32 Cylinder, UINT32 Head, UINT32 Sector);
		void LBAToCHS(BYTE *DPF, UINT32 LBA, UINT32 * Cylinder, UINT32 * Head, UINT32 * Sector);

		BYTE ReadSectors(BYTE * DPF, UINT16 StartCylinder, BYTE StartHead, BYTE StartSector, BYTE NoOfSectors, BYTE * Buffer, BOOLEAN WithRetry = TRUE);
		BYTE ReadSectors(BYTE * DPF, UINT32 StartLBASector, BYTE NoOfSectors, BYTE * Buffer, BOOLEAN WithRetry = TRUE);
		BYTE WriteSectors(BYTE * DPF, UINT16 StartCylinder, BYTE StartHead, BYTE StartSector, BYTE NoOfSectors, BYTE * Buffer, BOOLEAN WithRetry = TRUE);
	

		//--------Implementation of GSH functions
		BYTE GetNoOfDevices();
		UINT16 GetDeviceParameters(BYTE * DPF, BYTE * Buffer);
		BYTE Reset(BYTE * DPF);		

		BYTE GetLastErrorCode()
		{
			return LastError;
		}
		char * GetLastError(BYTE ErrorCode);
		char * GetLastError()
		{
			return GetLastError(LastError);
		}
	};
extern HardDiskHandler* g_pHDDHandler;
#endif
