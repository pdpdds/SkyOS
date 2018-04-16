#include "HardDisk.h"
#include "windef.h"
#include "kheap.h"
#include "SkyConsole.h"
#include "Hal.h"
#include "string.h"
#include "memory.h"
#include "sprintf.h"
#include "SysInfo.h"

/*---IDE Controller's Compability mode Base Registers and Interrupts ---*/
const BYTE IDE_MAX_CONTROLLER = 4;
const UINT16 IDE_Con_IOBases[IDE_MAX_CONTROLLER][2] =
{
	{ 0x1F0, 0x3F6 },
	{ 0x170, 0x376 },
	{ 0x1E8, 0x3EE },
	{ 0x168, 0x36E }
};
const UINT16 IDE_Con_IRQs[IDE_MAX_CONTROLLER][3] =
{
	{ 14, 0, 0 },
	{ 15, 0, 0 },
	{ 11, 12, 9 },
	{ 10, 12, 9 }
};


/* ---------- Command Block Registers Offset From Base ------------------*/
const BYTE IDE_CB_DATA = 0;       /* Read & Write */
const BYTE IDE_CB_ERROR = 1;	/* Read only    */
const BYTE IDE_CB_FEATURES = 1;	/* Write only   */
const BYTE IDE_CB_STATUS = 7;     /* Read only    */
const BYTE IDE_CB_COMMAND = 7;    /* Write only   */


const BYTE IDE_CB_SECTOR_COUNT = 2;

const BYTE IDE_CB_SECTOR = 3;
const BYTE IDE_CB_CYLINDER_LOW = 4;
const BYTE IDE_CB_CYLINDER_HIGH = 5;
const BYTE IDE_CB_DEVICE_HEAD = 6;

const BYTE IDE_CB_LBA_0_7 = 3;
const BYTE IDE_CB_LBA_8_15 = 4;
const BYTE IDE_CB_LBA_16_23 = 5;
const BYTE IDE_CB_LBA_24_27 = 6;

/* ---------- Control Block Registers Constants ------------------*/
BYTE IDE_CON_BSY = 7;
BYTE IDE_CON_DRDY = 6;
BYTE IDE_CON_DF = 5;
BYTE IDE_CON_DSC = 4;
BYTE IDE_CON_DRQ = 3;
BYTE IDE_CON_CORR = 2;
BYTE IDE_CON_IDX = 1;
BYTE IDE_CON_ERR = 0;


/* ---------- Control Block Registers Offset From Base ------------*/
const BYTE IDE_CON_ALTERNATE_STATUS = 2;
const BYTE IDE_CON_DEVICE_CONTROL = 2;

/*-------------     ATA - 2  Command's OpCodes  -------------------*/
const BYTE IDE_COM_ACKNOWLEDGE_MEDIA_CHANGE = 0xDB;
const BYTE IDE_COM_POST_BOOT = 0xDC;
const BYTE IDE_COM_PRE_BOOT = 0xDD;

/*const BYTE IDE_COM_CHECK_POWER_MODE=98hE5h */

const BYTE IDE_COM_DOOR_LOCK = 0xDE;
const BYTE IDE_COM_DOOR_UNLOCK = 0xDF;
const BYTE IDE_COM_DOWNLOAD_MICROCODE = 0x92;
const BYTE IDE_COM_EXECUTE_DEVICE_DIAGNOSTIC = 0x90;
const BYTE IDE_COM_FORMAT_TRACK = 0x50;
const BYTE IDE_COM_IDENTIFY_DEVICE = 0xEC;

/*const BYTE IDE_COM_IDLE=97hE3h
const BYTE IDE_COM_IDLE_IMMEDIATE=0x95hE1h */

