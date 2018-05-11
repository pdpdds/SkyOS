#include "SkyPCI.h"
#include "windef.h"
#include "memory.h"
#include "Hal.h"
#include "SkyAPI.h"
#include "sprintf.h"

PCI	pci;

typedef struct tag_PCIDeviceName
{
	UCHAR	c[3];
	char	*pName;
} PCIDeviceName;

static PCIDeviceName	pci_name[] = {
	{ { 0   ,   0,    1 }, "SCSI Controller"			},
	{ { 0xFF,   1,    1 }, "IDE Controller"				},
	{ { 0	,   0,    2	}, "Ethernet Controller"		},
	{ { 0	,   4,    2	}, "ISDN Controller"			},
	{ { 0	,   0,    3	}, "VGA Controller"				},
	{ { 0	,   0,    4	}, "Video Device"				},
	{ { 0	,   1,    4	}, "Audio Device"				},
	{ { 0	,   0,    6	}, "Host/PCI Bridge"			},
	{ { 0	,   1,    6	}, "PCI/ISA Bridge"				},
	{ { 0	, 128,    6	}, "Other Bridge"				},
	{ { 0	,   2,    6	}, "PCI/EISA Bridge"			},
	{ { 0	,   4,    6	}, "PCI/PCI Bridge"				},
	{ { 0	,   5,    6	}, "PCI/PCMCIA Bridge"			},
	{ { 0	,   3,    7	}, "Generic Modem"				},
	{ { 0	,   3,   12	}, "USB Controller"				},

	{ { 0,   0,    0    }, NULL }
};

// display the pci device's configuration area.
void DisplayPCIParameteter( PCIDevice *pPci )
{
    kdbg_printf( "UINT16  wVendorID        : %X\n", pPci->cfg.wVendorID );
    kdbg_printf( "UINT16  wDeviceID        : %X\n", pPci->cfg.wDeviceID );
    kdbg_printf( "UINT16  wCmdRegister     : %X\n", pPci->cfg.wCmdRegister );
    kdbg_printf( "UINT16  wStatusRegister  : %X\n", pPci->cfg.wStatusRegister );
    kdbg_printf( "UCHAR   byRevisionID     : %X\n", pPci->cfg.byRevisionID );
    kdbg_printf( "UCHAR   class_code[0]    : %X\n", pPci->cfg.class_code[0] );
    kdbg_printf( "UCHAR   class_code[1]    : %X\n", pPci->cfg.class_code[1] );
    kdbg_printf( "UCHAR   class_code[2]    : %X\n", pPci->cfg.class_code[2] );
    kdbg_printf( "UCHAR   byCacheLineSize  : %X\n", pPci->cfg.byCacheLineSize );
    kdbg_printf( "UCHAR   byLatencyTimer   : %X\n", pPci->cfg.byLatencyTimer );
    kdbg_printf( "UCHAR   byHeaderType     : %X\n", pPci->cfg.byHeaderType );
    kdbg_printf( "UCHAR   byBIST           : %X\n", pPci->cfg.byBIST );
    kdbg_printf( "DWORD   base_addr[0]     : %X\n", pPci->cfg.base_addr[0] );
    kdbg_printf( "DWORD   base_addr[1]     : %X\n", pPci->cfg.base_addr[1] );
    kdbg_printf( "DWORD   base_addr[2]     : %X\n", pPci->cfg.base_addr[2] );
    kdbg_printf( "DWORD   base_addr[3]     : %X\n", pPci->cfg.base_addr[3] );
    kdbg_printf( "DWORD   base_addr[4]     : %X\n", pPci->cfg.base_addr[4] );
    kdbg_printf( "DWORD   base_addr[5]     : %X\n", pPci->cfg.base_addr[5] );
    kdbg_printf( "DWORD   dwCISPointer     : %X\n", pPci->cfg.dwCISPointer );
    kdbg_printf( "UINT16  wSubVendorID     : %X\n", pPci->cfg.wSubVendorID );
    kdbg_printf( "UINT16  wSubSystemID     : %X\n", pPci->cfg.wSubSystemID );
    kdbg_printf( "DWORD   dwRomBaseAddr    : %X\n", pPci->cfg.dwRomBaseAddr );
    kdbg_printf( "UCHAR   byCapabilityBase : %X\n", pPci->cfg.byCapabilityBase );
    kdbg_printf( "UCHAR   rsv0[3]          : %X\n", pPci->cfg.rsv0[3] );
    kdbg_printf( "UCHAR   rsv1[4]          : %X\n", pPci->cfg.rsv1[4] );
    kdbg_printf( "UCHAR   byIntLine        : %X\n", pPci->cfg.byIntLine );
    kdbg_printf( "UCHAR   byIntPin         : %X\n", pPci->cfg.byIntPin );
    kdbg_printf( "UCHAR   byMinGnt         : %X\n", pPci->cfg.byMinGnt );
    kdbg_printf( "UCHAR   byMaxLat         : %X\n", pPci->cfg.byMaxLat );
}

