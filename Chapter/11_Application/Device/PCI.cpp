/*  	Created on 14-June-2002 by Sam - samuelhard@yahoo.com


	This file provide interface to detect the PCI devices present
	in the system and also provide the IO address, IRQ information.

	Supports PCI 2.2
*/
#include "SkyOS.h"
#include "list.h"
#include "PCI.h"

#define PCI_CONFIG_ADDRESS 0xCF8
#define PCI_CONFIG_DATA 0xCFC

/* This is the 256 byte PCI Configuration Space.
The first 16 bytes are PreDefined Header and others are depends on
the Header type 0 - NonBridge 1- Bridge 2 - PCCard
*/
//------------------------------Static class Descriptions for PCI 2.2 definitions------------------
//--------------------------------Sub class Disk ---------------------------
struct PCISubClass PCISubClassDisk[] =
{
	{ 0x00, "SCSI" , NULL},
	{ 0x01, "IDE" ,NULL },
	{ 0x02, "Floppy",NULL },
	{ 0x03, "IPI",NULL},
	{ 0x04, "RAID" ,NULL},
	{ 0x80, "Other",NULL },
			{ 0,0,0}
};

struct PCISubClass PCISubClassNetwork[] =
{
	{ 0x00, "Ethernet" ,NULL },
	{ 0x01, "TokenRing" ,NULL},
	{ 0x02, "FDDI",NULL },
	{ 0x03, "ATM",NULL },
	{ 0x04, "ISDN",NULL },
	{ 0x80, "Other",NULL },
			{ 0,0,0}
};

struct PCISubClass PCISubClassDisplay[] =
{
	{ 0x00, "VGA",NULL },
	{ 0x01, "SuperVGA",NULL },
	{ 0x02, "XGA",NULL },
	{ 0x80, "Other" ,NULL},
			{ 0,0,0}
};

struct PCISubClass PCISubClassMultimedia[] =
{
	{ 0x00, "Video",NULL },
	{ 0x01, "Audio",NULL },
	{ 0x02, "Telephony",NULL },
	{ 0x80, "Other",NULL },
			{ 0,0,0}
};

struct PCISubClass PCISubClassMemory[] =
{
	{ 0x00, "RAM",NULL },
	{ 0x01, "Flash memory",NULL },
	{ 0x80, "Other",NULL },
			{ 0,0,0}
};

struct PCISubClass PCISubClass[] =
{
	{ 0x00, "CPU/PCI", NULL },
	{ 0x01, "PCI/ISA", NULL },
	{ 0x02, "PCI/EISA", NULL  },
	{ 0x03, "PCI/MCA", NULL  },
	{ 0x04, "PCI/PCI", NULL  },
	{ 0x05, "PCI/PCMCIA", NULL  },
	{ 0x06, "PCI/NuBus", NULL  },
	{ 0x07, "PCI/CardBus", NULL  },
	{ 0x08, "PCI/RACEway", NULL  },
	{ 0x80, "Other", NULL  },
			{ 0,0,0}
};

//----------------------------Communication ---------------------------------
struct PCIPrgInfo PCIPrgInfoSerial[] =
{
	{ 0x0, "Generic XT" },
	{ 0x1, "16450" } ,
	{ 0x2, "16550" } ,
	{ 0x3, "16650" } ,
	{ 0x4, "16750" } ,
			{ 0,0}
};

struct PCIPrgInfo PCIPrgInfoParallel[] =
{
	{ 0x0, "Standard" },
	{ 0x1, "Bidirectional" } ,
	{ 0x2, "ECP 1.x Complaint" } ,
	{ 0x3, "IEEE 1284 Controller" } ,
	{ 0x3, "IEEE 1284 Target" } ,
	{ 0xFE, "16750" } ,
			{ 0,0}
};

