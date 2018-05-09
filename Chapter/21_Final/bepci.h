#pragma once
#include "windef.h"

#define MAX_PCI_DEVICE	16

typedef struct PCICfgTag
{
	UINT16  wVendorID;
	UINT16	wDeviceID;
	UINT16  wCmdRegister;
	UINT16  wStatusRegister;
	UCHAR	byRevisionID;
	UCHAR	class_code[3];
	UCHAR	byCacheLineSize;
	UCHAR	byLatencyTimer;
	UCHAR	byHeaderType;
	UCHAR	byBIST;
	DWORD	base_addr[6];
	DWORD	dwCISPointer;
	UINT16	wSubVendorID;
	UINT16	wSubSystemID;
	DWORD	dwRomBaseAddr;
	UCHAR	byCapabilityBase;
	UCHAR	rsv0[3];
	UCHAR	rsv1[4];
	UCHAR	byIntLine;
	UCHAR	byIntPin;
	UCHAR	byMinGnt;
	UCHAR	byMaxLat;
}PCICfgTag, PCICfgStt;

typedef struct PCIDeviceTag
{
	PCICfgStt	cfg;
}PCIDeviceTag, PCIDeviceStt;

typedef struct PCITag
{
	int				nTotal;
	PCIDeviceStt	ent[MAX_PCI_DEVICE];
}PCITag, PCIStt;

extern int scan_pci_devices();
extern int find_pci_device( int nIndex, PCIDeviceStt *pPCI, UCHAR *pClass );
extern void display_pci_parameter( PCIDeviceStt *pPci );
