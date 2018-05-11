#include "3cdef.h"
#include "windef.h"
#include "SkyPCI.h"
#include "memory.h"
#include "Hal.h"
#include "SkyAPI.h"
#include "header.h"
#include "kheap.h"
#include "VirtualMemoryManager.h"
#include "SkyAPI.h"
#include "SkyConsole.h"

static UPDControl	ucb;

static void int_handler_3c905b();

static RegInfo3c905bStt ri_3c905b[] = {
	// Name,			Window, Offset, Size
    { RN_BiosRomAddr,       0,   0x4,    4   },
    { RN_BiosRomData,       0,   0x8,    1   },
    { RN_EepromCommand,     0,   0xA,    2   },
    { RN_EepromData,        0,   0xC,    2   },
    { RN_IntStatus,         0,   0xE,    2   },
    { RN_Command,           0,   0xE,    2   },
	
    { RN_StationAddrLo,     2,   0x0,    2   },
    { RN_StationAddrMid,    2,   0x2,    2   },
    { RN_StationAddrHi,     2,   0x4,    2   },
    { RN_StationMaskLo,     2,   0x6,    2   },
    { RN_StationMaskMid,    2,   0x8,    2   },
    { RN_StationMaskHi,     2,   0xA,    2   },
    { RN_resetOptions,      2,   0xC,    2   },

    { RN_InternalConfig,    3,   0x0,    4   },
    { RN_MaxPktSize,        3,   0x4,    2   },
    { RN_MacControl,        3,   0x6,    2   },
    { RN_MediaOptions,      3,   0x8,    2   },
    { RN_RxFree,            3,   0xA,    2   },
    { RN_TxFree,            3,   0xC,    2   },

    { RN_FifoDiagnostic,    4,   0x4,    2   },
    { RN_NetworkDiagnostic, 4,   0x6,    2   },
    { RN_PhysicalMgmt,      4,   0x8,    2   },
    { RN_MediaStatus,       4,   0xA,    2   },
    { RN_BadSSD,            4,   0xC,    1   },
    { RN_UpperByteOk,       4,   0xD,    1   },

    { RN_TxStartThresh,     5,   0x0,    2   },
    { RN_RxEarlyThresh,     5,   0x6,    2   },
    { RN_RxFilter,          5,   0x8,    1   },
    { RN_TxReclaimThresh,   5,   0x9,    1   },
    { RN_InterruptEnable,   5,   0xA,    2   },
    { RN_IndicationEnable,  5,   0xC,    2   },

    { RN_CarrierLost,       6,   0x0,    1   },
    { RN_SeqError,          6,   0x1,    1   },
    { RN_MultipleCollisions,6,   0x2,    1   },
    { RN_SingleCollisions,  6,   0x3,    1   },
    { RN_LateCollisions,    6,   0x4,    1   },
    { RN_RxOverruns,        6,   0x5,    1   },
    { RN_FramesXmittedOk,   6,   0x6,    1   },
    { RN_FramesRcvdOk,      6,   0x7,    1   },
    { RN_framesDeferred,    6,   0x8,    1   },
    { RN_UpperFramesOk,     6,   0x9,    1   },
    { RN_BytesRcvdOk,       6,   0xA,    2   },
    { RN_ByteXmittedOk,     6,   0xC,    2   },

    { RN_VlanMask,          7,   0x0,    2   },
    { RN_VlanEtherType,     7,   0x4,    2   },
    { RN_PowerMgmtEvent,    7,   0xC,    2   },
									   
    { RN_UpListPtr,			-1,  0x38,   4   },

	{ 0, 0, 0, 0 }
};

// PCI Configuration register set.
static PCIDevice	pci_3c905b;

// get 3c905b register window, offset, size
static RegInfo3c905bStt *get_3c905b_reg_info( int nRegName )
{
	if( nRegName < 0 || nRegName >= END_OF_REG_NAME_CONST )
		return( NULL );

	return( &ri_3c905b[ nRegName ] );
}

// change active register window
static int select_register_window_3c905b(int nWindow )
{
	DWORD dwPort, dwX;

	dwPort = ( pci_3c905b.cfg.base_addr[0] & (DWORD)0xFFFFFF80 ) + (DWORD)0x0E;
	dwX = (DWORD)0x0800 + (DWORD)( nWindow	& 0x07 );
	
	OutPortWord( dwPort, dwX );

	return( 0 );
}