struct PCIPrgInfo PCIPrgInfoModem[] =
{
	{ 0x0, "Generic Modem" },
	{ 0x1, "Hayes-Compatible Modem 16450" },
	{ 0x2, "Hayes-Compatible Modem 16550" },
	{ 0x3, "Hayes-Compatible Modem 16650" },
	{ 0x4, "Hayes-Compatible Modem 16750" },
	{ 0x5, "Hayes-Compatible Modem 16850" },
	{ 0x6, "Hayes-Compatible Modem 16950" },
			{ 0,0}
};

struct PCISubClass PCISubClassCommunication[] =
{
	{ 0x00, "Serial", PCIPrgInfoSerial },
	{ 0x01, "Parallel", PCIPrgInfoParallel },
	{ 0x02, "MultiPort Serial", NULL },
	{ 0x03, "Modem" , PCIPrgInfoModem },
	{ 0x80, "Other" , NULL },
			{ 0,0,0}
};
//-------------------------Base System Peripherals ---------------------------------
struct PCIPrgInfo PCIPrgInfoPIC[] =
{
	{ 0x0, "Generic 8259" },
	{ 0x1, "ISA" },
	{ 0x2, "EISA" },
	{ 0x10, "I/O APIC" },
	{ 0x20, "I/O(x) APIC" },
			{ 0,0}
};
struct PCIPrgInfo PCIPrgInfoDMAC[] =
{
	{ 0x0, "Generic 8237" },
	{ 0x1, "ISA" },
	{ 0x2, "EISA" },
			{ 0,0}
};

struct PCIPrgInfo PCIPrgInfoTimer[] =
{
	{ 0x0, "Generic 8254" },
	{ 0x1, "ISA" },
	{ 0x2, "EISA" },
			{ 0,0}
};

struct PCIPrgInfo PCIPrgInfoRTC[] =
{
	{ 0x0, "Generic" },
	{ 0x1, "ISA" },
			{ 0,0}
};

struct PCISubClass PCISubClassSystemPeripherals[] =
{
	{ 0x00, "PIC", PCIPrgInfoPIC  },
	{ 0x01, "DMAC", PCIPrgInfoDMAC },
	{ 0x02, "Timer", PCIPrgInfoTimer },
	{ 0x03, "RTC" ,  PCIPrgInfoRTC },
	{ 0x04, "PCI Hot-Plug Controller", NULL },
	{ 0x80, "Other", NULL },
			{ 0,0,0}
};
//---------------------------------------------Input -----------------------------------
struct PCIPrgInfo PCIPrgInfoGamePort[] =
{
	{ 0x0, "Generic" },
	{ 0x10, "Legacy" },
			{ 0,0}
};
struct PCISubClass PCISubClassInput[] =
{
	{ 0x00, "Keyboard" , NULL},
	{ 0x01, "Digitizer", NULL },
	{ 0x02, "Mouse" , NULL },
	{ 0x03, "Scanner" , NULL },
	{ 0x04, "Game Port" , PCIPrgInfoGamePort },
	{ 0x80, "Other" , NULL},
			{ 0,0,0}
};
//----------------------------------Docking Station-------------------
struct PCISubClass PCISubClassDockingStation[] =
{
	{ 0x00, "Generic", NULL },
	{ 0x80, "Other" , NULL},
			{ 0,0,0}
};
//-------------------------------------Processor ------------------

struct PCISubClass PCISubClassCPU[] =
{
	{ 0x00, "386", NULL },
	{ 0x01, "486", NULL },
	{ 0x02, "Pentium", NULL },
	{ 0x03, "P6", NULL },
	{ 0x10, "Alpha", NULL },
	{ 0x20, "Power PC", NULL },
	{ 0x30, "MIPS", NULL },
	{ 0x40, "Coprocessor", NULL },
	{ 0x80, "Other", NULL },
			{ 0,0,0}
};
// ------------------------------ Serial Bus ---------------------------

struct PCIPrgInfo PCIPrgInfoIEEE1394[] =
{
	{ 0x0, "FireWire" },
	{ 0x10, "OpenHCI" },
			{ 0,0}
};

