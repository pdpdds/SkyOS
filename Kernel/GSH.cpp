/*
	GSH - Generic Storage Handler
    Date: 29-Nov-2001 
    Modified on 1-Jun-2003 10:33
        Converted C++ code to C Code
        Replaced Collection with List
*********this file should be modified as passing messages instead of direct function calling
*/

#define _DEBUG_

#include "GSH.h"
#include "string.h"
#include "ctype.h"
#include "SysInfo.h"

LPLISTNODE sysGSHDevices=NULL;

#define GET_LPGSH_FOR_DEVICE_PATH(szDevicePath, lpGSH) \
    char szKey[DEVICE_KEY_SIZE]; \
    ExtractDeviceKeyFromDevicePath(szDevicePath, szKey);\
    lpGSH = (LPGSH)List_GetData(sysGSHDevices, szKey, -1); \
    if ( lpGSH == NULL ) \
    {\
SetLastError( ERROR_NOT_SUPPORTED );\
        return 0;\
    }

/*#define GET_LPGSH_FOR_DEVICE_PATH(szDevicePath, lpGSH) \
    char szKey[DEVICE_KEY_SIZE]; \
    ExtractDeviceKeyFromDevicePath(szDevicePath, szKey);\
    lpGSH = (LPGSH)List_GetData(sysGSHDevices, szKey, -1); \
    if ( lpGSH == NULL ) \
    {\
        DEBUG_PRINT_OBJECT1("LPGSH is null for the Path/Key [%s]", szDevicePath);\
        SetLastError( ERROR_NOT_SUPPORTED );\
        return 0;\
    }*/
    
