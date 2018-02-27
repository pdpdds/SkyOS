#include "GFS.h"
#include "string.h"
#include "ctype.h"
#include "Partition.h"
#include "SysInfo.h"
#include "SkyConsole.h"

#define FILESYSTEM_KEY_SIZE 20
#define MAX_DEVICE_PATH 100

//#define _DEBUG_

#define DEFINE_AND_GET_FS_FOR_FILE_PATH( lpFilePath )\
    LPFSHANDLE lpFS;\
    char szBuffer[MAX_DEVICE_PATH];\
    GetDevicePathFromFilePath( lpFilePath, szBuffer );\
    lpFS = (LPFSHANDLE)List_GetData( sysGFSDevicePathHandlers, (char *)szBuffer, -1);\
    if ( lpFS == NULL )\
    {\
		SkyConsole::Print("88888\n");\
		SetLastError(ERROR_INVALID_HANDLE);\
        return 0;\
    }
#define DEFINE_LPOPENEDFILEINFO_AND_CONVERT_FROM_HANDLE( hFile )\
    LPOPENEDFILEINFO lpOpenedFileInfo=(LPOPENEDFILEINFO)hFile;\
    if ( lpOpenedFileInfo == NULL )\
    {\
        return 0;\
    }\
    if( lpOpenedFileInfo->lpFS == NULL )\
    {\
		SetLastError(ERROR_INVALID_HANDLE);\
        return 0;\
    }

/*#define DEFINE_AND_GET_FS_FOR_FILE_PATH( lpFilePath )\
    LPFSHANDLE lpFS;\
    char szBuffer[MAX_DEVICE_PATH];\
    GetDevicePathFromFilePath( lpFilePath, szBuffer );\
    lpFS = List_GetData( sysGFSDevicePathHandlers, (char *)szBuffer, -1);\
    if ( lpFS == NULL )\
    {\
        dbgprintf("\nGFS: No FS for %s (%s)", lpFilePath, szBuffer);\
		SetLastError(ERROR_INVALID_HANDLE);\
        return 0;\
    }
#define DEFINE_LPOPENEDFILEINFO_AND_CONVERT_FROM_HANDLE( hFile )\
    LPOPENEDFILEINFO lpOpenedFileInfo=(LPOPENEDFILEINFO)hFile;\
    if ( lpOpenedFileInfo == NULL )\
    {\
        dbgprintf("\nGFS :: lpOpenedFileInfo is null for %X", hFile);\
        SetLastError(ERROR_INVALID_HANDLE);\
        return 0;\
    }\
    if( lpOpenedFileInfo->lpFS == NULL )\
    {\
        dbgprintf("\nGFS :: lpOpenedFileInfo->lpFS is null for %X", lpOpenedFileInfo->lpFS);\
        SetLastError(ERROR_INVALID_HANDLE);\
        return 0;\
    }*/
    
LPLISTNODE sysGFSHandlers=NULL;
LPLISTNODE sysGFSDevicePathHandlers=NULL;

LPLISTNODE sysGFSOpenedFiles=NULL; //this structure is just maintained to know the opened files