const BYTE IDE_COM_INITIALIZE_DEVICE_PARAMETERS = 0x91;
const BYTE IDE_COM_MEDIA_EJECT = 0xED;
const BYTE IDE_COM_NOP = 0x0;
const BYTE IDE_COM_READ_BUFFER = 0xE4;
const BYTE IDE_COM_READ_DMA_W_RETRY = 0xC8;
const BYTE IDE_COM_READ_DMA = 0xC9;
const BYTE IDE_COM_READ_LONG_W_RETRY = 0x22;
const BYTE IDE_COM_READ_LONG = 0x23;
const BYTE IDE_COM_READ_MULTIPLE = 0xC4;
const BYTE IDE_COM_READ_SECTORS_W_RETRY = 0x20;
const BYTE IDE_COM_READ_SECTORS = 0x21;
const BYTE IDE_COM_READ_VERIFY_SECTORS_W_RETRY = 0x40;
const BYTE IDE_COM_READ_VERIFY_SECTORS = 0x41;
const BYTE IDE_COM_RECALIBRATE = 0x10;
const BYTE IDE_COM_SEEK = 0x70;
const BYTE IDE_COM_SET_FEATURES = 0xEF;
const BYTE IDE_COM_SET_MULTIPLE_MODE = 0xC6;

/*const BYTE IDE_COM_SimpleSleep=99hE6h
const BYTE IDE_COM_STANDBY=96hE2h
const BYTE IDE_COM_STANDBY_IMMEDIATE=94hE0h */

const BYTE IDE_COM_WRITE_BUFFER = 0xE8;
const BYTE IDE_COM_WRITE_DMA_W_RETRY = 0xCA;
const BYTE IDE_COM_WRITE_DMA = 0xCB;
const BYTE IDE_COM_WRITE_LONG_W_RETRY = 0x32;
const BYTE IDE_COM_WRITE_LONG = 0x33;
const BYTE IDE_COM_WRITE_MULTIPLE = 0xC5;
const BYTE IDE_COM_WRITE_SAME = 0xE9;
const BYTE IDE_COM_WRITE_SECTORS_W_RETRY = 0x30;
const BYTE IDE_COM_WRITE_SECTORS = 0x31;
const BYTE IDE_COM_WRITE_VERIFY = 0x3C;


__declspec(naked) void _HDDInterruptHandler() {

	_asm {
		cli
		pushad
	}

	_asm {
		mov al, 0x20
		out 0x20, al
		popad
		sti
		iretd
	}
}
/*
   -----------------------------------------------------------
   The following structure is used to filled during the initialization of
   [HardDiskHandler]. It is stored in the Collection and accessed using the
   index of Collection. During detection of a harddisk if one fails then it
   is replaced by the next harddisk.
   The Count of this collection returns the total detected harddisks.
   -----------------------------------------------------------
   */

//---------------------------------------------------------------------
//              This function returns the number harddisks founded
//---------------------------------------------------------------------
BYTE HardDiskHandler::GetTotalDevices()
{
	return (BYTE)HDDs.Count();
}
//---------------------------------------------------------------------
//        This function returns the description of the last error
//---------------------------------------------------------------------
char * HardDiskHandler::GetLastError(BYTE ErrorCode)
{
	switch (ErrorCode)
	{
	case HDD_NO_ERROR:
		return "No Error";
	case HDD_NOT_FOUND:
		return "HDD Not Found";
	case HDD_CONTROLLER_BUSY:
		return "Device Controller Busy";
	case HDD_DATA_NOT_READY:
		return "Device Data Not Ready";
	case HDD_DATA_COMMAND_NOT_READY:
		return "Device not ready";
	default:
		return "Undefined Error";
	}
}
/* This function returns the Error Register value*/
BYTE ReadErrorRegister(BYTE DeviceController)
{
	BYTE Status = InPortByte(IDE_Con_IOBases[DeviceController][0] + IDE_CB_STATUS);
	if ((Status & 0x80) == 0 && (Status & 0x1)) //busy bit=0 and err bit=1
	{
		Status = InPortByte(IDE_Con_IOBases[DeviceController][0] + IDE_CB_ERROR);
		return Status;
	}
	else
		return 0;
}
/*
   -----------------------------------------------------------
   This function checks whether the given device ready for data transfer or receive
   Input :
   DeviceController - Index number of Device to test  ( 0 to IDE_MAX_CONTROLLER )
   Wait UpTo ms - Optional Time in milli second to check for the busy status
   Output :
   Returns 'TRUE' if the device is ready else 'FALSE'
   -----------------------------------------------------------
   */
