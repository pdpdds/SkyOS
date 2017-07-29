#include "fdddrv.h"
#include "Console.h"
#include "Hal.h"

/*
 * descriptions : "tinybios" & "http://debs.future.easyspace.com/Programming/Hardware/FDC/floppy.html"
 */

/*
 * I/O ADDRESS MAP FOR THE PC/AT (from 82077AA datasheet)
 * Only 4 ports are being used for the disk related communication
 *
 * 3F0h -  .  - Unused
 * 3F1h -  .  - Unused
 * 3F2h -  W  - Digital Output Register
 * 3F3h -  .  - Unused
 * 3F4h -  R  - Main Status Register
 * 3F5h - R/W - Data Register
 * 3F6h -  .  - Unused
 * 3F7h -  W  - Data Rate Select Register
 * 3F7h -  R  - Digital Input Register
 */

/*
 * DEFINITIONS
 */
#define DEFAULT_STACK_SIZE				(64*1024) /* 64kbytes */

#define FDD_DOR_PORT		0x3f2
#define FDD_STATUS_PORT		0x3f4
#define FDD_CMD_PORT		0x3f4
#define FDD_DATA_PORT		0x3f5

typedef enum _FDD_JOB_TYPE {
	FDD_READ_SECTOR,
	FDD_WRITE_SECTOR,
} FDD_JOB_TYPE;

/*
 * STRUCTURES
 */
typedef struct _FDD_JOB_ITEM {
	FDD_JOB_TYPE			type;
	WORD					sector;
	BYTE					numbers_of_sectors;
	BYTE					*pt_data;

	HANDLE					thread;
} FDD_JOB_ITEM, *PFDD_JOB_ITEM;

#define FDD_JOB_ITEM_Q_SIZE		32
typedef struct _FDD_JOB_ITEM_Q {
	BYTE				cnt;

	BYTE				head;
	BYTE				tail;
	FDD_JOB_ITEM		queue[FDD_JOB_ITEM_Q_SIZE];
} FDD_JOB_ITEM_Q, *PFDD_JOB_ITEM_Q;

/*
 * GLOBAL FUNCTIONS
 */
bool FddInitializeDriver(VOID);
VOID Fdd_IRQ_Handler(VOID);

/*
 * INTERNEL FUNCTIONS
 */
static bool  FddpPopJobItem(FDD_JOB_ITEM *pJobItem);
static bool  FddpPushJobItem(FDD_JOB_ITEM *pJobItem);
static DWORD FddpJobProcessThread(PVOID StartContext);

static bool FddpReadWriteSector(FDD_JOB_TYPE JobType, WORD Sector, BYTE NumbersOfSectors, BYTE *pData);

static bool FddpTurnOnMotor(void);
static bool FddpTurnOffMotor(void);
static bool FddpSetupDMA(FDD_JOB_TYPE JobType);
static bool FddpWriteFdcData(BYTE Data);
static bool FddpReadFdcData(BYTE *pData);

/*
 * GLOBAL VARIABLES
 */
static FDD_JOB_ITEM_Q m_JobItemQ;
static HANDLE m_ProcessHandle, m_ThreadHandle;

bool m_FddInterruptOccurred;


/**********************************************************************************************************
 *                                             GLOBAL FUNTIONS                                            *
 **********************************************************************************************************/
bool FddInitializeDriver(VOID)
{
	/* motor off */
	if(!FddpTurnOffMotor()) {
		SkyConsole::Print("FddpTurnOffMotor() returned an error.\r\n");
		return FALSE;
	}
	
	return TRUE;
}

VOID Fdd_IRQ_Handler(VOID)
{
	m_FddInterruptOccurred=TRUE;
}


/**********************************************************************************************************
 *                                           EXPORTED FUNTIONS                                            *
 **********************************************************************************************************/
bool FddReadSector (WORD SectorNumber, BYTE NumbersOfSectors, BYTE *pData)
{
	FDD_JOB_ITEM job_item;

	EnterCriticalSection();
	job_item.type					= FDD_READ_SECTOR;
	job_item.sector					= SectorNumber;
	job_item.numbers_of_sectors		= NumbersOfSectors;
	job_item.pt_data				= pData;
	//job_item.thread					= PsGetCurrentThread();

	FddpReadWriteSector(FDD_READ_SECTOR, SectorNumber, NumbersOfSectors, pData);

	LeaveCriticalSection();

	return true;

	if(!FddpPushJobItem(&job_item)) {
		LeaveCriticalSection();
		return FALSE;
	}	

	return TRUE;
}

