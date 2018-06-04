#pragma once
#include "windef.h"
#include "stdint.h"

/*---IDE Controller's Compability mode Base Registers and Interrupts ---*/
static const BYTE IDE_MAX_CONTROLLER = 4;
static const BYTE IDE_CONTROLLER_NUM = 1;

static const UINT16 IDE_Con_IOBases[IDE_MAX_CONTROLLER][2] =
{
	{ 0x1F0, 0x3F6 },
	{ 0x170, 0x376 },
	{ 0x1E8, 0x3EE },
	{ 0x168, 0x36E }
};
static const UINT16 IDE_Con_IRQs[IDE_MAX_CONTROLLER][3] =
{
	{ 14, 0, 0 },
	{ 15, 0, 0 },
	{ 11, 12, 9 },
	{ 10, 12, 9 }
};

/* ---------- Command Block Registers Offset From Base ------------------*/
static const BYTE IDE_CB_DATA = 0;       /* Read & Write */
static const BYTE IDE_CB_ERROR = 1;	/* Read only    */
static const BYTE IDE_CB_FEATURES = 1;	/* Write only   */
static const BYTE IDE_CB_STATUS = 7;     /* Read only    */
static const BYTE IDE_CB_COMMAND = 7;    /* Write only   */

static const BYTE IDE_CB_SECTOR_COUNT = 2;

static const BYTE IDE_CB_SECTOR = 3;
static const BYTE IDE_CB_CYLINDER_LOW = 4;
static const BYTE IDE_CB_CYLINDER_HIGH = 5;
static const BYTE IDE_CB_DEVICE_HEAD = 6;

static const BYTE IDE_CB_LBA_0_7 = 3;
static const BYTE IDE_CB_LBA_8_15 = 4;
static const BYTE IDE_CB_LBA_16_23 = 5;
static const BYTE IDE_CB_LBA_24_27 = 6;

/* ---------- Control Block Registers Constants ------------------*/
static BYTE IDE_CON_BSY = 7;
static BYTE IDE_CON_DRDY = 6;
static BYTE IDE_CON_DF = 5;
static BYTE IDE_CON_DSC = 4;
static BYTE IDE_CON_DRQ = 3;
static BYTE IDE_CON_CORR = 2;
static BYTE IDE_CON_IDX = 1;
static BYTE IDE_CON_ERR = 0;

/* ---------- Control Block Registers Offset From Base ------------*/
static const BYTE IDE_CON_ALTERNATE_STATUS = 2;
static const BYTE IDE_CON_DEVICE_CONTROL = 2;

/*-------------     ATA - 2  Command's OpCodes  -------------------*/
static const BYTE IDE_COM_ACKNOWLEDGE_MEDIA_CHANGE = 0xDB;
static const BYTE IDE_COM_POST_BOOT = 0xDC;
static const BYTE IDE_COM_PRE_BOOT = 0xDD;

/*const BYTE IDE_COM_CHECK_POWER_MODE=98hE5h */

static const BYTE IDE_COM_DOOR_LOCK = 0xDE;
static const BYTE IDE_COM_DOOR_UNLOCK = 0xDF;
static const BYTE IDE_COM_DOWNLOAD_MICROCODE = 0x92;
static const BYTE IDE_COM_EXECUTE_DEVICE_DIAGNOSTIC = 0x90;
static const BYTE IDE_COM_FORMAT_TRACK = 0x50;
static const BYTE IDE_COM_IDENTIFY_DEVICE = 0xEC;

/*const BYTE IDE_COM_IDLE=97hE3h
const BYTE IDE_COM_IDLE_IMMEDIATE=0x95hE1h */

static const BYTE IDE_COM_INITIALIZE_DEVICE_PARAMETERS = 0x91;
static const BYTE IDE_COM_MEDIA_EJECT = 0xED;
static const BYTE IDE_COM_NOP = 0x0;
static const BYTE IDE_COM_READ_BUFFER = 0xE4;
static const BYTE IDE_COM_READ_DMA_W_RETRY = 0xC8;
static const BYTE IDE_COM_READ_DMA = 0xC9;
static const BYTE IDE_COM_READ_LONG_W_RETRY = 0x22;
static const BYTE IDE_COM_READ_LONG = 0x23;
static const BYTE IDE_COM_READ_MULTIPLE = 0xC4;
static const BYTE IDE_COM_READ_SECTORS_W_RETRY = 0x20;
static const BYTE IDE_COM_READ_SECTORS = 0x21;
static const BYTE IDE_COM_READ_VERIFY_SECTORS_W_RETRY = 0x40;
static const BYTE IDE_COM_READ_VERIFY_SECTORS = 0x41;
static const BYTE IDE_COM_RECALIBRATE = 0x10;
static const BYTE IDE_COM_SEEK = 0x70;
static const BYTE IDE_COM_SET_FEATURES = 0xEF;
static const BYTE IDE_COM_SET_MULTIPLE_MODE = 0xC6;

/*const BYTE IDE_COM_SimpleSleep=99hE6h
const BYTE IDE_COM_STANDBY=96hE2h
const BYTE IDE_COM_STANDBY_IMMEDIATE=94hE0h */

static const BYTE IDE_COM_WRITE_BUFFER = 0xE8;
static const BYTE IDE_COM_WRITE_DMA_W_RETRY = 0xCA;
static const BYTE IDE_COM_WRITE_DMA = 0xCB;
static const BYTE IDE_COM_WRITE_LONG_W_RETRY = 0x32;
static const BYTE IDE_COM_WRITE_LONG = 0x33;
static const BYTE IDE_COM_WRITE_MULTIPLE = 0xC5;
static const BYTE IDE_COM_WRITE_SAME = 0xE9;
static const BYTE IDE_COM_WRITE_SECTORS_W_RETRY = 0x30;
static const BYTE IDE_COM_WRITE_SECTORS = 0x31;
static const BYTE IDE_COM_WRITE_VERIFY = 0x3C;


//에러 관련 상수
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


typedef struct tag_HDDInfo
{
	BYTE IORegisterIdx;  // IO 리소스와 IRQ 인덱스 번호
	BYTE IRQ;

	BYTE DeviceNumber;		// 0: 마스터, 1:슬레이브 
	char SerialNumber[21];
	char FirmwareRevision[21];
	char ModelNumber[21];
	BYTE DMASupported;
	BYTE LBASupported;

	BYTE DeviceID[512];		// 디바이스 식별 커맨드로부터 얻은 디바이스 ID

	BYTE Mode;			// 0 - CHS 모드, 1-LBA 모드

	UINT16 CHSHeadCount;
	UINT16 CHSCylinderCount;
	UINT16 CHSSectorCount;
	UINT32 LBACount;   		//LBA 모드에서만 사용

	UINT16 BytesPerSector;

	BYTE LastError;
}HDDInfo;

typedef struct tag_VFS_IO_PARAMETER
{
	UINT32 Cylinder;
	UINT16 Head;
	UINT16 Sector;

	UINT32 LBASector;

	UINT16 SectorCount;

	BYTE   Mode;		//0이면 LBA 모드

	UINT16 Others[20];
}VFS_IO_PARAMETER;