#include "pit.h"
BOOLEAN IsDeviceDataReady(int DeviceController, DWORD WaitUpToms = 0, BOOLEAN CheckDataRequest = TRUE)
{
	UINT32 Time1, Time2;
	Time1 = GetTickCount();
	do
	{
		UINT16 PortID = IDE_Con_IOBases[DeviceController][0] + IDE_CB_STATUS;
		BYTE Status = InPortByte(PortID);
		if ((Status & 0x80) == 0) //Checking BSY bit, because DRDY bit is valid only when BSY is zero
		{
			if (Status & 0x40) //checking DRDY is set
				if (CheckDataRequest) // if DataRequest is also needed
				{
					if (Status & 0x8) // DRQ bit set
					{						
						return TRUE;
					}
				}
				else
				{					
					return TRUE;
				}
		}
		Time2 = GetTickCount();
	} while ((Time2 - Time1) < WaitUpToms);

	return FALSE;
}
/*
  ---------------------------------------------------------
  This function checks whether the given device controlller is busy or not.
  Input :
  DeviceController - Device Number ( 0 to IDE_MAX_CONTROLLER )
  Wait UpTo ms - Optional Time in milli second to check for the busy status
  Output :
  Returns 'TRUE' if the device is busy else 'FALSE'
  -----------------------------------------------------------
  */
#include "pit.h"
BOOLEAN IsDeviceControllerBusy(int DeviceController, int WaitUpToms = 0)
{
	UINT32 Time1, Time2;
	Time1 = GetTickCount();
	do	{
		
		UINT16 PortID = IDE_Con_IOBases[DeviceController][0] + IDE_CB_STATUS;
		BYTE Status = InPortByte(PortID);
		if ((Status & 0x80) == 0) //BSY bit 
			return FALSE;
		Time2 = GetTickCount();
	} while ((Time2 - Time1) <= (UINT32)WaitUpToms);
	
	return TRUE;
}

/*
This function performs a Software Reset on the devices attached. You may
specifing only one or one portion of a hard disk but all harddisks and
entire space is subjected to sofware reset and the status is posted in the
Error Register.

1) Set the SRST bit to 1, wait for 400ns
2) Clear the SRST bit to 0
3) Wait for BSY bit cleared in the Status Register upto 31 seconds
4) Read the Error Register
Input  :
DeviceController Number
Output :
( Output == 0x1 )
Device 0 passed, Device 1 passed or not present
( Output == 0x0 || ( Output >=0x2 && Output <=0x7F ) )
Device 0 failed, Device 1 passed or not present
( Output == 0x81 )
Device 0 passed, Device 1 failed
( Output == 0x80 || ( Output >=0x82 && Output <=0xFF ) )
Device 0 failed, Device 1 failed

*/
BYTE HardDiskHandler::DoSoftwareReset(UINT16 DeviceController)
{
	BYTE DeviceControl = 4; //Setting SRST bit in the control register ( 2nd bit  (bit count 3) )
	OutPortByte(IDE_Con_IOBases[DeviceController][0] + IDE_CON_DEVICE_CONTROL, DeviceControl);
	DeviceControl = 0;      //Clearing the SRST bit in the control register ( 2nd bit)
	OutPortByte(IDE_Con_IOBases[DeviceController][0] + IDE_CON_DEVICE_CONTROL, DeviceControl);

	return InPortByte(IDE_Con_IOBases[DeviceController][0] + IDE_CB_ERROR);
}
BOOLEAN HardDiskHandler::IsRemovableDevice(BYTE * DPF)
{
	return !(HDDs.Item((char *)DPF)->DeviceID[0] & 0x70);
}
BOOLEAN HardDiskHandler::IsRemovableMedia(BYTE * DPF)
{
	return HDDs.Item((char *)DPF)->DeviceID[0] & 0x80;
}
/*
	Loop all the device controllers available to the PC in Native Mode
	1) Check for whether the device controller's IO Space for busy bit
	if yes then the controller is not installed.
	2) Issue EXECUTE DEVICE DIAGNOSTIC Command
	3) Wait up to 6 seconds for clearing busy bit
	4) Read the Error Register
	5) a) If bit 0 is set then Master  is installed
	b) If bit 7 is set then Slave   is not installed
	6) Set the appropriate bit in the DEV_HEAD register
	7) Delay for 50ns
	8) Issue Identify Device Command
	9) Receive 512 bytes of data from the device.

	*/