// write to 3c905b general register
static void write_3c905b_register( int nRegName, DWORD dwValue )
{
	RegInfo3c905bStt	*pR;
	DWORD				dwPort;

	pR = get_3c905b_reg_info( nRegName );
	if( pR == NULL )
		return;	// error!
	
	// calculate port
	dwPort = (DWORD)( pci_3c905b.cfg.base_addr[0] & (DWORD)0xFFFFFF80 )+ (DWORD)pR->nOffset;

	// select window
	if( pR->nWindow >= 0 && pR->nOffset <= 8 )
		select_register_window_3c905b( pR->nWindow );

	// write port
	if( pR->nSize == 1 )
		OutPortByte( dwPort, dwValue );
	else if (pR->nSize == 2)
	{
		OutPortDWord(dwPort, dwValue);
	}
	else
		OutPortDWord( dwPort, dwValue );
}

// read from 3c905b general register
static DWORD read_3c905b_register( int nRegName )
{
	RegInfo3c905bStt	*pR;
	DWORD				dwX, dwPort;

	pR = get_3c905b_reg_info( nRegName );
	if( pR == NULL )
		return( 0 );	// error!

	dwX = 0;
	// calculate port
	dwPort = (DWORD)( pci_3c905b.cfg.base_addr[0] & (DWORD)0xFFFFFF80 )+ (DWORD)pR->nOffset;

	// select window
	if( pR->nOffset <= 8 )
		select_register_window_3c905b( pR->nWindow );

	// read port
	if (pR->nSize == 1)
	{
		dwX = InPortByte(dwPort);
	}
	else if (pR->nSize == 2)
	{
	
		dwX = InPortWord((USHORT)dwPort);
		//SkyConsole::Print("%d Port 0x%x\n", pR->nSize, dwPort);
	}
	else
	{
		dwX = InPortDWord(dwPort);
	}
	return( dwX );
}

// check status register if the NIC is in comand processing.
static int wait_command_processing()
{
	DWORD	dwX;
	int		nI;

	for( nI = 0; nI < 100000; nI++ )
	{
		dwX = read_3c905b_register( RN_IntStatus );
		if( !( dwX & INT_BIT_cmdInProgress ) )
			return( 0 );
	}

	return( -1 );	// error!!
}

// read eeprom data
static DWORD read_eeprom_data(int nOffset)
{
	int		nI;
	DWORD	dwX;

	// check the busy bit in the EepromCommand Register
	for (nI = 0; nI < 65000; nI++)
	{
		dwX = read_3c905b_register(RN_EepromCommand);
		if ((dwX & (DWORD)0x8000) == 0)
			break;	// Not busy!
	}

	// send read command to the EepromCommand register
	dwX = (DWORD)nOffset;	// Mac word Offset
	dwX += 0x80;			// Read Command
	write_3c905b_register(RN_EepromCommand, dwX);
	
	// wait (162us)... (do not omit this!!)
	msleep(1000);
	// Read Eeprom Data
	dwX = read_3c905b_register(RN_EepromData);
	
	return(dwX);
}

// find 6 byte mac address
int get_3c905b_mac_address( UINT16 *pMac )
{
	DWORD dwX; 

	memset( pMac, 0, 6 );
	
	dwX = read_eeprom_data( 0x00 );
	pMac[0] = (UINT16)dwX;
	
	dwX = read_eeprom_data( 0x01 );
	pMac[1] = (UINT16)dwX;
	dwX = read_eeprom_data( 0x02 );
	pMac[2] = (UINT16)dwX;

	return( 0 );
}				

// find 3CM905B network interface card
int find_3c905b_nic()
{
	int				nI;
	UCHAR			class_code[3];

	class_code[0] = 0;
	class_code[1] = 0;
	class_code[2] = 2;
	
	for( nI = 0; nI >= 0; nI++ )
	{
		nI = FindPCIDevice( nI, &pci_3c905b, class_code );
		if( nI > 0 )
		{	// is it a 3COM905B ??
			DisplayPCIParameteter( &pci_3c905b );
			break;
		}
	}
	if( nI < 0 )
		return( -1 );	// not found

	return( 0 );
}

// send command to 3c905b
// wCommandValue includes command and parameter bits.
static int issue_3c905b_command( UINT16 wCommandValue )
{
	DWORD dwPort;

	dwPort = ( pci_3c905b.cfg.base_addr[0] & (DWORD)0xFFFFFF80 ) + (DWORD)0x0E;
	OutPortWord( dwPort, wCommandValue );

	return( 0 );
}