struct PCIPrgInfo PCIPrgInfoUSB[] =
{
	{ 0x0, "Universal Host Controller" },
	{ 0x10, "Open Host Controller" },
	{ 0x80, "Non-specific Controller" },
	{ 0xFE, "Device" },
			{ 0,0}
};

struct PCISubClass PCISubClassSerialBus[] =
{
	{ 0x00, "IEEE 1394", PCIPrgInfoIEEE1394 },
	{ 0x01, "ACCESS.bus" , NULL  },
	{ 0x02, "SSA (Serial Storage Architecture)" , NULL },
	{ 0x03, "USB" ,NULL},
	{ 0x04, "Fiber Channel",NULL },
	{ 0x04, "SMBus",NULL },
	{ 0x80, "Other",NULL },
			{ 0,0,0}
};
// -----------------------------------Wireless --------------------------------
struct PCISubClass PCISubClassWireless[] =
{
	{ 0x00, "iRDA-Compatible Controller", NULL },
	{ 0x01, "Consumer IR Controller" , NULL  },
	{ 0x02, "RF Controller" , NULL },
	{ 0x80, "other",NULL },
			{ 0,0,0}
};

// -----------------------------------Intelligent I/O --------------------------------
struct PCISubClass PCISubClassI20[] =
{
	{ 0x00, "I20", NULL },
			{ 0,0,0}
};
// -----------------------------------Wireless --------------------------------
struct PCISubClass PCISubClassSatellite[] =
{
	{ 0x00, "TV", NULL },
	{ 0x01, "Audio" , NULL  },
	{ 0x02, "Voice" , NULL },
	{ 0x03, "Data" ,NULL },
			{ 0,0,0}
};
// -----------------------------------Encryption --------------------------------
struct PCISubClass PCISubClassEncryption[] =
{
	{ 0x00, "Network / Computing", NULL },
	{ 0x10, "Entertainment" , NULL  },
	{ 0x80, "Other" , NULL },
			{ 0,0,0}
};

// -----------------------------------Signal Processing--------------------------------
struct PCISubClass PCISubClassSignalProcessing[] =
{
	{ 0x00, "DPIO", NULL },
	{ 0x80, "Other", NULL },
			{ 0,0,0}
};


