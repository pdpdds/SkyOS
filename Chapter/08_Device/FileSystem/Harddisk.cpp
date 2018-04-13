/*
        Ace OS - Source Code
                Developer - samueldotj@gmail.com
*/


//#define _DEBUG_

#include "HardDisk.h"
#include "string.h"
#include "memory.h"
#include "Hal.h"
#include "PIT.h"
#include "SysInfo.h"
#include "SkyConsole.h"

/*---IDE Controller's Compability mode Base Registers and Interrupts ---*/
const BYTE IDE_MAX_CONTROLLER=4;
const BYTE IDE_MAX_DEVICE_PER_CONTROLLER=2;
const BYTE IDE_MAX_DISK_DRIVES=IDE_MAX_CONTROLLER * IDE_MAX_DEVICE_PER_CONTROLLER;

const UINT16 IDE_Con_IOBases[IDE_MAX_CONTROLLER][2]=
				{
					{ 0x1F0, 0x3F6 },
					{ 0x170, 0x376 },
					{ 0x1E8, 0x3EE },
					{ 0x168, 0x36E }
                };
const UINT16 IDE_Con_IRQs[IDE_MAX_CONTROLLER][3] =
				{
					{14, 0,  0},
					{15, 0,  0},
					{11, 12, 9},
					{10, 12, 9}
                };


/* ---------- Command Block Registers Offset From Base ------------------*/
const BYTE IDE_CB_DATA=0;       /* Read & Write */
const BYTE IDE_CB_ERROR=1;	/* Read only    */
const BYTE IDE_CB_FEATURES=1;	/* Write only   */
const BYTE IDE_CB_STATUS=7;     /* Read only    */
const BYTE IDE_CB_COMMAND=7;    /* Write only   */


const BYTE IDE_CB_SECTOR_COUNT=2;

const BYTE IDE_CB_SECTOR=3;
const BYTE IDE_CB_CYLINDER_LOW=4;
const BYTE IDE_CB_CYLINDER_HIGH=5;
const BYTE IDE_CB_DEVICE_HEAD=6;

const BYTE IDE_CB_LBA_0_7=3;
const BYTE IDE_CB_LBA_8_15=4;
const BYTE IDE_CB_LBA_16_23=5;
const BYTE IDE_CB_LBA_24_27=6;

/* ---------- Control Block Registers Constants ------------------*/
BYTE IDE_CON_BSY=7;
BYTE IDE_CON_DRDY=6;
BYTE IDE_CON_DF=5;
BYTE IDE_CON_DSC=4;
BYTE IDE_CON_DRQ=3;
BYTE IDE_CON_CORR=2;
BYTE IDE_CON_IDX=1;
BYTE IDE_CON_ERR=0;


/* ---------- Control Block Registers Offset From Base ------------*/
const BYTE IDE_CON_ALTERNATE_STATUS=2;
const BYTE IDE_CON_DEVICE_CONTROL=2;

/*-------------     ATA - 2  Command's OpCodes  -------------------*/
const BYTE IDE_COM_ACKNOWLEDGE_MEDIA_CHANGE=0xDB;
const BYTE IDE_COM_POST_BOOT=0xDC;
const BYTE IDE_COM_PRE_BOOT=0xDD;

/*const BYTE IDE_COM_CHECK_POWER_MODE=98hE5h */

const BYTE IDE_COM_DOOR_LOCK=0xDE;
const BYTE IDE_COM_DOOR_UNLOCK=0xDF;
const BYTE IDE_COM_DOWNLOAD_MICROCODE=0x92;
const BYTE IDE_COM_EXECUTE_DEVICE_DIAGNOSTIC=0x90;
const BYTE IDE_COM_FORMAT_TRACK=0x50;
const BYTE IDE_COM_IDENTIFY_DEVICE=0xEC;

/*const BYTE IDE_COM_IDLE=97hE3h
const BYTE IDE_COM_IDLE_IMMEDIATE=0x95hE1h */