HardDiskHandler::HardDiskHandler()
{
	//Initialize();
}
void HardDiskHandler::Initialize()
{
	char strKey[3];
	strKey[0] = 'H'; //HDD type ID
	strKey[1] = '0';   //First HDD
	strKey[2] = 0;   //Null Character
	setvect(32+14,_HDDInterruptHandler);
	setvect(32+15,_HDDInterruptHandler);

	HDDs.Initialize();
	//for (BYTE DeviceController = 0; DeviceController < IDE_MAX_CONTROLLER; DeviceController++)
	for (int DeviceController = 0; DeviceController < 1; DeviceController++)
	{
		DoSoftwareReset(DeviceController);
		if (IsDeviceControllerBusy(DeviceController, 1000)) //if device controller is busy then skipping
		{			
			SkyConsole::Print("Controller Busy\n");
			continue;
		}
		OutPortByte(IDE_Con_IOBases[DeviceController][0] + IDE_CB_COMMAND, IDE_COM_EXECUTE_DEVICE_DIAGNOSTIC);
		if (IsDeviceControllerBusy(DeviceController, 1000))
		{			
			SkyConsole::Print("Controller busy after EXE\n");
			continue;
		}
		
		
		BYTE Result = InPortByte(IDE_Con_IOBases[DeviceController][0] + IDE_CB_ERROR);
		for (BYTE Device = 0; Device < 1; Device++)         // loop for master and slave disks
		{
			UINT16 DeviceID_Data[512], j;
			//if (Device == 0 && !(Result & 1))
				//continue;
			if (Device == 1 && (Result & 0x80))
				continue;
			if (Device == 1)
				OutPortByte(IDE_Con_IOBases[DeviceController][0] + IDE_CB_DEVICE_HEAD, 0x10); //Setting 4th bit(count 5) to set device as 1
			else
				OutPortByte(IDE_Con_IOBases[DeviceController][0] + IDE_CB_DEVICE_HEAD, 0x0);
			
			//msleep(50);
			
			OutPortByte(IDE_Con_IOBases[DeviceController][0] + IDE_CB_COMMAND, IDE_COM_IDENTIFY_DEVICE);
			if (!IsDeviceDataReady(DeviceController, 600, TRUE))
			{
				SkyConsole::Print("Data not ready %d\n", DeviceController);
				continue;
			}							

			/*Reading 512 bytes of information from the Device*/
			for (j = 0; j < 256; j++)
				DeviceID_Data[j] = InPortWord(IDE_Con_IOBases[DeviceController][0] + IDE_CB_DATA);
			/* Creating new HDD node for the Collection HDDs */
			
			//struct __HDDInfo * newHDD;
			__HDDInfo * newHDD=(__HDDInfo *)kmalloc(sizeof(__HDDInfo));
			if (newHDD == NULL)
			{
				SkyConsole::Print("HDD Initialize :: Allocation failed\n");
				return;
			}
			
			newHDD->IORegisterIdx = DeviceController;
			memcpy(newHDD->DeviceID, DeviceID_Data, 512);
			newHDD->DeviceNumber = Device;
			newHDD->LastError = 0;

			newHDD->BytesPerSector = 512; //-------------Modify Code

			newHDD->CHSCylinderCount = DeviceID_Data[1];
			newHDD->CHSHeadCount = DeviceID_Data[3];
			newHDD->CHSSectorCount = DeviceID_Data[6];

			if (DeviceID_Data[10] == 0)
				strcpy(newHDD->SerialNumber, "N/A");
			else
			for (j = 0; j < 20; j += 2)
			{
				newHDD->SerialNumber[j] = DeviceID_Data[10 + (j / 2)] >> 8;
				newHDD->SerialNumber[j + 1] = (DeviceID_Data[10 + (j / 2)] << 8) >> 8;
			}
			if (DeviceID_Data[23] == 0)
				strcpy(newHDD->FirmwareRevision, "N/A");
			else
			for (j = 0; j < 8; j += 2)
			{
				newHDD->FirmwareRevision[j] = DeviceID_Data[23 + (j / 2)] >> 8;
				newHDD->FirmwareRevision[j + 1] = (DeviceID_Data[23 + (j / 2)] << 8) >> 8;
			}
			
			if (DeviceID_Data[27] == 0)
				strcpy(newHDD->ModelNumber, "N/A");
			else
			for (j = 0; j < 20; j += 2)
			{
				newHDD->ModelNumber[j] = DeviceID_Data[27 + (j / 2)] >> 8;
				newHDD->ModelNumber[j + 1] = (DeviceID_Data[27 + (j / 2)] << 8) >> 8;
			}
			newHDD->LBASupported = DeviceID_Data[49] & 0x200;
			newHDD->DMASupported = DeviceID_Data[49] & 0x100;
			
			UINT32 LBASectors = DeviceID_Data[61];
			LBASectors = LBASectors << 16;
			LBASectors |= DeviceID_Data[60];
			SkyConsole::Print("DeviceId : %x, %s\n", Device, newHDD->ModelNumber);
			newHDD->LBACount = LBASectors;
			HDDs.Add(newHDD, strKey);
			strKey[1]++;

			
		}
	}
}
HardDiskHandler::~HardDiskHandler()
{
	HDDs.Clear();
}
__HDDInfo * HardDiskHandler::GetHDDInfo(BYTE * DPF)
{
		
	__HDDInfo * getHDD, * retHDD=(__HDDInfo *)kmalloc(sizeof(__HDDInfo));
	getHDD = HDDs.Item((char *)DPF);
	if (getHDD == NULL)
	{
		LastError = HDD_NOT_FOUND;
		return NULL;
	}
	memcpy(retHDD, getHDD, sizeof(__HDDInfo));
	return retHDD;
}
/* ----------------------PIO functions -------------------------------------
ReadSectors()
1) Get the HDDInfo object
2) Check whether Device is busy
3) Set the DEVICE BIT
4) Check whether the device ready accept Data commands
5) Set Head, Track etc informations
6) Issue READ command
7) Check whether the device ready to transfer data
8) Read the Data Register to get data
*/
BYTE HardDiskHandler::ReadSectors(BYTE * DPF, UINT16 StartCylinder, BYTE StartHead, BYTE StartSector, BYTE NoOfSectors, BYTE * Buffer, BOOLEAN WithRetry)
{
	__HDDInfo * HDD;
	BYTE DevHead, StartCylHigh = 0, StartCylLow = 0;

	HDD = HDDs.Item((char *)DPF);
	if (HDD == NULL)
	{
		LastError = HDD_NOT_FOUND;
		return HDD_NOT_FOUND;
	}

	if (HDD->DeviceNumber == 0)
		DevHead = StartHead | 0xA0;
	else
		DevHead = StartHead | 0xB0;

	if (IsDeviceControllerBusy(HDD->IORegisterIdx, 1 * 60))
	{
		LastError = HDD_CONTROLLER_BUSY;
		return HDD_CONTROLLER_BUSY;
	}

	OutPortByte(IDE_Con_IOBases[HDD->IORegisterIdx][0] + IDE_CB_DEVICE_HEAD, DevHead);

	if (!IsDeviceDataReady(HDD->IORegisterIdx, 1 * 60, FALSE))
	{
		LastError = HDD_DATA_COMMAND_NOT_READY;
		return HDD_DATA_COMMAND_NOT_READY;
	}

	StartCylHigh = StartCylinder >> 8;
	StartCylLow = (StartCylinder << 8) >> 8;

	OutPortByte(IDE_Con_IOBases[HDD->IORegisterIdx][0] + IDE_CB_CYLINDER_HIGH, StartCylHigh);
	OutPortByte(IDE_Con_IOBases[HDD->IORegisterIdx][0] + IDE_CB_CYLINDER_LOW, StartCylLow);
	OutPortByte(IDE_Con_IOBases[HDD->IORegisterIdx][0] + IDE_CB_SECTOR, StartSector);
	OutPortByte(IDE_Con_IOBases[HDD->IORegisterIdx][0] + IDE_CB_SECTOR_COUNT, NoOfSectors);

	OutPortByte(IDE_Con_IOBases[HDD->IORegisterIdx][0] + IDE_CB_COMMAND, WithRetry ? IDE_COM_READ_SECTORS_W_RETRY : IDE_COM_READ_SECTORS);
	for (BYTE j = 0; j < NoOfSectors; j++)
	{
		if (!IsDeviceDataReady(HDD->IORegisterIdx, 1 * 60, TRUE))
		{
			LastError = HDD_DATA_NOT_READY;
			return HDD_DATA_NOT_READY;
		}

		for (UINT16 i = 0; i < (HDD->BytesPerSector) / 2; i++)
		{
			UINT16 w = 0;
			BYTE l, h;
			w = InPortWord(IDE_Con_IOBases[HDD->IORegisterIdx][0] + IDE_CB_DATA);
			l = (w << 8) >> 8;
			h = w >> 8;
			Buffer[(j * (HDD->BytesPerSector)) + (i * 2)] = l;
			Buffer[(j * (HDD->BytesPerSector)) + (i * 2) + 1] = h;
		}
	}
	return HDD_NO_ERROR;
}