struct PCIBaseClass PCIClassDetails[] =
{
	{0x0, "Reserved", NULL },
	{0x1, "Disk", PCISubClassDisk },
	{0x2, "Network", PCISubClassNetwork },
	{0x3, "Display",	PCISubClassDisplay },
	{0x4, "Multimedia", PCISubClassMultimedia },
	{0x5, "Memory", PCISubClassMemory },
	{0x6, "Bridge", NULL },
	{0x7, "Communication", PCISubClassCommunication },
	{0x8, "System Peripheral", PCISubClassSystemPeripherals },
	{0x9, "Input", PCISubClassInput },
	{0xA, "Docking Station", PCISubClassDockingStation },
	{0xB, "CPU", PCISubClassCPU },
	{0xC, "Serial Bus", PCISubClassSerialBus },
	{0xD, "Wireless", PCISubClassWireless },
	{0xE, "Intelligent I/O", PCISubClassI20 },
	{0xF, "Satellite", PCISubClassSatellite },
	{0x10, "Encryption", PCISubClassEncryption },
	{0x11, "Signal Processing", PCISubClassSignalProcessing },
			{NULL, NULL, NULL}
};
/* This function fills the PCICfg structure using the configuration method 1
*/
extern "C" BYTE GetPCIConfigurationSpace(BYTE Bus, BYTE Device, BYTE Function, struct PCIConfigurationSpace * PCICfg)
{
	int Reg;
	int First = 1;
	for (Reg = 0; Reg < sizeof(struct PCIConfigurationSpace) / sizeof(DWORD); Reg++)
	{
		DWORD Addr, Value;
		Addr = 0x80000000L |
			(((DWORD)Bus) << 16) |
			(((DWORD)(Device << 3) | Function) << 8) |
			(Reg << 2);

		OutPortDWord(PCI_CONFIG_ADDRESS, Addr);

		Value = InPortDWord(PCI_CONFIG_DATA);

		Addr = InPortDWord(PCI_CONFIG_ADDRESS);
		((DWORD *)PCICfg)[Reg] = Value;
		OutPortDWord(PCI_CONFIG_ADDRESS, 0);
		if (First)
		{
			if (Value == 0xFFFFFFFF)
				return 0;
			First = 0;
		}
	}
	return Reg;
}
/* this function will initialize the _SysPCIList
Note you have to deallocate all the nodes before refreshing.
returns Total PCI Devices found on the system or 0 on error*/
list<struct PCIDeviceDetails *>* __SysPCIDevices;
extern "C" UINT16 InitPCIDevices()
{
	UINT16 TotDev = 0;
	BYTE Bus, Device, Function;
	struct PCIConfigurationSpace PCICfg;
	struct PCIDeviceDetails *newPCIDev;

	__SysPCIDevices = new list<struct PCIDeviceDetails *>();

	for (Bus = 0; Bus < 0xFF; Bus++)
		for (Device = 0; Device < 32; Device++)
			for (Function = 0; Function < 8; Function++)
			{
				BYTE Ret = GetPCIConfigurationSpace(Bus, Device, Function, &PCICfg);
				if (!Ret)
					continue;
				if (!(PCICfg.VendorID == 0xFFFF || PCICfg.DeviceID == 0xFFFFFFFF))
				{
					newPCIDev = new PCIDeviceDetails;
					newPCIDev->Bus = Bus;
					newPCIDev->Device = Device;
					newPCIDev->Function = Function;
					memcpy(&newPCIDev->PCIConfDetails, &PCICfg, sizeof(struct PCIConfigurationSpace));
					__SysPCIDevices->push_back(newPCIDev);
					TotDev++;
				}
			}
	return TotDev;
}

extern "C" void EnumeratePCIDevices(BYTE(*CallBackFn) (PCIConfigurationSpace *))
{
	list<struct PCIDeviceDetails *>::iterator iter = __SysPCIDevices->begin();
	for (; iter != __SysPCIDevices->end(); iter++)
	{
		PCIConfigurationSpace* pPCIDet = &(*iter)->PCIConfDetails;

		if (nullptr != pPCIDet)
		{			
			CallBackFn(pPCIDet);
		}
	}
}

#define __PCI_MaxClass 0x12
extern "C" BYTE PrintPCIDeviceList(struct PCIConfigurationSpace * ptrPCIDet)
{
	SkyConsole::Print("%x %x ", ptrPCIDet->VendorID, ptrPCIDet->DeviceID);
	BYTE SClCode = 0;
	if (ptrPCIDet->ClassCode < __PCI_MaxClass)
	{
		SkyConsole::Print(" %s", PCIClassDetails[ptrPCIDet->ClassCode].Description);
		if (ptrPCIDet->SubClass != 0)
		{
			struct PCISubClass * PCISubCl = PCIClassDetails[ptrPCIDet->ClassCode].SubClass;

			while (!(PCISubCl[SClCode].SubClassCode == 0 && PCISubCl[SClCode].Description == 0))
			{
				if (ptrPCIDet->SubClass == SClCode)

				{
					if (PCISubCl != NULL)
						SkyConsole::Print(" - %s\n", PCISubCl[SClCode].Description);
					else SkyConsole::Print("\n");
					break;
				}
				SClCode++;
			}
		}
		else SkyConsole::Print("\n");
	}

	return SClCode;
}

extern "C" void RequestPCIList()
{
	if (SystemProfiler::GetInstance()->GetGlobalState()._pciDevices > 0)
	{
		SkyConsole::Print("Device Vendor Class SubClass\n");
		EnumeratePCIDevices(PrintPCIDeviceList);
	}
	else
		SkyConsole::Print("Device not detected\n");
}