// allocate upd blocks
static int alloc_ucbs( UPDControl *pUCB )
{
	int nI, nJ;

	// alocate page
	pUCB->nTotalUPD  = MAX_UPDS;
	pUCB->nTotalPage = pUCB->nTotalUPD + 1;
	pUCB->pPage = (void*)kmalloc( pUCB->nTotalPage * 4096 + 4095 );
	if( pUCB->pPage == NULL )
		return( -1 );

	pUCB->pUPD = (UPD*)( (DWORD)((DWORD)pUCB->pPage + 4095)&(DWORD)0xFFFFF000 );
	pUCB->pFrag = (char*)( (DWORD)pUCB->pUPD + 4096 );

	// set structure member fields
	memset( pUCB->pUPD, 0, 4096 );
	for( nJ = nI = 0; nI < MAX_UPDS; nI++ )
	{
		if (nI + 1 < MAX_UPDS)
		{
			//pUCB->pUPD[nI].dwUpNextPtr = get_physical_address((DWORD)&pUCB->pUPD[nI + 1]);
			pUCB->pUPD[nI].dwUpNextPtr = (DWORD)VirtualMemoryManager::GetPhysicalAddressFromVirtualAddress(VirtualMemoryManager::GetCurPageDirectory(), (DWORD)&pUCB->pUPD[nI + 1]);
			
		}
		//pUCB->pUPD[nI].frag[0].dwAddr   = get_physical_address( (DWORD)&pUCB->pFrag[ FRAG_SIZE * nJ++] ); 
		pUCB->pUPD[nI].frag[0].dwAddr = (DWORD)VirtualMemoryManager::GetPhysicalAddressFromVirtualAddress(VirtualMemoryManager::GetCurPageDirectory(), (DWORD)&pUCB->pFrag[FRAG_SIZE * nJ++]); 
		pUCB->pUPD[nI].frag[0].dwLength = (DWORD)FRAG_SIZE;
		if( nI + 1 >= MAX_UPDS )
			pUCB->pUPD[nI].frag[0].dwLength += (DWORD)0x80000000;
	}
	
	return( 0 );
}

// set interrupt enable register
static int set_interrupt_enable_register()
{
	int		nR;
	UINT16	wIntBit;

	wIntBit = INT_BIT_upComplete | //INT_BIT_rxComplete | 
			  INT_BIT_dnComplete | //INT_BIT_txComplete | 
			  INT_BIT_hostError  | INT_BIT_intRequested | 
			  INT_BIT_interruptLatch;

	nR = issue_3c905b_command( (UINT16)(CV_SetIndicationEnable + wIntBit) );
	nR = issue_3c905b_command( (UINT16)(CV_SetInterruptEnable + wIntBit) );
	
	return( nR );
}				

// initialize 3COM905B
int init_3c905b( UINT16 *pMacWord )
{
	// set full duplex
	write_3c905b_register( RN_MacControl, FULL_DUPLEX_ENABLE );
	issue_3c905b_command( CV_TxReset );
	wait_command_processing();
	issue_3c905b_command( CV_RxReset );
	wait_command_processing();

	// set mac address from eeprom
	write_3c905b_register( RN_StationAddrLo,  (DWORD)pMacWord[0]  );
	write_3c905b_register( RN_StationAddrMid, (DWORD)pMacWord[1]  );
	write_3c905b_register( RN_StationAddrHi,  (DWORD)pMacWord[1]  );
	
	// set the receive filter
	issue_3c905b_command( CV_SetRxFilter + RX_FILTER_ALL );

	// set interrupt handler
	//set_int_handler(pci_3c905b.cfg.byIntLine, int_handler_3c905b);
	setvect(pci_3c905b.cfg.byIntLine, int_handler_3c905b);

	// make ucbs
	alloc_ucbs( &ucb );

	// set indications and interrupt
	set_interrupt_enable_register();

	// rxenable
	issue_3c905b_command( CV_RxEnable );

	// upunstall
	issue_3c905b_command( CV_UpUnStall );

	// set UpNextPtr
	//write_3c905b_register( RN_UpListPtr, get_physical_address( (DWORD)ucb.pUPD ) );
	DWORD physicalAddress = (DWORD)VirtualMemoryManager::GetPhysicalAddressFromVirtualAddress(VirtualMemoryManager::GetCurPageDirectory(), (DWORD)ucb.pUPD);
	write_3c905b_register(RN_UpListPtr, physicalAddress);
	return( 0 );
}

// interrupt handler
static void int_handler_3c905b()
{	
	DWORD dwX;

	// read status register
	dwX = read_3c905b_register( RN_IntStatus );

	kdbg_printf( "NIC Interrupt = 0x%08X\n", dwX );
 


	// acknowledge interupt
	issue_3c905b_command( CV_AckInterrupt		+ 
						ACK_BIT_interruptLatch	+	ACK_BIT_rxEarly			+
						ACK_BIT_intRequest		+	ACK_BIT_dnComplete		+
						ACK_BIT_upComplete		);
	return;
}	

// dump upds
int dump_upds()
{
	int		nI;
	UPD	*pU;

	for( nI = 0; nI < ucb.nTotalUPD; nI++ )
	{
		pU = &ucb.pUPD[nI];

		kdbg_printf( "ucb.pUPD[%d]->dwUpPktStatus : 0x%08X   FRAG(0x%08X)\n", nI, pU->dwUpPktStatus, (DWORD)&ucb.pFrag[nI*4096] );
	}	

	return( 0 );
}