char * ExtractDeviceKeyFromDevicePath(const char * szDevicePath, char * szDeviceKey)
{
    int i;
    szDeviceKey[0]=0;
    for( i=0; szDevicePath[i] && i<=DEVICE_KEY_SIZE; i++ )
    {
        if ( szDevicePath[i] == 0 || szDevicePath[i] == '\\' || isdigit(szDevicePath[i]) )
            break;
        szDeviceKey[i] = szDevicePath[i];
    }
    szDeviceKey[i]=0;
    
    return szDeviceKey;
}
INT16 ExtractDeviceNoFromDevicePath(const char * szDevicePath)
{
    int i;
    for( i=0; !( szDevicePath[i] == 0 || szDevicePath[i] == '\\' || isdigit(szDevicePath[i]) ) ; i++ );
    if ( isdigit(szDevicePath[i]) )
        return szDevicePath[i]-'0';
    return -1;
}
BOOLEAN GSH_Register(LPCSTR szDeviceKey, LPGSH lpGSH)
{
    int i=0;
    char szKey[DEVICE_KEY_SIZE];
    /*check the DeviceKey for 
        1)invalid characters(currently \ should not present in the string 
        2)zero length
        3)Null value
    and also check whether the lpGSH is null or not
    */
    if ( strchr(szDeviceKey, '\\') || strlen(szDeviceKey) == 0 || szDeviceKey == NULL || lpGSH == NULL )
    {
        //DEBUG_PRINT_OBJECT1("Bad parameter passed to GSH. Either szDeviceKey(%s) is invalid or lpGSH is NULL", szDeviceKey);
        SetLastError( ERROR_BAD_FORMAT );
        return FALSE;
    }
    /*Now check whether the key contains any numeric character in it*/
    for( i=0; szDeviceKey[i]; i++)
    {
        if ( isdigit( szDeviceKey[i] ) )
        {
            //DEBUG_PRINT_OBJECT1("Bad parameter passed to GSH. DeviceKey (%s) contains a number", szDeviceKey);
            SetLastError( ERROR_BAD_FORMAT );
            return FALSE;
        }
    }
    /*shorten the size of the key if it is larger than DEVICE_KEY_SIZE*/
    strncpy( szKey, szDeviceKey, DEVICE_KEY_SIZE );
    szKey[DEVICE_KEY_SIZE-1] = 0;
    
    if ( List_Add( &sysGSHDevices, szKey, lpGSH ) != 0 )
    {
        //DEBUG_PRINT_OBJECT1("Registration of the device (%s) is failed", szDeviceKey);
        //here the last error will be set by List_Add
        return FALSE;
    }
    SetLastError( ERROR_SUCCESS );
    return TRUE;
}
/* the following function takes a functions address as input and
    enumerate all the device type registered to the _GSH
    Input : - Pointer to a call back function
                The feed back function must return a non-zero value to continue.
    Output : -
	  Returns number of devices enumerated
*/
BYTE GSH_GetRegisteredDeviceTypes(BYTE (*fnFeedBack)(LPCSTR szDeviceKey))
{
    LPLISTNODE tmpNode;
    int i=0;
    FOREACH(tmpNode, sysGSHDevices)
    {
        fnFeedBack( tmpNode->szKey );
        i++;
    }
    
    SetLastError(ERROR_SUCCESS);
    return i;
}
BYTE GSH_GetNoOfDevices(LPCSTR szDeviceKey)
{
    LPGSH lpGSH;
    
    GET_LPGSH_FOR_DEVICE_PATH(szDeviceKey, lpGSH);
    
    if ( lpGSH->GetNoOfDevices == NULL )
    {
        SetLastError(ERROR_NOT_SUPPORTED);
        return 0;
    }
    
    return lpGSH->GetNoOfDevices();
}
UINT32 GSH_GetDeviceParameters(LPCSTR szDevicePath, BYTE * lpBuffer)
{
    LPGSH lpGSH;
    
    GET_LPGSH_FOR_DEVICE_PATH(szDevicePath , lpGSH);
    
    if ( lpGSH->GetDeviceParameters == NULL )
    {
       SetLastError(ERROR_NOT_SUPPORTED);
        return 0;
    }
    
    return lpGSH->GetDeviceParameters(ExtractDeviceNoFromDevicePath(szDevicePath), lpBuffer);
}
UINT32 GSH_Reset(LPCSTR szDevicePath)
{
    LPGSH lpGSH;
    
    GET_LPGSH_FOR_DEVICE_PATH(szDevicePath, lpGSH);
    
    if ( lpGSH->GetDeviceParameters == NULL )
    {
        SetLastError(ERROR_NOT_SUPPORTED);
        return 0;
    }
    
    return lpGSH->Reset(ExtractDeviceNoFromDevicePath(szDevicePath));
}
UINT32 GSH_Status(LPCSTR szDevicePath)
{
    LPGSH lpGSH;
    
    GET_LPGSH_FOR_DEVICE_PATH(szDevicePath, lpGSH);
    
    if ( lpGSH->Status == NULL )
    {
        SetLastError(ERROR_NOT_SUPPORTED);
        return 0;
    }
    
    return lpGSH->Status(ExtractDeviceNoFromDevicePath(szDevicePath));
}
UINT32 GSH_Read(LPCSTR szDevicePath,  LPGSHIOPARA lpIOPara, BYTE * lpBuffer)
{
    LPGSH lpGSH;
    
    GET_LPGSH_FOR_DEVICE_PATH(szDevicePath, lpGSH);
    
    if ( lpGSH->Read == NULL )
    {
        SetLastError(ERROR_NOT_SUPPORTED);
        return 0;
    }
    
    return lpGSH->Read(ExtractDeviceNoFromDevicePath(szDevicePath), lpIOPara, lpBuffer );
}
UINT32 GSH_Write(LPCSTR szDevicePath,  LPGSHIOPARA lpIOPara, BYTE * lpBuffer)
{
    LPGSH lpGSH;
    
    GET_LPGSH_FOR_DEVICE_PATH(szDevicePath, lpGSH);
    
    if ( lpGSH->Write == NULL )
    {
        SetLastError(ERROR_NOT_SUPPORTED);
        return 0;
    }
    
    return lpGSH->Write(ExtractDeviceNoFromDevicePath(szDevicePath), lpIOPara, lpBuffer );
}
UINT32 GSH_Verify(LPCSTR szDevicePath,  LPGSHIOPARA lpIOPara)
{
    LPGSH lpGSH;
    
    GET_LPGSH_FOR_DEVICE_PATH(szDevicePath, lpGSH);
    
    if ( lpGSH->Verify == NULL )
    {
        SetLastError(ERROR_NOT_SUPPORTED);
        return 0;
    }
    
    return lpGSH->Verify(ExtractDeviceNoFromDevicePath(szDevicePath), lpIOPara );
}

