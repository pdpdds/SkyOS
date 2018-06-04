#pragma once
#include "windef.h"

#define MAX_PCI_DEVICE	16

typedef struct tag_PCICfg
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
}PCICfg;

typedef struct tag_PCIDevice
{
	PCICfg	cfg;
}PCIDevice;

typedef struct tag_PCI
{
	int				nTotal;
	PCIDevice	ent[MAX_PCI_DEVICE];
}PCI;

int ScanPCIDevices();
int FindPCIDevice( int nIndex, PCIDevice *pPCI, UCHAR *pClass );
void DisplayPCIParameteter(PCIDevice *pPci );