/* ----------------------PIO functions -------------------------------------
ReadSectors()
1) Get the HDDInfo object
2) Check whether Device is busy
3) Set the DEVICE BIT
4) Check whether the device ready accept Data commands
5) Set Head, Track etc informations
6) Issue READ command
7) Check whether the device ready to transfer data
8) Read the Data Register to get data
*/
BYTE HardDiskHandler::ReadSectors(BYTE * DPF, UINT32 StartLBASector, BYTE NoOfSectors, BYTE * Buffer, BOOLEAN WithRetry)
{
	__HDDInfo * HDD;
	BYTE LBA0_7, LBA8_15, LBA16_23, LBA24_27;

	HDD = HDDs.Item((char *)DPF);
	if (HDD == NULL)
	{
		LastError = HDD_NOT_FOUND;
		return HDD_NOT_FOUND;
	}
	LBA0_7 = (StartLBASector << 24) >> 24;
	LBA8_15 = (StartLBASector << 16) >> 24;
	LBA16_23 = (StartLBASector << 8) >> 24;
	LBA24_27 = (StartLBASector << 4) >> 28;

	if (HDD->DeviceNumber == 0)
		LBA24_27 = LBA24_27 | 0xE0;
	else
		LBA24_27 = LBA24_27 | 0xF0;

	if (IsDeviceControllerBusy(HDD->IORegisterIdx, 1 * 60))
	{
		LastError = HDD_CONTROLLER_BUSY;
		return HDD_CONTROLLER_BUSY;
	}

	OutPortByte(IDE_Con_IOBases[HDD->IORegisterIdx][0] + IDE_CB_DEVICE_HEAD, LBA24_27);

	if (!IsDeviceDataReady(HDD->IORegisterIdx, 1 * 60, FALSE))
	{
		LastError = HDD_DATA_COMMAND_NOT_READY;
		return HDD_DATA_COMMAND_NOT_READY;
	}

	OutPortByte(IDE_Con_IOBases[HDD->IORegisterIdx][0] + IDE_CB_LBA_16_23, LBA16_23);
	OutPortByte(IDE_Con_IOBases[HDD->IORegisterIdx][0] + IDE_CB_LBA_8_15, LBA8_15);
	OutPortByte(IDE_Con_IOBases[HDD->IORegisterIdx][0] + IDE_CB_LBA_0_7, LBA0_7);
	OutPortByte(IDE_Con_IOBases[HDD->IORegisterIdx][0] + IDE_CB_SECTOR_COUNT, NoOfSectors);

	OutPortByte(IDE_Con_IOBases[HDD->IORegisterIdx][0] + IDE_CB_COMMAND, WithRetry ? IDE_COM_READ_SECTORS_W_RETRY : IDE_COM_READ_SECTORS);
	for (BYTE j = 0; j < NoOfSectors; j++)
	{
		if (!IsDeviceDataReady(HDD->IORegisterIdx, 1 * 60, TRUE))
		{
			LastError = HDD_DATA_NOT_READY;
			return HDD_DATA_NOT_READY;
		}

		for (UINT16 i = 0; i < (HDD->BytesPerSector) / 2; i++)
		{
			UINT16 w = 0;
			BYTE l, h;
			w = InPortWord(IDE_Con_IOBases[HDD->IORegisterIdx][0] + IDE_CB_DATA);
			l = (w << 8) >> 8;
			h = w >> 8;
			Buffer[(j * (HDD->BytesPerSector)) + (i * 2)] = l;
			Buffer[(j * (HDD->BytesPerSector)) + (i * 2) + 1] = h;
		}
	}
	return HDD_NO_ERROR;
}