bool FddWriteSector(WORD SectorNumber, BYTE NumbersOfSectors, BYTE *pData)
{
	FDD_JOB_ITEM job_item;

	EnterCriticalSection();
	job_item.type					= FDD_WRITE_SECTOR;
	job_item.sector					= SectorNumber;
	job_item.numbers_of_sectors		= NumbersOfSectors;
	job_item.pt_data				= pData;
	//job_item.thread					= PsGetCurrentThread();

	if(!FddpPushJobItem(&job_item)) {
		LeaveCriticalSection();
		return FALSE;
	}

	LeaveCriticalSection();

	return TRUE;
}


/**********************************************************************************************************
 *                                           INTERNAL FUNTIONS                                            *
 **********************************************************************************************************/
#define BYTES_PER_SECTOR			512
#define SECTORS_PER_TRACK			18

static bool FddpTurnOnMotor(void)
{
	OutPortByte(FDD_DOR_PORT, 0x1c); /* drive A, DMA, FDC Enable */
	return TRUE;
}

static bool FddpTurnOffMotor(void)
{
	OutPortByte(FDD_DOR_PORT, 0x00);
	return TRUE;
}

static bool FddpWriteFdcData(BYTE Data)
{
	UCHAR status;

	do {
		status = InPortByte(FDD_STATUS_PORT);
	} while( (status & 0xc0) != 0x80 );
	OutPortByte(FDD_DATA_PORT, Data);

	return TRUE;
}

static bool FddpReadFdcData(BYTE *pData)
{
	UCHAR status;

	do {
		status = InPortByte(FDD_STATUS_PORT);
	} while( !(status & 0x80) );
	*pData = InPortByte(FDD_DATA_PORT);

	return TRUE;
}

static bool FddpSetupDMA(FDD_JOB_TYPE JobType)
{
	WORD count = BYTES_PER_SECTOR*SECTORS_PER_TRACK-1;

	/* setup mode */
	if(JobType == FDD_READ_SECTOR) {
		OutPortByte(0x0b, 0x46); /* single mode, write, channel 2 */
	} else {
		OutPortByte(0x0b, 0x4a); /* single mode, read, channel 2 */
	}

	/* setup address = 0x00000000 */
	OutPortByte(0x04, 0x00);
	OutPortByte(0x04, 0x00);
	OutPortByte(0x81, 0x00);

	/* setup count */
	OutPortByte(0x05, (BYTE)count);
	OutPortByte(0x05, (BYTE)(count >> 8));

	/* enable dma controller */
	OutPortByte(0x0a, 0x02); /* clear mask bit, channel 2 */

	return TRUE;
}