/* This function should to do some initialization code for GFS.
Currently this routines just sets the global variable to null and always returns sucess. should be removed in the version 3
*/
BYTE GFS_Init()
{
    sysGFSHandlers=NULL;
    sysGFSDevicePathHandlers=NULL;
    
    sysGFSOpenedFiles=NULL;
    return 1;
}
LPCSTR GetDevicePathFromFilePath(LPCSTR lpFilePath, LPSTR szBuffer)
{
    char DriveLetter = toupper(lpFilePath[0]);
    szBuffer[0]=0;
    
    if ( lpFilePath == NULL )
    {
        #ifdef _DEBUG_
            dbgprintf("\n\rGFS GetDevicePathFromFilePath() :: lpFilePath is NULL.");
        #endif
        return NULL;
    }
    if ( lpFilePath[0] == 0 )
    {
        #ifdef _DEBUG_
            dbgprintf("\n\rGFS GetDevicePathFromFilePath() :: lpFilePath is zero length string.");
        #endif
        return NULL;
    }
    if ( lpFilePath[1] == ':'  )
    {
        strcpy(szBuffer, sysDevicePathLookupTable[ DriveLetter - 'A' ].szDevicePath );
        return szBuffer;
    }
    if ( lpFilePath[0] == '\\' && lpFilePath[1] == '\\' )
    {
        int iSize=0;
        char * lpTemp = strchr( &lpFilePath[2], '\\');
        if ( lpTemp == NULL )
        {
            #ifdef _DEBUG_
                dbgprintf("\n\rGFS :: Invalid device path(1). %s is invalid", lpFilePath);
            #endif
            return NULL;
        }
        lpTemp = strchr( lpTemp+1, '\\');
        if ( lpTemp == NULL )
        {
            #ifdef _DEBUG_
                dbgprintf("\n\rGFS :: Invalid device path(2). %s is invalid", lpFilePath);
            #endif
            return NULL;
        }
        iSize = lpTemp-lpFilePath-2;
        if ( iSize > 0 )
        {
            strncpy( szBuffer, &lpFilePath[2], iSize);
            szBuffer[iSize]=0;
        }
        return szBuffer;
    }
    else
    {
        //this is for Virtual File system
        if ( stricmp(lpFilePath,"STDIN")==0 || stricmp(lpFilePath,"STDERR")==0  || stricmp(lpFilePath,"STDOUT")==0  )
        {
            strcpy( szBuffer, "VFS\\IORedirection" );
            return szBuffer;
        }
        
        #ifdef _DEBUG_
            dbgprintf("\n\rGFS :: File path should with a drive letter or \\\\. %s is invalid", lpFilePath);
        #endif
        return NULL;
    }
    
}
//------------------DLL Functions------------------------------

/*
This function registers a file system with kernel. The given filesystem must supply all the functions needed for generic file io.
*/
BOOLEAN GFS_Register(LPCSTR szFileSystemName, LPFSHANDLE lpFileSystem)
{
    char szKey[FILESYSTEM_KEY_SIZE];
    int i;
    /*check the FileSystemName for 
        1)invalid characters(currently \ should not present in the string 
        2)zero length
        3)Null value
    and also check whether the lpFileSystem is null or not
    */
    
    if ( strchr(szFileSystemName, '\\') || strlen(szFileSystemName) == 0 || szFileSystemName == NULL || lpFileSystem == NULL )
    {
        #ifdef _DEBUG_
            dbgprintf("GFS :: Bad parameter passed to GFS Register. Either szFileSystemName(%s) is invalid or lpFileSystem is NULL", szFileSystemName);
        #endif
      SetLastError( ERROR_BAD_FORMAT );
        return FALSE;
    }
    /*Now check whether the key contains any numeric character in it*/
    for( i=0; szFileSystemName[i]; i++)
    {
        if ( isdigit( szFileSystemName[i] ) )
        {
            #ifdef _DEBUG_
                dbgprintf("GFS :: Bad parameter passed to GFS. szFileSystemName (%s) contains a number", szFileSystemName);
            #endif
            SetLastError( ERROR_BAD_FORMAT );
            return FALSE;
        }
    }
    /*shorten the size of the key if it is larger than DEVICE_KEY_SIZE*/
    strncpy( szKey, szFileSystemName, FILESYSTEM_KEY_SIZE );
    szKey[FILESYSTEM_KEY_SIZE-1] = 0;
    
    if ( List_Add( &sysGFSHandlers, szKey, lpFileSystem ) != 0 )
    {
        #ifdef _DEBUG_
            dbgprintf("GFS :: Registration of the file system (%s) is failed", lpFileSystem);
        #endif
        //here the last error will be set by List_Add
        return FALSE;
    }
    SetLastError( ERROR_SUCCESS );
    return TRUE;
}
BOOLEAN GFS_RegisterForDevicePath(LPCSTR szDevicePath, LPFSHANDLE lpFileSystem)
{
    if ( strlen(szDevicePath) == 0 || szDevicePath == NULL || lpFileSystem == NULL )
    {
        #ifdef _DEBUG_
            dbgprintf("GFS :: Bad parameter passed to GFS Register. Either szFileSystemName(%s) is invalid or lpFileSystem is NULL", szDevicePath);
        #endif
        SetLastError( ERROR_BAD_FORMAT );
        return FALSE;
    }
    if ( List_Add( &sysGFSDevicePathHandlers, (char *)szDevicePath, lpFileSystem ) != 0 )
    {
        #ifdef _DEBUG_
            dbgprintf("GFS :: Registration of a file system for the Device Path(%s) is failed", szDevicePath);
        #endif
        //here the last error will be set by List_Add
        return FALSE;
    }
   SetLastError( ERROR_SUCCESS );
    return TRUE;
}