/*WriteSectors()
1) Get the HDDInfo object
2) Check whether Device is busy
3) Set the DEVICE BIT
4) Check whether the device ready accept Data commands
5) Set Head, Track etc informations
6) Issue WRITE command
7) Check whether the device ready to read data
8) Write to the Data Register to send data
*/
BYTE HardDiskHandler::WriteSectors(BYTE * DPF, UINT16 StartCylinder, BYTE StartHead, BYTE dwStartLBASector, BYTE NoOfSectors, BYTE * lpBuffer, BOOLEAN WithRetry)
{
	__HDDInfo * HDD;
	BYTE LBA0_7, LBA8_15, LBA16_23, LBA24_27;

	HDD = HDDs.Item((char *)DPF);
	if (HDD == NULL)
	{
		LastError = HDD_NOT_FOUND;
		return HDD_NOT_FOUND;
	}
	
	LBA0_7 = (dwStartLBASector << 24) >> 24;
	LBA8_15 = (dwStartLBASector << 16) >> 24;
	LBA16_23 = (dwStartLBASector << 8) >> 24;
	LBA24_27 = (dwStartLBASector << 4) >> 28;

	if (HDD->DeviceNumber == 0)
		LBA24_27 = LBA24_27 | 0xE0;
	else
		LBA24_27 = LBA24_27 | 0xF0;

	if (IsDeviceControllerBusy(HDD->IORegisterIdx, 400))
	{
		//  DEBUG_PRINT_OBJECT1("Controller busy - [%d]",bDeviceNo );
		SetLastError(ERROR_BUSY);
		return HDD_CONTROLLER_BUSY;
	}

	OutPortByte(IDE_Con_IOBases[HDD->IORegisterIdx][0] + IDE_CB_DEVICE_HEAD, LBA24_27);

	if (!IsDeviceDataReady(HDD->IORegisterIdx, 1000, FALSE))
	{		
		SetLastError(ERROR_NOT_READY);
		return HDD_DATA_COMMAND_NOT_READY;
	}
	OutPortByte(IDE_Con_IOBases[HDD->IORegisterIdx][0] + IDE_CB_LBA_16_23, LBA16_23);
	OutPortByte(IDE_Con_IOBases[HDD->IORegisterIdx][0] + IDE_CB_LBA_8_15, LBA8_15);
	OutPortByte(IDE_Con_IOBases[HDD->IORegisterIdx][0] + IDE_CB_LBA_0_7, LBA0_7);
	OutPortByte(IDE_Con_IOBases[HDD->IORegisterIdx][0] + IDE_CB_SECTOR_COUNT, NoOfSectors);

	OutPortByte(IDE_Con_IOBases[HDD->IORegisterIdx][0] + IDE_CB_COMMAND, IDE_COM_WRITE_SECTORS_W_RETRY);
	for (UINT16 j = 0; j < NoOfSectors; j++)
	{
		if (!IsDeviceDataReady(HDD->IORegisterIdx, 1000, TRUE))
		{			
			SetLastError(ERROR_NOT_READY);
			return HDD_DATA_NOT_READY;
		}
		for (UINT16 i = 0; i<HDD->BytesPerSector / 2; i++)
		{
			OutPortWord(IDE_Con_IOBases[HDD->IORegisterIdx][0] + IDE_CB_DATA, ((UINT16 *)lpBuffer)[(j * (HDD->BytesPerSector) / 2) + i]);
		}
	}

	return HDD_NO_ERROR;
}