// find pci device with class codes
int FindPCIDevice( int nIndex, PCIDevice *pPCI, UCHAR *pClass )
{
	int nI;

	for( nI = nIndex; nI < pci.nTotal; nI++ )
	{
		if( memcmp( pci.ent[nI].cfg.class_code, pClass, 3 ) == 0 )
		{
			memcpy( pPCI, &pci.ent[nI], sizeof( PCIDevice ) );
			return( nI );	// found~
		}
	}

	return( -1 );			// not found!
}

static char *get_pci_name( PCICfg *pCfg )
{
	int nI;

	for( nI = 0; pci_name[nI].pName != NULL; nI++ )
	{	
		if( pCfg->class_code[2] == pci_name[nI].c[2] && pCfg->class_code[1] == pci_name[nI].c[1] )
			return( pci_name[nI].pName );		  
	}

	return( NULL );
}

static int display_pci_info( PCI *pPCI )
{
	int		nI;
	char	*pS, szT[128];

	for( nI = 0; nI < pPCI->nTotal; nI++ )
	{
		pS = get_pci_name( &pPCI->ent[nI].cfg );
		if( pS == NULL )
		{
			sprintf( szT, "Unknown PCI Device ( %d.%d.%d )", pPCI->ent[nI].cfg.class_code[2], 
				pPCI->ent[nI].cfg.class_code[1], pPCI->ent[nI].cfg.class_code[0] );
			pS = szT;
		}

		kdbg_printf( "[%d] %s", nI, pS );
		if( pPCI->ent[nI].cfg.byIntLine == 0 )
			kdbg_printf( "\n" );
		else
			kdbg_printf( " IRQ-%d\n", pPCI->ent[nI].cfg.byIntLine );
	}	
	
	kdbg_printf( "Total %d pci devices.\n", nI );
	
	return( 0 );
}

static int read_pci_port_dword( int nBus, int nDevFn, int nIndex, DWORD *pDword )
{
	DWORD dwX;

	dwX = (DWORD)( (DWORD)0x80000000 | (DWORD)( nBus << 16 ) | (DWORD)( nDevFn << 8 ) |
		           (DWORD)( nIndex << 2 ) );

	// write pci address register
	OutPortDWord( (DWORD)0xCF8, dwX );

	// read pci data register
	*pDword = (DWORD)InPortDWord( (DWORD)0xCFC);

	return( 0 );
}

static int read_pci_config_data( PCICfg *pCfg, int nBus, int nDevFn )
{
	int		nI;
	DWORD	*pX;

	memset( pCfg, 0, sizeof( PCICfg ) );
	   	
	pX = (DWORD*)pCfg;
	for( nI = 0; nI < 16; nI++ )
	{
		read_pci_port_dword( nBus, nDevFn, nI, &pX[nI] );
		if( nI == 0 )
		{
			if( pX[0] == (DWORD)0xFFFFFFFF || pX[0] == 0 || pX[0] == (DWORD)0xFFFF0000 || pX[0] == (DWORD)0x0000FFFF )
				return( -1 );
		}	
	}

	//kdbg_printf( "bus(%d) devfn(%d), device id (%X), revision %d , class ( %d, %d, %d )\n", nBus, nDevFn, pCfg->wDeviceID, pCfg->byRevisionID, pCfg->class_code[0], pCfg->class_code[1], pCfg->class_code[2] );
																 
	return( 0 );
}	

int ScanPCIDevices()
{
	UCHAR			c[3];
	int				nI, nR, nBus, nDevFn;
	
	nI = 0;
	for( nBus = 0; nBus < 256; nBus++ )
	{
		for( nDevFn = 0; nDevFn < 256; nDevFn++ )
		{
			nR =  read_pci_config_data( &pci.ent[nI].cfg, nBus, nDevFn );
			if( nR == 0 )
			{
				if( pci.ent[nI].cfg.class_code[0] == c[0] && 
					pci.ent[nI].cfg.class_code[1] == c[1] &&
					pci.ent[nI].cfg.class_code[2] == c[2]  )
					continue;		

				c[0] = pci.ent[nI].cfg.class_code[0]; 
				c[1] = pci.ent[nI].cfg.class_code[1]; 
				c[2] = pci.ent[nI].cfg.class_code[2];
				
				nI++;
				if( nI >= MAX_PCI_DEVICE )
					goto RETURN;
			}
		}
	}	

RETURN:
	pci.nTotal = nI;

	display_pci_info( &pci );

	return( nI );
}

