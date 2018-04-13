#pragma once
#include "windef.h"
#include "ACEList.h"

#define MAX_DEVICE_PATH 100

#define DEVICE_KEY_SIZE 20


#ifndef _GSH_IO_PARAMETER_DEFINED
    #define _GSH_IO_PARAMETER_DEFINED
#endif   //this structure is used by kernel32.h so modify it also if you do any changes here

#ifdef _MSC_VER
#pragma pack (push, 1)
#endif

    typedef struct tag_GSH_IO_Parameter
    {
        UINT32 dwCylinder;
        UINT32 dwHead;
        UINT32 dwSector;
    
        UINT32 dwLBASector;
    
        UINT32 dwSectorCount;
    
        BYTE   bMode;		//if bit 0 is set LBA mode
    
        UINT32 Reserved[20];
	}_GSH_IO_Parameter;

#ifdef _MSC_VER
#pragma pack (pop)
#endif
    
typedef _GSH_IO_Parameter GSHIOPARA;
typedef GSHIOPARA *LPGSHIOPARA;

struct GenericStorageHandle
{
    BYTE   (*GetNoOfDevices)();
    UINT32 (*GetDeviceParameters)(BYTE bDeviceNo, BYTE * lpBuffer);

    UINT32 (*Reset) (BYTE bDeviceNo);
    UINT32 (*Status)(BYTE bDeviceNo);

    UINT32 (*Verify)(BYTE bDeviceNo,  LPGSHIOPARA lpIOPara);
    UINT32 (*Read)  (BYTE bDeviceNo,  LPGSHIOPARA lpIOPara, BYTE * lpBuffer);
    UINT32 (*Write) (BYTE bDeviceNo,  LPGSHIOPARA lpIOPara, BYTE * lpBuffer);
      
    UINT32 DeviceSpecific[10]; 
};
typedef struct GenericStorageHandle GSH;
typedef GSH * LPGSH;
/* After creating the Handler you must Register your handler for one and only one
device type. For this you must call the Register() function in the [SystemStorageHandler] object

The following structure is used for internal storage(Collection).
*/
#ifdef __cplusplus
    extern "C" {
#endif

extern LPLISTNODE GSH_DeviceHandlers;
extern UINT16 GSH_LastErrorCode;
		
BYTE GSH_Init();
BOOLEAN GSH_Register(LPCSTR szDeviceKey, LPGSH lpGSH);
BYTE GSH_GetRegisteredDeviceTypes(BYTE (*fnFeedBack)(LPCSTR szDeviceKey));
BYTE GSH_GetNoOfDevices(LPCSTR szDeviceKey);
        
UINT32 GSH_GetDeviceParameters(LPCSTR szDevicePath, BYTE * lpBuffer);
UINT32 GSH_Reset(LPCSTR szDevicePath);
UINT32 GSH_Status(LPCSTR szDevicePath);

UINT32 GSH_Read(LPCSTR szDevicePath,  LPGSHIOPARA lpIOPara, BYTE * lpBuffer);
UINT32 GSH_Write(LPCSTR szDevicePath,  LPGSHIOPARA lpIOPara, BYTE * lpBuffer);
UINT32 GSH_Verify(LPCSTR szDevicePath,  LPGSHIOPARA lpIOPara);

#ifdef __cplusplus
    }
#endif