//----------------------GSH Functions implementation ----------------
BYTE HardDiskHandler::GetNoOfDevices()
{
	return GetTotalDevices();
}
/*--------------------------------------------------------
 This function returns the specified devices parameters in the buffer
 Note :-  1) You must allocate memory for the Buffer before calling this function.
 2) Allocated size must be atleast of the sizeof(_GSH_IO_Parameter)
 Input  :
 DPF - Path of the device , Buffer
 Output :
 Returns HDD_NO_ERROR on success
 */

UINT16 HardDiskHandler::GetDeviceParameters(BYTE * DPF, BYTE * Buffer)
{
	struct _GSH_IO_Parameter DeviceInfo;

	__HDDInfo * getHDD;
	getHDD = this->HDDs.Item((char *)DPF);
	if (getHDD == NULL)
	{
		this->LastError = HDD_NOT_FOUND;
		return HDD_NOT_FOUND;
	}

	DeviceInfo.Cylinder = getHDD->CHSCylinderCount;
	DeviceInfo.Head = getHDD->CHSHeadCount;
	DeviceInfo.Sector = getHDD->CHSSectorCount;
	DeviceInfo.LBASector = getHDD->LBACount;
	memcpy(Buffer, &DeviceInfo, sizeof(DeviceInfo));

	return HDD_NO_ERROR;
}
/* This function reset the given controller and returns the Diagnostic Code
for detailed information see DoSoftwareReset().
Output :
on failure returns 0
on success returns the diagnostic code returned by DoSoftwareReset()
*/
BYTE HardDiskHandler::Reset(BYTE * DPF)
{
	//struct _GSH_IO_Parameter DeviceInfo;

	__HDDInfo * getHDD;
	getHDD = this->HDDs.Item((char *)DPF);
	if (getHDD == NULL)
	{
		this->LastError = HDD_NOT_FOUND;
		return 0;
	}
	return this->DoSoftwareReset(getHDD->IORegisterIdx);

}
UINT32 HardDiskHandler::CHSToLBA(BYTE * DPF, UINT32 Cylinder, UINT32 Head, UINT32 Sector)
{
	__HDDInfo * getHDD;
	getHDD = this->HDDs.Item((char *)DPF);

	return (Sector - 1) + (Head*getHDD->CHSSectorCount) + (Cylinder * (getHDD->CHSHeadCount + 1) * getHDD->CHSSectorCount);
}
void HardDiskHandler::LBAToCHS(BYTE * DPF, UINT32 LBA, UINT32 * Cylinder, UINT32 * Head, UINT32 * Sector)
{
	__HDDInfo * getHDD;
	getHDD = this->HDDs.Item((char *)DPF);

	*Sector = ((LBA % getHDD->CHSSectorCount) + 1);
	UINT32 CylHead = (LBA / getHDD->CHSSectorCount);
	*Head = (CylHead % (getHDD->CHSHeadCount + 1));
	*Cylinder = (CylHead / (getHDD->CHSHeadCount + 1));
}