/*
This function callbacks the given function for each file system registered with the kernel
*/
void GFS_RegisteredFSTypes(BYTE (*fnFeedBack)(LPCSTR szFileSystemName, LPFSHANDLE hFileSystem))
{
    LPLISTNODE tmpNode;
    FOREACH(tmpNode, sysGFSHandlers)
    {
        fnFeedBack( tmpNode->szKey, (LPFSHANDLE)tmpNode->ptrData );
    }
}

/*
This function returns the total size of the given volume in a file system. You should specify the file system and 
volume no in the input
*/
UINT32 GFS_GetVolumeCapacity(LPCSTR lpDevicePath)
{
    DEFINE_AND_GET_FS_FOR_FILE_PATH( lpDevicePath );
        
    if ( lpFS->GetVolumeCapacity == NULL )
    {
        SetLastError(ERROR_NOT_SUPPORTED);
        return 0;
    }	
    return lpFS->GetVolumeCapacity(lpFS);
}
/*
This function returns the size of volume occupied by bad blocks in a file system. 
You should specify the file system and volume no in the input
*/
UINT32 GFS_GetVolumeBad(LPCSTR lpDevicePath)
{
    DEFINE_AND_GET_FS_FOR_FILE_PATH( lpDevicePath );
        
    if ( lpFS->GetVolumeBad == NULL )
    {
        SetLastError(ERROR_NOT_SUPPORTED);
        return 0;
    }	
    return lpFS->GetVolumeBad(lpFS);
}
/*
This function returns the free size of the given volume in a file system. 
You should specify the file system and volume no in the input
*/
UINT32 GFS_GetVolumeFree(LPCSTR lpDevicePath)
{
    DEFINE_AND_GET_FS_FOR_FILE_PATH( lpDevicePath );
        
    if ( lpFS->GetVolumeFree == NULL )
    {
       SetLastError(ERROR_NOT_SUPPORTED);
        return 0;
    }	
    return lpFS->GetVolumeFree(lpFS );
}
/*
This function calls the File System's Create Directory function. 
If CreateDirectory() function is not supported by the File System then this function Sets Last Error code to ERROR_NOT_SUPPORTED
If any error occured during the creation of directory then the File System should set the appropirate error code
*/
UINT32 GFS_CreateDirectory(LPCSTR szPath)
{
    DEFINE_AND_GET_FS_FOR_FILE_PATH( szPath );
        
    if ( lpFS->CreateDirectory == NULL )
    {
        SetLastError(ERROR_NOT_SUPPORTED);
        return 0;
    }	
    return lpFS->CreateDirectory(lpFS, szPath );
}
/*This function calls the File System's Create Directory function. 
If CreateDirectory() function is not supported by the File System then this function Sets Last Error code to ERROR_NOT_SUPPORTED
If any error occured during the creation of directory then the File System should set the appropirate error code
*/
UINT32 GFS_RemoveDirectory(LPCSTR szPath)
{
    DEFINE_AND_GET_FS_FOR_FILE_PATH( szPath );
        
    if ( lpFS->RemoveDirectory == NULL )
    {
        //SetLastError(ERROR_NOT_SUPPORTED);
        return 0;
    }	
    return lpFS->RemoveDirectory(lpFS, szPath );
}
/*
The following function gets the current directory for the specified volume for that process

UINT32 GFS_GetCurrentDirectory(DWORD nBufferLength,  LPTSTR lpBuffer)
{
    LPFSHANDLE_ID lpFSID;
    
    GFS_GET_FSID( _GetFSType(lpPath), lpFSID); 
        
    if ( lpFS->GetCurrentDirectory == NULL )
    {
        SetLastError(ERROR_NOT_SUPPORTED);
        return 0;
    }	
    return lpFS->GetCurrentDirectory( nBufferLength, lpBuffer );
}
UINT32 GFS_SetCurrentDirectory(LPTSTR lpDirPath)
{
    LPFSHANDLE_ID lpFSID;
    
    GFS_GET_FSID( _GetFSType(lpDirPath), lpFSID); 
        
    if ( lpFS->SetCurrentDirectory == NULL )
    {
        SetLastError(ERROR_NOT_SUPPORTED);
        return 0;
    }	
    return lpFS->SetCurrentDirectory( lpDirPath );
}
*/

