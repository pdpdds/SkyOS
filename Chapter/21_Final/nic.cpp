#include "SkyOS.h"
#include "3c905b.h"

// initialize network inteerface card
int init_nic()
{
	int				nR;
	UINT16			mac_addr[3];
	UCHAR			*pM;

	// find nic
	nR = find_3c905b_nic();
	
	if( nR < 0 )
	{
		kdbg_printf( "3C905B NIC not found!\n" );
		return( -1 );	// nic not found
	}

	kdbg_printf("3C905B NIC Detected!\n");

	// get MAC address
	nR = get_3c905b_mac_address( mac_addr );

	if( nR < 0 )
	{
		kdbg_printf( "3C905B NIC MAC Address : Not found!\n" );
		return( -1 );
	}
	else
	{	// display mac address
		pM = (UCHAR*)mac_addr;
		kdbg_printf( "3C905B NIC MAC Address : %x:%x:%x:%x:%x:%x\n",
		pM[1], pM[0], pM[3], pM[2], pM[5], pM[4] );
	}
	SkyConsole::Print("222222\n");
	for (;;);
	// initialize 3c905b
 	init_3c905b( mac_addr );

	return( 0 );
}