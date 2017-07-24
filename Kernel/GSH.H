/*
	GSH - Generic Storage Handler
		It is like device driver between the storage devices and
		system softwares.
Created By Sam ( 29-Nov-2001 )
	- Problems
		1) inline keyword is required for
		   accessing collections defined in a class
*/
#ifndef __GSH_H
#define __GSH_H

#include "windef.h"
#include "Collect.h"
/*

	Variable Prefiex
		DPF - Device Path Format in bytes
			The device type in first byte and the sub device identification
		in the following bytes.
		For Example :
			H0 - Means first harddisk in the system
			H3 - Means fourth harddisk in the system
		****
		   Donot confuse DPF with user interface paths. DPF must be interruptted
		   to user in different form. For Example, H0 - must be showed as
		   hdd1 or c: like that it may further divided into d:,e: etc

--------------------Specification for a Genernic Storage Handler--------------
	You must implement the following functions in your module and create
object for the GenericStorageHandler structure and fill the function pointers
with valid addresses.
	Any Extra calls to the handler must be called through the
called through (*SpecificHandler) pointer. It may be a object pointer. If it
does not support any functions then you can fill it as NULL.
*/
struct GenericStorageHandler
	{
	public:
		BYTE (*GetNoOfDevices)();
		UINT16 (*GetDeviceParameters)(BYTE * DPF, void * Buffer);

		UINT16 (*Reset) (BYTE * DPF);
		UINT16 (*Status)(BYTE * DPF);

		UINT16 (*Verify)(BYTE * DPF, void * InputParameter);
		UINT16 (*Read)  (BYTE * DPF, void * InputParameter, void * Buffer);
		UINT16 (*Write) (BYTE * DPF, void * InputParameter, void * Buffer);
		UINT16 (*Format)(BYTE * DPFDevice, void * InputParameter, void * Buffer);

		void * SpecificHandler; //pointer to a object
	};
/* After creating the Handler you must Register your handler for one and only one
device type. For this you must call the Register() function in the [SystemStorageHandler] object

The following structure is used for internal storage(Collection).
*/
struct __StorageHandlerID
	{
	BYTE DeviceType;
	char Description[11];
	void * Handler;
	};

struct _GSH_IO_Parameter
	{
	UINT32 Cylinder;
	UINT16 Head;
	UINT16 Sector;

	UINT32 LBASector;

	UINT16 SectorCount;

	BYTE   Mode;		//if bit 0 is set LBA mode

	UINT16 Others[20];
	};

/*There must be one and only one object for this class. If it is not present
then, no program can access the storage device. If duplicate(s) present, each have same
device specifications and therefore wasting memory and may also cause dead lock */

enum GSH_ERROR_CODES
	{GSH_NO_ERROR,
	GSH_HANDLER_NOT_INSTALLED
	};

class __GSH
	{
	private:
		Collection <__StorageHandlerID *> Handlers;
		UINT16 LastErrorCode;
	public:
		__GSH()
			{LastErrorCode=GSH_NO_ERROR;
			};
		~__GSH()
			{Handlers.Clear();
			}
		/*
		1) You must register a specific handler after initializing that handler
		2 )There is no provision for unregister a registered handler
			donot try to add code for modification and deletion because a system level
			modifications and deletions are not required.
		3) This call must be done by device drivers and the kernel.
		Input : - DeviceType - Must be Unique
			  Desciption - A Descriptive Message about the Device
				       Must be maxiumum of 10 characters long
				       if shorter pad with space
			  Handler    - void pointer pointing the device class
				       this class must implement GH class.
		Output : -
			  True - on success.    False - on failure.
		*/
		BOOLEAN Register(const BYTE DeviceType,const char * Description, void * Handler);

		/* the following function takes a functions address as input and
		enumerate all the device type registered to the _GSH
		Input : - Pointer to a call back function
			  The feed back function must return a non-zero value to continue.
		Output : -
			 Returns number of devices enumerated
		*/
		BYTE GetRegisteredDeviceTypes(BYTE (*FeedBack)(const BYTE DeviceType,const char * Description));

		BYTE GetNoOfDevices(const BYTE DeviceType);
		UINT16 GetDeviceParameters(BYTE * DPFDevice, BYTE * Buffer);
		UINT16 Reset(BYTE * DPFDevice);
		UINT16 Status(BYTE * DPFDevice);

		UINT16 Read(BYTE * DPFDevice, _GSH_IO_Parameter * InputParameter, BYTE * Buffer);
		UINT16 Write(BYTE * DPFDevice, _GSH_IO_Parameter * InputParameter, BYTE * Buffer);
		UINT16 Verify(BYTE * DPFDevice, _GSH_IO_Parameter * InputParameter);
		UINT16 Format(BYTE * DPFDevice, _GSH_IO_Parameter * InputParameter, BYTE * Buffer);

		/* For Device Specific functions get the handler and call directly */
		void * GetHandler(const BYTE DeviceType);

		/* Returns the last error code. It contains the error occured
		at the GSH Level for extract error code for a particular device
		use the device handler */
		UINT32 GetLastErrorCode()
			{return LastErrorCode;
			}
		/* This function returns the string describing the ErrorCode*/
		char * GetErrorDescription(int ErrCode);
		char * GetErrorDescription()
			{return GetErrorDescription(LastErrorCode);
			}
		};
#endif 