/*This function Creates or Opens a file by calling the file systems CreateFile().
The file system should respond with the pointer to the newly created/opened files information. 
This information is given back to the file system during the other calls such as ReadFile(), WriteFile(), CloseFile() etc
Note :-If the file system returns NULL then it is interrupted as error
*/
HANDLE GFS_CreateFile(LPCTSTR lpFilePath, DWORD dwOpenMode, DWORD dwShareMode, DWORD dwCreationDisposition,DWORD dwFlags )
{
    LPOPENEDFILEINFO lpNewFileInfo;
    HANDLE hFile;
    
    #ifdef _DEBUG_
        dbgprintf("\n\rGFS :: CreateFile(%s) Called", lpFilePath);
    #endif
    
    DEFINE_AND_GET_FS_FOR_FILE_PATH( lpFilePath );
    
    if ( lpFS->CreateFile == NULL )      // check whether the function is supported
	{
		SkyConsole::Print("99999\n");
       SetLastError(ERROR_NOT_SUPPORTED);
        return 0;
    }
	
    //Execute the CreateFile() and store the result
    hFile=lpFS->CreateFile(lpFS, lpFilePath, dwOpenMode, dwShareMode, dwCreationDisposition, dwFlags);
    if ( hFile == NULL ) //if error occured during execution return
    {		
        #ifdef _DEBUG_
            dbgprintf("\n\t FS_CreateFile() returned NULL(%X)", hFile);
        #endif
        return 0;
    }
    
    //allocate OPENEDFILEINFO structure
    lpNewFileInfo = new OPENEDFILEINFO;
    if ( lpNewFileInfo == NULL )        //if we unable to allocate just return error. Todo :- Add Code to call file system's CloseFile()
    {
        #ifdef _DEBUG_
            dbgprintf("\n\t GFS_CreateFile() - Memory Allocation Failed");
        #endif
        SetLastError(ERROR_NOT_ENOUGH_MEMORY);
        return 0;
    }

	memset(lpNewFileInfo, 0, sizeof(OPENEDFILEINFO));
    
    lpNewFileInfo->dwOpenMode=dwOpenMode;
    lpNewFileInfo->dwShareMode=dwShareMode;
    lpNewFileInfo->lpFS=lpFS;
    lpNewFileInfo->lpFSData=hFile;
    
    List_Add(&sysGFSOpenedFiles,"",lpNewFileInfo);  //add to list. This list is maintained just for garbage collection
    
    /*-----file object management*/
    //TODOH :: check the file name
    lpNewFileInfo->lpObject = CreateObject( (char *)lpFilePath, lpNewFileInfo);
    if ( lpNewFileInfo->lpObject == NULL ) //allocation faile
    {
        //TODOL :: garbage collection
        return 0;
    }
    
    #ifdef _DEBUG_
        dbgprintf("\n\rGFS :: CreateFile() Succeded Returns %X", lpNewFileInfo);
    #endif
    return lpNewFileInfo;
}
void FreeOpenFileInfo(LPOPENEDFILEINFO lpOpenedFileInfo)
{
    delete lpOpenedFileInfo; //free the GFS area for opened file
    //to-do delete the file from GFS_FileOpen List
    //free the file number
}
/*
Closes the given file handle
*/
UINT32 GFS_CloseFile(HANDLE hFile)
{
    DEFINE_LPOPENEDFILEINFO_AND_CONVERT_FROM_HANDLE( hFile );
    
    if ( lpOpenedFileInfo->lpFS->CloseFile == NULL )      // check whether the function is supported
    {
        SetLastError(ERROR_NOT_SUPPORTED);
        return 0;
    }
    
    if ( lpOpenedFileInfo->lpFS->CloseFile(lpOpenedFileInfo->lpFS, lpOpenedFileInfo->lpFSData) )
        return 0; //error occured
    else
    {
        lpOpenedFileInfo->lpObject->dwSignals |= SIG_CLOSE;
        FreeOpenFileInfo(lpOpenedFileInfo);
        return 1;
    }
}
/*
Returns where the file ponter is
*/
UINT32 GFS_GetFilePointer(HANDLE hFile)
{
    DEFINE_LPOPENEDFILEINFO_AND_CONVERT_FROM_HANDLE( hFile );
    
    if ( lpOpenedFileInfo->lpFS->GetFilePointer == NULL )      // check whether the function is supported
    {
       SetLastError(ERROR_NOT_SUPPORTED);
        return 0;
    }
    
    return lpOpenedFileInfo->lpFS->GetFilePointer(lpOpenedFileInfo->lpFS, lpOpenedFileInfo->lpFSData);
}
/*
Sets the file pointer possition
*/
UINT32 GFS_SetFilePointer(HANDLE hFile, DWORD dwNewPos, DWORD dwMoveMethod)
{
    DEFINE_LPOPENEDFILEINFO_AND_CONVERT_FROM_HANDLE( hFile );
    
    if ( lpOpenedFileInfo->lpFS->SetFilePointer == NULL )      // check whether the function is supported
    {
        SetLastError(ERROR_NOT_SUPPORTED);
        return 0;
    }
    
    return lpOpenedFileInfo->lpFS->SetFilePointer(lpOpenedFileInfo->lpFS, lpOpenedFileInfo->lpFSData, dwNewPos, dwMoveMethod );
}
/*
Read the specified bytes from the file into the buffer
*/
UINT32 GFS_ReadFile(HANDLE hFile, UINT32 dwNoOfBytes, void * lpBuffer)
{
    DEFINE_LPOPENEDFILEINFO_AND_CONVERT_FROM_HANDLE( hFile );
    
    #ifdef _DEBUG_
        dbgprintf("\n\rGFS:: ReadFile(%X,%ld,%X) Called",hFile,dwNoOfBytes,lpBuffer);
    #endif		
    
    if ( lpOpenedFileInfo->lpFS->ReadFile == NULL )      // check whether the function is supported
    {
      SetLastError(ERROR_NOT_SUPPORTED);
        return 0;
    }
    #ifdef _DEBUG_
        dbgprintf("\n\rGFS:: ReadFile() end");
    #endif
    lpOpenedFileInfo->lpObject->dwSignals |= SIG_READ;
		
    return lpOpenedFileInfo->lpFS->ReadFile(lpOpenedFileInfo->lpFS, lpOpenedFileInfo->lpFSData, dwNoOfBytes, lpBuffer);
}
/*
Write no of bytes from the buffer into the file
*/
UINT32 GFS_WriteFile(HANDLE hFile, UINT32 dwNoOfBytes, void * lpBuffer)
{
    DEFINE_LPOPENEDFILEINFO_AND_CONVERT_FROM_HANDLE( hFile );
    if ( lpOpenedFileInfo->lpFS->WriteFile == NULL )      // check whether the function is supported
    {
       SetLastError(ERROR_NOT_SUPPORTED);
        return 0;
    }
    lpOpenedFileInfo->lpObject->dwSignals |= SIG_WRITE;
    return lpOpenedFileInfo->lpFS->WriteFile(lpOpenedFileInfo->lpFS, lpOpenedFileInfo->lpFSData, dwNoOfBytes, lpBuffer);
}
/*
Deletes the given file
*/
UINT32 GFS_DeleteFile(LPCTSTR lpFilePath)
{
    DEFINE_AND_GET_FS_FOR_FILE_PATH( lpFilePath );
    
    if ( lpFS->DeleteFile == NULL )      // check whether the function is supported
    {
       SetLastError(ERROR_NOT_SUPPORTED);
        return 0;
    }
	    
    return lpFS->DeleteFile(lpFS, lpFilePath );
}
/*
Returns the files attributes  in the buffer
*/
UINT32 GFS_GetFileAttributes(LPCTSTR lpFilePath, LPFILEINFO lpFINFO)
{
    DEFINE_AND_GET_FS_FOR_FILE_PATH( lpFilePath );
    
    if ( lpFS->GetFileAttributes == NULL )      // check whether the function is supported
    {
       SetLastError(ERROR_NOT_SUPPORTED);
        return 0;
    }
    
    return lpFS->GetFileAttributes(lpFS, lpFilePath, lpFINFO );
}
/*
Finds the first file in the specified file pattern and fills the file information in the given file info structure
Returns the handle for subsequent find next calls
*/
HANDLE GFS_FindFirstFile(LPCTSTR lpFilePath, LPFILEINFO lpFINFO)
{
    LPOPENEDFILEINFO lpNewFileInfo;
    HANDLE hFile;
    
    DEFINE_AND_GET_FS_FOR_FILE_PATH( lpFilePath );
        
    if ( lpFS->FindFirstFile == NULL )      // check whether the function is supported
    {
        #ifdef _DEBUG_
            dbgprintf("GFS :: FindFirstFile() Error - Not supports this function");
        #endif
       SetLastError(ERROR_NOT_SUPPORTED);
        return 0;
    }
    
    hFile = lpFS->FindFirstFile(lpFS, lpFilePath, lpFINFO );
    if ( hFile )
    {
        //allocate OPENEDFILEINFO structure
        lpNewFileInfo = new OPENEDFILEINFO;
        if ( lpNewFileInfo == NULL )        //if we unable to allocate just return error. Todo :- Add Code to call file system's CloseFile()
        {
            #ifdef _DEBUG_
                dbgprintf("GFS :: FindFirstFile() Error - Memory Allocation Failed");
            #endif
        SetLastError(ERROR_NOT_ENOUGH_MEMORY);
            return 0;
        }
        lpNewFileInfo->lpFS = lpFS;
        lpNewFileInfo->lpFSData = hFile;
        
        List_Add( &sysGFSOpenedFiles, "", lpNewFileInfo);  //add to list. This list is maintained just for garbage collection
        
        return lpNewFileInfo;
    }
    else
        return NULL;
}
/*
Continues search for the next file
*/
UINT32 GFS_FindNextFile(HANDLE hFile, LPFILEINFO lpFINFO )
{
    DEFINE_LPOPENEDFILEINFO_AND_CONVERT_FROM_HANDLE( hFile );
    
    if ( lpOpenedFileInfo->lpFS->FindNextFile == NULL )      
    {
       // SetLastError(ERROR_NOT_SUPPORTED);
        return 0;
    }
    
    return lpOpenedFileInfo->lpFS->FindNextFile(lpOpenedFileInfo->lpFS, lpOpenedFileInfo->lpFSData, lpFINFO );
}
UINT32 GFS_FindClose(HANDLE hFile)
{
    DEFINE_LPOPENEDFILEINFO_AND_CONVERT_FROM_HANDLE( hFile );
    
    if ( lpOpenedFileInfo->lpFS->FindClose == NULL )      
    {
       SetLastError(ERROR_NOT_SUPPORTED);
        return 0;
    }
    
    lpOpenedFileInfo->lpFS->FindClose(lpOpenedFileInfo->lpFS, lpOpenedFileInfo->lpFSData );
    FreeOpenFileInfo(lpOpenedFileInfo);
    return 1;
}