static bool FddpReadWriteSector(FDD_JOB_TYPE JobType, WORD Sector, BYTE NumbersOfSectors, BYTE *pData)
{
	BYTE *pDMAAddr = (BYTE *)0x00000000;
	BYTE drive, head, track, sector;
	int i;

/* ---------------------------------------- */ /* do not process write operation yet! */
	if(JobType == FDD_WRITE_SECTOR)
		return FALSE;
/* ---------------------------------------- */

	/* determine the values of drive, head, track and sector */
	drive	= 0; /* A */
	head	= ((Sector % (SECTORS_PER_TRACK * 2)) / SECTORS_PER_TRACK);
	track	= (Sector / (SECTORS_PER_TRACK * 2));
	sector	= (Sector % SECTORS_PER_TRACK) + 1;

	/* turn on motor - 1st interrupt */
	m_FddInterruptOccurred = FALSE; {
		FddpTurnOnMotor();
	} while(!m_FddInterruptOccurred) ;

	/* calibrate drive - 2nd interrupt */
	m_FddInterruptOccurred = FALSE; {
		FddpWriteFdcData(0x07); /* calibrate command */
		FddpWriteFdcData(0x00); /* drive 00 : A */
	} while(!m_FddInterruptOccurred) ;

	/* seek - 3rd interrupt */
	m_FddInterruptOccurred = FALSE; {
		FddpWriteFdcData(0x0f);
		FddpWriteFdcData((head << 2) + drive);
		FddpWriteFdcData(track);
	} while(!m_FddInterruptOccurred) ;

	/* setup dma */
/*	if(JobType == FDD_WRITE_SECTOR) {
		for(i=0; i<(BYTES_PER_SECTOR*NumbersOfSectors); i++) {
			*(pDMAAddr+i) = *(pData+i);
		}
	}
*/	FddpSetupDMA(JobType);

	/* r/w operation - 4th interrupt */
	m_FddInterruptOccurred = FALSE; {
/*		if(JobType == FDD_READ_SECTOR) {
*/			FddpWriteFdcData(0xe6);
/*		} else {
			FddpWriteFdcData(0xc5);
		}
*/		FddpWriteFdcData((head << 2) + drive);
		FddpWriteFdcData(track);
		FddpWriteFdcData(head);
		FddpWriteFdcData(1); /* FddpWriteFdcData(sector); */
		FddpWriteFdcData(2);
		FddpWriteFdcData(SECTORS_PER_TRACK); /* FddpWriteFdcData(sector+NumbersOfSectors-1); */
		FddpWriteFdcData(27); /* 3.5inch disk */
		FddpWriteFdcData(0xff);
	} while(!m_FddInterruptOccurred) ;

	/* copy buffer */
	pDMAAddr += (BYTES_PER_SECTOR*(sector-1));
	if(JobType == FDD_READ_SECTOR) {
		for(i=0; i<(BYTES_PER_SECTOR*NumbersOfSectors); i++) {
			*(pData+i) = *(pDMAAddr+i);
		}
	}

	/* turn off motor */
	FddpTurnOffMotor();

	return true;
}


/**********************************************************************************************************
 *                                           JOB_ITEM & THREAD                                            *
 **********************************************************************************************************/
static DWORD FddpJobProcessThread(PVOID StartContext)
{
	FDD_JOB_ITEM job_item;

	while(1) {
		if(!FddpPopJobItem(&job_item)) {			
			continue;
		}

		FddpReadWriteSector(job_item.type, job_item.sector, job_item.numbers_of_sectors, job_item.pt_data);		
	}

	return 0;
}

static bool FddpPopJobItem(FDD_JOB_ITEM *pJobItem)
{
	BOOL bResult = TRUE;

EnterCriticalSection();
	{
		/* check up count */
		if(m_JobItemQ.cnt == 0) {
			bResult = FALSE;
			goto $exit;
		}

		/* process */
		m_JobItemQ.cnt--;
		pJobItem->type					= m_JobItemQ.queue[m_JobItemQ.head].type;
		pJobItem->sector				= m_JobItemQ.queue[m_JobItemQ.head].sector;
		pJobItem->numbers_of_sectors	= m_JobItemQ.queue[m_JobItemQ.head].numbers_of_sectors;
		pJobItem->pt_data				= m_JobItemQ.queue[m_JobItemQ.head].pt_data;
		pJobItem->thread				= m_JobItemQ.queue[m_JobItemQ.head].thread;
		m_JobItemQ.head++;
		if(m_JobItemQ.head >= FDD_JOB_ITEM_Q_SIZE)
			m_JobItemQ.head = 0;
	}
$exit:
LeaveCriticalSection();
	return bResult;
}

static bool FddpPushJobItem(FDD_JOB_ITEM *pJobItem)
{
	BOOL bResult = TRUE;

	EnterCriticalSection();
	{
		/* check up the remain space of Q */
		if(m_JobItemQ.cnt >= FDD_JOB_ITEM_Q_SIZE) {
			bResult = FALSE;
			goto $exit;
		}

		/* process */
		m_JobItemQ.cnt++;
		m_JobItemQ.queue[m_JobItemQ.tail].type					= pJobItem->type;
		m_JobItemQ.queue[m_JobItemQ.tail].sector				= pJobItem->sector;
		m_JobItemQ.queue[m_JobItemQ.tail].numbers_of_sectors	= pJobItem->numbers_of_sectors;
		m_JobItemQ.queue[m_JobItemQ.tail].pt_data				= pJobItem->pt_data;
		m_JobItemQ.queue[m_JobItemQ.tail].thread				= pJobItem->thread;
		m_JobItemQ.tail++;
		if(m_JobItemQ.tail >= FDD_JOB_ITEM_Q_SIZE)
			m_JobItemQ.tail = 0;
	}
$exit:
LeaveCriticalSection();
	return bResult;
}