const BYTE IDE_COM_INITIALIZE_DEVICE_PARAMETERS=0x91;
const BYTE IDE_COM_MEDIA_EJECT=0xED;
const BYTE IDE_COM_NOP=0x0;
const BYTE IDE_COM_READ_BUFFER=0xE4;
const BYTE IDE_COM_READ_DMA_W_RETRY=0xC8;
const BYTE IDE_COM_READ_DMA=0xC9;
const BYTE IDE_COM_READ_LONG_W_RETRY=0x22;
const BYTE IDE_COM_READ_LONG=0x23;
const BYTE IDE_COM_READ_MULTIPLE=0xC4;
const BYTE IDE_COM_READ_SECTORS_W_RETRY=0x20;
const BYTE IDE_COM_READ_SECTORS=0x21;
const BYTE IDE_COM_READ_VERIFY_SECTORS_W_RETRY=0x40;
const BYTE IDE_COM_READ_VERIFY_SECTORS=0x41;
const BYTE IDE_COM_RECALIBRATE=0x10;
const BYTE IDE_COM_SEEK=0x70;
const BYTE IDE_COM_SET_FEATURES=0xEF;
const BYTE IDE_COM_SET_MULTIPLE_MODE=0xC6;

/*const BYTE IDE_COM_SLEEP=99hE6h
const BYTE IDE_COM_STANDBY=96hE2h
const BYTE IDE_COM_STANDBY_IMMEDIATE=94hE0h */

const BYTE IDE_COM_WRITE_BUFFER=0xE8;
const BYTE IDE_COM_WRITE_DMA_W_RETRY=0xCA;
const BYTE IDE_COM_WRITE_DMA=0xCB;
const BYTE IDE_COM_WRITE_LONG_W_RETRY=0x32;
const BYTE IDE_COM_WRITE_LONG=0x33;
const BYTE IDE_COM_WRITE_MULTIPLE=0xC5;
const BYTE IDE_COM_WRITE_SAME=0xE9;
const BYTE IDE_COM_WRITE_SECTORS_W_RETRY=0x30;
const BYTE IDE_COM_WRITE_SECTORS=0x31;
const BYTE IDE_COM_WRITE_VERIFY=0x3C;


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

//this list contains all the IDEs found during the detection
LPHDD sysHDDArray[IDE_MAX_DISK_DRIVES];
BYTE bTotalDevices=0;

#define GET_LPHDD_FOR_DEVICE( bDeviceNo, FailureValue )\
    if ( bDeviceNo > bTotalDevices )\
    {\
 SetLastError( ERROR_INVALID_PARAMETER );\
        return FailureValue;\
    }\
    if ( sysHDDArray[bDeviceNo] == NULL )\
    {\
 SetLastError( ERROR_DEV_NOT_EXIST );\
        return FailureValue;\
    }\
    lpHDD = sysHDDArray[bDeviceNo];

/*#define GET_LPHDD_FOR_DEVICE( bDeviceNo, FailureValue )\
    if ( bDeviceNo > bTotalDevices )\
    {\
        DEBUG_PRINT_OBJECT2("bDeviceNo(%d) > bTotalDevices(%d)",bDeviceNo, bTotalDevices );\
        SetLastError( ERROR_INVALID_PARAMETER );\
        return FailureValue;\
    }\
    if ( sysHDDArray[bDeviceNo] == NULL )\
    {\
        DEBUG_PRINT_OBJECT1("sysHDDArray[bDeviceNo] == NULL  %X",sysHDDArray );\
        SetLastError( ERROR_DEV_NOT_EXIST );\
        return FailureValue;\
    }\
    lpHDD = sysHDDArray[bDeviceNo];*/

