/*
	Created on 20-May-2002 by Sam - samuelhard@yahoo.com
	You can get the latest copy from www.geocities.com/samuelhard

	This file provide interface to detect the PCI devices present
	in the system and also provide the IO address, IRQ information.

	PCI 2.2
*/
#ifndef __PCI_H
#define __PCI_H

#include "windef.h"


extern "C" {


#define PCI_CONFIG_ADDRESS 0xCF8
#define PCI_CONFIG_DATA 0xCFC


	/* This is the 256 byte PCI Configuration Space.
	The first 16 bytes are PreDefined Header and others are depends on
	the Header type 0 - NonBridge 1- Bridge 2 - PCCard
	*/
	struct PCIConfigurationSpace
	{
		WORD VendorID;
		WORD DeviceID;
		WORD CommandRegister;
		WORD StatusRegister;
		BYTE RevisionID;
		BYTE ProgIF;
		BYTE SubClass;
		BYTE ClassCode;
		BYTE CacheLineSize;
		BYTE Latency;
		BYTE HeaderType;
		BYTE BIST;

		union
		{
			struct
			{
				DWORD BaseAddress0;
				DWORD BaseAddress1;
				DWORD BaseAddress2;
				DWORD BaseAddress3;
				DWORD BaseAddress4;
				DWORD BaseAddress5;
				DWORD CardBusCIS;
				WORD  SubSystemVendorID;
				WORD  SubSystemDeviceID;
				DWORD ExpansionROM;
				BYTE  CapPointer;
				BYTE  Reserved1[3];
				DWORD Reserved2[1];
				BYTE  InterruptLine;
				BYTE  InterruptPin;
				BYTE  MinGrant;
				BYTE  MaxLatency;
				DWORD DeviceSpecific[48];
			} NonBridge;

			struct
			{
				DWORD BaseAddress0;
				DWORD BaseAddress1;
				BYTE  PrimaryBus;
				BYTE  SecondaryBus;
				BYTE  SubordinateBus;
				BYTE  SecondaryLatency;
				BYTE  IOBaseLow;
				BYTE  IOLimitLow;
				WORD  SecondaryStatus;
				WORD  MemoryBaseLow;
				WORD  MemoryLimitLow;
				WORD  PrefetchBaseLow;
				WORD  PrefetchLimitLow;
				DWORD PrefetchBaseHigh;
				DWORD PrefetchLimitHigh;
				WORD  IOBaseHigh;
				WORD  IOLimitHigh;
				DWORD Reserved2[1];
				DWORD ExpansionROM;
				BYTE  InterruptLine;
				BYTE  InterruptPin;
				WORD  BridgeControl;
				DWORD DeviceSpecific[48];
			} Bridge;
			struct
			{
				DWORD ExCa_base;
				BYTE  cap_ptr;
				BYTE  reserved05;
				WORD  secondary_status;
				BYTE  PCI_bus;
				BYTE  CardBus_bus;
				BYTE  subordinate_bus;
				BYTE  latency_timer;
				DWORD memory_base0;
				DWORD memory_limit0;
				DWORD memory_base1;
				DWORD memory_limit1;
				WORD  IObase_0low;
				WORD  IObase_0high;
				WORD  IOlimit_0low;
				WORD  IOlimit_0high;
				WORD  IObase_1low;
				WORD  IObase_1high;
				WORD  IOlimit_1low;
				WORD  IOlimit_1high;
				BYTE  interrupt_line;
				BYTE  interrupt_pin;
				WORD  bridge_control;
				WORD  subsystem_vendorID;
				WORD  subsystem_deviceID;
				DWORD legacy_baseaddr;
				DWORD cardbus_reserved[14];
				DWORD vendor_specific[32];
			} CardBus;
		} PreDefinedHeader;
	};
	//------------------------------Static class Descriptions for PCI 2.2 definitions------------------

	struct PCIPrgInfo
	{
		BYTE PrgInfoCode;
		char * Description;
	};
	struct PCISubClass
	{
		BYTE SubClassCode;
		char * Description;
		struct PCIPrgInfo * PrgInfo;
	};
	struct PCIBaseClass
	{
		BYTE ClassCode;
		char * Description;
		struct PCISubClass * SubClass;
	};
	struct PCIDeviceDetails
	{
		BYTE Bus;
		BYTE Device;
		BYTE Function;
		PCIConfigurationSpace PCIConfDetails;
	};
	extern struct PCIBaseClass PCIClassDetails[];
	// --------------------------------- Functions ----------------------------

	BYTE GetPCIConfigurationSpace(BYTE Bus, BYTE Device, BYTE Function, struct PCIConfigurationSpace * PCICfg);
	UINT16 InitPCIDevices();
	void EnumeratePCIDevices(BYTE(*CallBackFn) (struct PCIConfigurationSpace *));

	VOID RequestPCIList();
}

#endif