/* This function returns the Error Register value*/
BYTE ReadErrorRegister(BYTE bDeviceController)
{
    BYTE bStatus=InPortByte(IDE_Con_IOBases[bDeviceController][0]+IDE_CB_STATUS);
    if ( (bStatus & 0x80) == 0 && (bStatus & 0x1) ) //busy bit=0 and err bit=1
    {
        bStatus=InPortByte(IDE_Con_IOBases[bDeviceController][0]+IDE_CB_ERROR);
        return bStatus;
    }
    else
        return 0;
}
/*
   -----------------------------------------------------------
This function checks whether the given device ready for data transfer or receive
	Input :
		bDeviceController - Index number of Device to test  ( 0 to IDE_MAX_CONTROLLER )
		Wait UpTo ms - Optional Time in milli second to check for the busy status
	Output :
		Returns 'TRUE' if the device is ready else 'FALSE'
   -----------------------------------------------------------
*/
BOOLEAN IsDeviceDataReady(BYTE bDeviceController, DWORD dwWaitUpToms=0, BOOLEAN bCheckDataRequest=TRUE)
{
    UINT32 dwTime1,dwTime2;
    BYTE bStatus;
    dwTime1= GetTickCount();
    //printf(" DeviceDataReady " );
    do
    {
        bStatus=InPortByte( IDE_Con_IOBases[bDeviceController][0]+IDE_CB_STATUS );
        if ( (bStatus & 0x80) == 0 ) //Checking BSY bit, because DRDY bit is valid only when BSY is zero
        {
            if ( bStatus & 0x40 ) //checking DRDY is set
            {
                if ( bCheckDataRequest ) // if DataRequest is also needed
                {
                    if ( bStatus & 0x8 ) // DRQ bit set
                        return TRUE;
                }
                else
                    return TRUE;
            }
        }
        dwTime2= GetTickCount();
    }while ( ( dwTime2-dwTime1 ) < dwWaitUpToms );  /* Wait for data ready */
    //BYTE bError=InPortByte( IDE_Con_IOBases[bDeviceController][0]+IDE_CB_ERROR );
    //printf("\n\r [%X] [%X] ", bStatus, bError );
    return FALSE;
}
/*
  ---------------------------------------------------------
This function checks whether the given device controlller is busy or not.
	Input :
		bDeviceController - Device Number ( 0 to IDE_MAX_CONTROLLER )
		Wait UpTo ms - Optional Time in milli second to check for the busy status
	Output :
		Returns 'TRUE' if the device is busy else 'FALSE'
   -----------------------------------------------------------
*/
BOOLEAN IsDeviceControllerBusy(BYTE bDeviceController, DWORD dwWaitUpToms=0)
{
    UINT32 dwTime1,dwTime2;
    BYTE bStatus;
    
    dwTime1= GetTickCount();
    //printf("\n\rDeviceController Status ");
    do	
    {
        bStatus=InPortByte( IDE_Con_IOBases[bDeviceController][0]+IDE_CB_STATUS );
        if ( ( bStatus & 0x80 ) == 0 ) //BSY bit 
            return FALSE;
        dwTime2= GetTickCount();
    }while( (  dwTime2 - dwTime1 ) <= dwWaitUpToms );
    //printf(" [%X] ", bStatus );
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
		bDeviceController Number
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
BYTE HardDisk_DoSoftwareReset(BYTE bDeviceController)
{
    BYTE bDeviceControl=4; //Setting SRST bit in the control register ( 2nd bit  (bit count 3) )
    OutPortByte(IDE_Con_IOBases[bDeviceController][0]+IDE_CON_DEVICE_CONTROL,bDeviceControl);
    bDeviceControl=0;      //Clearing the SRST bit in the control register ( 2nd bit)
    OutPortByte(IDE_Con_IOBases[bDeviceController][0]+IDE_CON_DEVICE_CONTROL,bDeviceControl);
		
    return InPortByte(IDE_Con_IOBases[bDeviceController][0]+IDE_CB_ERROR);
}
BOOLEAN HardDisk_IsRemovableDevice(BYTE bDeviceNo)
{
    LPHDD lpHDD;
    GET_LPHDD_FOR_DEVICE( bDeviceNo, FALSE );
    return !(lpHDD->bDeviceID[0] & 0x70 );
}
BOOLEAN HardDisk_IsRemovableMedia(BYTE bDeviceNo)
{
    LPHDD lpHDD;
    GET_LPHDD_FOR_DEVICE( bDeviceNo, FALSE );
    return lpHDD->bDeviceID[0] & 0x80;
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
UINT32 HardDisk_CHSToLBA(BYTE bDeviceNo, UINT32 dwCylinder, UINT32 dwHead, UINT32 dwSector)
{
    LPHDD lpHDD;
    GET_LPHDD_FOR_DEVICE( bDeviceNo, 0);
    
    return (dwSector-1)+(dwHead*lpHDD->wCHSSectorCount) + (dwCylinder * (lpHDD->wCHSHeadCount + 1) * lpHDD->wCHSSectorCount);
}
UINT32 HardDisk_LBAToCHS(BYTE bDeviceNo,UINT32 dwLBA, UINT32 * lpCylinder, UINT32 * lpHead, UINT32 * lpSector)
{
    LPHDD lpHDD;
    GET_LPHDD_FOR_DEVICE( bDeviceNo, 0);
    
    *lpSector = (dwLBA % lpHDD->wCHSSectorCount) +1;
    UINT32 dwCylHead = (dwLBA / lpHDD->wCHSSectorCount);
    *lpHead = (dwCylHead % ( lpHDD->wCHSHeadCount + 1));
    *lpCylinder = (dwCylHead / ( lpHDD->wCHSHeadCount + 1));
    
    return 1;
}

void HardDisk_Initialize()
{
    bTotalDevices = 0;
    //SetIDT(32+14,_HDDInterruptHandler);
    //SetIDT(32+15,_HDDInterruptHandler);
	setvect(32 + 14, _HDDInterruptHandler);
	setvect(32 + 15, _HDDInterruptHandler);

    
    for(int i=0;i<IDE_MAX_DISK_DRIVES;i++)
        sysHDDArray[i]=NULL;

    //for(BYTE bDeviceController=0;bDeviceController<IDE_MAX_CONTROLLER;bDeviceController++)
	for (BYTE bDeviceController = 0; bDeviceController<1; bDeviceController++)
    {
        HardDisk_DoSoftwareReset(bDeviceController);
        if ( IsDeviceControllerBusy(bDeviceController) ) //if device controller is busy then skipping
        {         
			SkyConsole::Print("Controller Busy %d\n", bDeviceController);
            continue;
        }
        OutPortByte(IDE_Con_IOBases[bDeviceController][0]+IDE_CB_COMMAND, IDE_COM_EXECUTE_DEVICE_DIAGNOSTIC);
        if ( IsDeviceControllerBusy(bDeviceController,400) )
        {            
			SkyConsole::Print("Controller Busy %d\n", bDeviceController);
            continue;
        }
		
        BYTE bResult=InPortByte(IDE_Con_IOBases[bDeviceController][0]+IDE_CB_ERROR);
        //for (BYTE bDevice=0;bDevice<IDE_MAX_DEVICE_PER_CONTROLLER;bDevice++)         // loop for master and slave disks
		for (BYTE bDevice = 0; bDevice<1; bDevice++)         // loop for master and slave disks
        {
            UINT16 DeviceID_Data[512],j;
            if ( bDevice==0 && ! ( bResult & 1 ) )
            {
                //DEBUG_PRINT_OBJECT1("Master device - Result != 1 (%d)",bResult);
                continue;
            }
            if ( bDevice==1 && ( bResult & 0x80 ) )
            {
                //DEBUG_PRINT_OBJECT1("Slave device - Result %d",bResult);
                continue;
            }
            if ( bDevice==1 )
                OutPortByte(IDE_Con_IOBases[bDeviceController][0]+IDE_CB_DEVICE_HEAD, 0x10 ); //Setting 4th bit(count 5) to set device as 1
            else
                OutPortByte(IDE_Con_IOBases[bDeviceController][0]+IDE_CB_DEVICE_HEAD, 0x0 );
			//MDelay(50);			
			msleep(5);
            OutPortByte(IDE_Con_IOBases[bDeviceController][0]+IDE_CB_COMMAND, IDE_COM_IDENTIFY_DEVICE);
            if ( !IsDeviceDataReady(bDeviceController,600, TRUE) )
            {
                //DEBUG_PRINT_OBJECT1("Data not ready %d", bDeviceController);
                continue;
            }
            //Reading 512 bytes of information from the Device
            for(j=0;j<256;j++)
                DeviceID_Data[j]=InPortWord(IDE_Con_IOBases[bDeviceController][0]+IDE_CB_DATA);
            // Creating new HDD node for the Collection HDDs 
            LPHDD lpNewHDD = new HDD;          
            
            lpNewHDD->bIORegisterIdx=bDeviceController;
            memcpy(lpNewHDD->bDeviceID, DeviceID_Data, 512);
            lpNewHDD->bDeviceNumber=bDevice;

            lpNewHDD->wBytesPerSector=512; //-------------Modify Code
            lpNewHDD->wCHSCylinderCount=DeviceID_Data[1];
            lpNewHDD->wCHSHeadCount=DeviceID_Data[3];
            lpNewHDD->wCHSSectorCount=DeviceID_Data[6];
            if ( DeviceID_Data[10] == 0 )
                strcpy(lpNewHDD->szSerialNumber,"                    ");
            else
                for (j=0;j<19;j+=2 )
                {
                    lpNewHDD->szSerialNumber[j] = DeviceID_Data[10+(j/2)]>>8;
                    lpNewHDD->szSerialNumber[j+1] = (DeviceID_Data[10+(j/2)]<<8)>>8;
                }
            if ( DeviceID_Data[23] == 0 )
                strcpy(lpNewHDD->szFirmwareRevision,"        ");
            else
                for (j=0;j<8;j+=2 )
                {
                    lpNewHDD->szFirmwareRevision[j]=DeviceID_Data[23+(j/2)]>>8;
                    lpNewHDD->szFirmwareRevision[j+1]=(DeviceID_Data[23+(j/2)]<<8)>>8;
                }
            if ( DeviceID_Data[27] == 0 )
                strcpy(lpNewHDD->szModelNumber,"                    ");
            else
                for (j=0;j<19;j+=2 )
                {
                    lpNewHDD->szModelNumber[j]=DeviceID_Data[27+(j/2)]>>8;
                    lpNewHDD->szModelNumber[j+1]=(DeviceID_Data[27+(j/2)]<<8)>>8;
                }
            
            lpNewHDD->dwFeatures=0;     //set no feature
            if ( DeviceID_Data[49]&0x200 )
                lpNewHDD->dwFeatures|=1;        //turn on-off LBA feature
            if ( DeviceID_Data[49]&0x100 )
                lpNewHDD->dwFeatures|=2;        //turn on-off DMA feature

            UINT32 dwLBASectors=DeviceID_Data[61];
            dwLBASectors=dwLBASectors<<16;
            dwLBASectors|=DeviceID_Data[60];
            lpNewHDD->dwLBACount=dwLBASectors;
            
            sysHDDArray[bTotalDevices] = lpNewHDD;
            bTotalDevices++;
        }
    }

    if ( bTotalDevices > 0 ) //one or more HDD is found and we must register into the GSH
    {
		//20180413
		//remember this structure should not be freed
        /*LPGSH lpNewGSH = new GSH;
               
        lpNewGSH->GetNoOfDevices=HDD_GetNoOfDevices;
        lpNewGSH->Read=HDD_Read;
        lpNewGSH->Write=HDD_Write;
        
        lpNewGSH->GetDeviceParameters=NULL;
        lpNewGSH->Verify=NULL;
        lpNewGSH->Reset=NULL;
        lpNewGSH->Status=NULL;
        
		if (GSH_Register("Harddisk", lpNewGSH) == FALSE)
		{
			
		//	DEBUG_PRINT_INFO("GSH Registration failed.");
		}*/
    }
}
/* This function reset the given controller and returns the Diagnostic Code
for detailed information see DoSoftwareReset().
	Output :
		on failure returns 0
		on success returns the diagnostic code returned by DoSoftwareReset()
*/
UINT32 HardDisk_Reset(BYTE bDeviceNo)
{
    LPHDD lpHDD;
    GET_LPHDD_FOR_DEVICE( bDeviceNo, 0 );
    return HardDisk_DoSoftwareReset(lpHDD->bIORegisterIdx);
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
UINT32 HardDisk_ReadSectors(BYTE bDeviceNo, UINT32 dwStartLBASector, BYTE bNoOfSectors, BYTE * lpBuffer)
{
    BYTE LBA0_7, LBA8_15, LBA16_23, LBA24_27;
    
   // DEBUG_PRINT_OBJECT3("Device %d Sector %ld Total %d", bDeviceNo, dwStartLBASector, bNoOfSectors );
   // DEBUG_PRINT_OBJECT1("Buffer %X", lpBuffer );
    
    LPHDD lpHDD;
    GET_LPHDD_FOR_DEVICE( bDeviceNo, 0);
    
    LBA0_7 =   ( dwStartLBASector<<24 )>>24;
    LBA8_15 =  ( dwStartLBASector<<16 )>>24;
    LBA16_23 = ( dwStartLBASector<<8  )>>24;
    LBA24_27 = ( dwStartLBASector<<4  )>>28;

    if ( lpHDD->bDeviceNumber==0 )
        LBA24_27 = LBA24_27|0xE0;
    else
        LBA24_27 = LBA24_27|0xF0;

    if ( IsDeviceControllerBusy(lpHDD->bIORegisterIdx,400) )
    {
    //    DEBUG_PRINT_OBJECT1("Controller busy - [%d]",bDeviceNo );
     SetLastError(ERROR_BUSY);
        return HDD_CONTROLLER_BUSY;
    }
    
    OutPortByte(IDE_Con_IOBases[lpHDD->bIORegisterIdx][0]+IDE_CB_DEVICE_HEAD, LBA24_27 );
    
    if ( !IsDeviceDataReady(lpHDD->bIORegisterIdx, 1000, FALSE) )
    {
      //  DEBUG_PRINT_OBJECT1("Data Command not ready - [%d]",bDeviceNo );
      SetLastError(ERROR_NOT_READY);
	    return HDD_DATA_COMMAND_NOT_READY;
    }
    OutPortByte(IDE_Con_IOBases[lpHDD->bIORegisterIdx][0]+IDE_CB_LBA_16_23, LBA16_23 );
    OutPortByte(IDE_Con_IOBases[lpHDD->bIORegisterIdx][0]+IDE_CB_LBA_8_15,  LBA8_15 );
    OutPortByte(IDE_Con_IOBases[lpHDD->bIORegisterIdx][0]+IDE_CB_LBA_0_7,   LBA0_7 );
    OutPortByte(IDE_Con_IOBases[lpHDD->bIORegisterIdx][0]+IDE_CB_SECTOR_COUNT, bNoOfSectors );

    OutPortByte(IDE_Con_IOBases[lpHDD->bIORegisterIdx][0]+IDE_CB_COMMAND,IDE_COM_READ_SECTORS_W_RETRY);
    for (UINT16 j=0; j < bNoOfSectors; j++)
    {
        if ( !IsDeviceDataReady(lpHDD->bIORegisterIdx,1000,TRUE))
        {
           // DEBUG_PRINT_INFO("Data not ready");
          SetLastError(ERROR_NOT_READY);
            return HDD_DATA_NOT_READY;
        }
        for(UINT16 i=0; i<lpHDD->wBytesPerSector/2; i++)
        {
            ((UINT16 *)lpBuffer)[(j * (lpHDD->wBytesPerSector)/2 )+i]=InPortWord(IDE_Con_IOBases[lpHDD->bIORegisterIdx][0]+IDE_CB_DATA ) ;
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
UINT32 HardDisk_WriteSectors(BYTE bDeviceNo, UINT32 dwStartLBASector, BYTE bNoOfSectors, BYTE * lpBuffer)
{
    BYTE LBA0_7, LBA8_15, LBA16_23, LBA24_27;
    
   // DEBUG_PRINT_OBJECT3("Device %d Sector %ld Total %d", bDeviceNo, dwStartLBASector, bNoOfSectors );
   // DEBUG_PRINT_OBJECT1("Buffer %X", lpBuffer );
    
    LPHDD lpHDD;
    GET_LPHDD_FOR_DEVICE( bDeviceNo, 0);
    		
    LBA0_7 =   ( dwStartLBASector<<24 )>>24;
    LBA8_15 =  ( dwStartLBASector<<16 )>>24;
    LBA16_23 = ( dwStartLBASector<<8  )>>24;
    LBA24_27 = ( dwStartLBASector<<4  )>>28;

    if ( lpHDD->bDeviceNumber==0 )
        LBA24_27=LBA24_27|0xE0;
    else
        LBA24_27=LBA24_27|0xF0;

    if ( IsDeviceControllerBusy(lpHDD->bIORegisterIdx,400) )
    {
      //  DEBUG_PRINT_OBJECT1("Controller busy - [%d]",bDeviceNo );
      SetLastError(ERROR_BUSY);
        return HDD_CONTROLLER_BUSY;
    }
    
    OutPortByte(IDE_Con_IOBases[lpHDD->bIORegisterIdx][0]+IDE_CB_DEVICE_HEAD, LBA24_27 );
    
    if ( !IsDeviceDataReady(lpHDD->bIORegisterIdx, 1000, FALSE) )
    {
     //   DEBUG_PRINT_OBJECT1("Data Command not ready - [%d]",bDeviceNo );
    SetLastError(ERROR_NOT_READY);
	return HDD_DATA_COMMAND_NOT_READY;
    }
    OutPortByte(IDE_Con_IOBases[lpHDD->bIORegisterIdx][0]+IDE_CB_LBA_16_23, LBA16_23 );
    OutPortByte(IDE_Con_IOBases[lpHDD->bIORegisterIdx][0]+IDE_CB_LBA_8_15,  LBA8_15 );
    OutPortByte(IDE_Con_IOBases[lpHDD->bIORegisterIdx][0]+IDE_CB_LBA_0_7,   LBA0_7 );
    OutPortByte(IDE_Con_IOBases[lpHDD->bIORegisterIdx][0]+IDE_CB_SECTOR_COUNT, bNoOfSectors );

    OutPortByte(IDE_Con_IOBases[lpHDD->bIORegisterIdx][0]+IDE_CB_COMMAND,IDE_COM_WRITE_SECTORS_W_RETRY);
    for (UINT16 j=0; j < bNoOfSectors; j++)
    {
        if ( !IsDeviceDataReady(lpHDD->bIORegisterIdx,1000,TRUE))
        {
          //  DEBUG_PRINT_INFO("Data not ready");
        SetLastError(ERROR_NOT_READY);
            return HDD_DATA_NOT_READY;
        }
        for(UINT16 i=0; i<lpHDD->wBytesPerSector/2; i++)
        {
            OutPortWord(IDE_Con_IOBases[lpHDD->bIORegisterIdx][0]+IDE_CB_DATA, ((UINT16 *)lpBuffer)[(j * (lpHDD->wBytesPerSector)/2 )+i] ) ;
        }
    }
    return HDD_NO_ERROR;
}

//----------------------GSH Functions implementation ----------------
BYTE HDD_GetNoOfDevices()
{
    return bTotalDevices;
}
UINT32 HDD_Read(BYTE bDeviceNo,  LPGSHIOPARA lpIOPara, BYTE * lpBuffer)
{
    return HardDisk_ReadSectors(bDeviceNo, lpIOPara->dwLBASector, lpIOPara->dwSectorCount, lpBuffer);
}
UINT32 HDD_Write(BYTE bDeviceNo,  LPGSHIOPARA lpIOPara, BYTE * lpBuffer)
{
    return HardDisk_WriteSectors(bDeviceNo, lpIOPara->dwLBASector, lpIOPara->dwSectorCount, lpBuffer);
}
