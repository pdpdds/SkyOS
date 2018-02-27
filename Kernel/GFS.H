#pragma once
#include "windef.h"
#include "ACEList.h"
#include "ObjMan.h"

#define _MAX_PATH   260 /* max. length of full pathname */
#define _MAX_DRIVE  3   /* max. length of drive component */
#define _MAX_DIR    256 /* max. length of path component */
#define _MAX_FNAME  256 /* max. length of file name component */
#define _MAX_EXT    256 /* max. length of extension component */

#define GENERIC_READ            0x1
#define GENERIC_WRITE           0x2
#define FILE_SHARE_READ         0x4
#define FILE_SHARE_WRITE        0x8

#define CREATE_NEW              0x1
#define CREATE_ALWAYS           0x2
#define OPEN_EXISTING           0x4
#define OPEN_ALWAYS             0x8
#define TRUNCATE_EXISTING       0x10

#define FILE_ATTRIBUTE_READONLY 0x01
#define FILE_ATTRIBUTE_HIDDEN   0x02
#define FILE_ATTRIBUTE_SYSTEM   0x04
#define FILE_ATTRIBUTE_VOL_ID   0x08
#define FILE_ATTRIBUTE_DIR      0x10
#define FILE_ATTRIBUTE_ARCHIVE  0x20
#define FILE_ATTRIBUTE_LONG_NAME ( FILE_ATTRIBUTE_READONLY | FILE_ATTRIBUTE_HIDDEN | FILE_ATTRIBUTE_SYSTEM | FILE_ATTRIBUTE_VOL_ID )


#define FILE_BEGIN 1
#define FILE_CURRENT 2
#define FILE_END 3

#define SIG_TIMER       1
#define SIG_ABORT       2
#define SIG_DMA         4
#define SIG_READ        8
#define SIG_WRITE       16
#define SIG_OPEN        32
#define SIG_CLOSE       64
#define SIG_DELETE      128

#define FS_DESCRIPTION_SIZE 30

#ifndef _FILEINFO_DEFINED
    #define _FILEINFO_DEFINED
    typedef struct _FILETIME {
    DWORD dwLowDateTime;   /* low 32 bits  */
    DWORD dwHighDateTime;  /* high 32 bits */
    } FILETIME, *PFILETIME, *LPFILETIME;

    struct GFS_FileInfo
    {
        DWORD       dwFileAttributes; 
        FILETIME    ftCreationTime; 
        FILETIME    ftLastAccessTime; 
        FILETIME    ftLastWriteTime; 
        DWORD       nFileSizeHigh; 
        DWORD       nFileSizeLow; 
        DWORD       dwReserved0; 
        DWORD       dwReserved1; 
        CHAR        cFileName[ _MAX_PATH ]; 
        CHAR        cAlternateFileName[ 14 ]; 
        BYTE        Reserved[20];
    };
    typedef struct GFS_FileInfo FILEINFO;
    typedef FILEINFO * LPFILEINFO;
#endif

typedef struct FileSystemHandle FSHANDLE;
typedef FSHANDLE * LPFSHANDLE;
struct FileSystemHandle
{
    UINT32 (*GetVolumeCapacity)(LPFSHANDLE lpFS);
    UINT32 (*GetVolumeFree) (LPFSHANDLE lpFS);
    UINT32 (*GetVolumeBad)  (LPFSHANDLE lpFS);
    
    UINT32 (*CreateDirectory) (LPFSHANDLE lpFS, LPCSTR lpPathName);
    UINT32 (*RemoveDirectory) (LPFSHANDLE lpFS, LPCSTR lpPathName);
    
    UINT32 (*GetCurrentDirectory) (LPFSHANDLE lpFS, DWORD nBufferLength,  LPTSTR lpBuffer);
    UINT32 (*SetCurrentDirectory) (LPFSHANDLE lpFS, LPTSTR lpDirPath);
    
    HANDLE (*CreateFile)(LPFSHANDLE lpFS, LPCTSTR lpFilePath, DWORD dwOpenMode, DWORD dwShareMode, DWORD dwCreationDisposition, DWORD dwFlags );
    UINT32 (*CloseFile)(LPFSHANDLE lpFS, HANDLE hFile);
    UINT32 (*SetFilePointer)(LPFSHANDLE lpFS, HANDLE hFile, DWORD dwNewPos, DWORD dwMoveMethod);
    UINT32 (*GetFilePointer)(LPFSHANDLE lpFS, HANDLE hFile);
    UINT32 (*ReadFile)(LPFSHANDLE lpFS, HANDLE hFile, UINT32 dwNoOfBytes, void * lpBuffer);
    UINT32 (*WriteFile)(LPFSHANDLE lpFS, HANDLE hFile, UINT32 dwNoOfBytes, void * lpBuffer);

    UINT32 (*DeleteFile)(LPFSHANDLE lpFS, LPCTSTR lpFileName);
    UINT32 (*GetFileAttributes)(LPFSHANDLE lpFS, LPCTSTR lpFileName, LPFILEINFO lpFINFO);
    
    HANDLE (*FindFirstFile)(LPFSHANDLE lpFS, LPCTSTR lpFileName, LPFILEINFO lpFINFO);
    UINT32 (*FindNextFile)(LPFSHANDLE lpFS, HANDLE hFile, LPFILEINFO lpFINFO );
    UINT32 (*FindClose)(LPFSHANDLE lpFS, HANDLE hFile);

    
    UINT32 FSSpecificFields[10];
};

    
/* The following structure keeps the number and FS type of each opened file*/
struct GFS_OpenedFileInfo
{
    LPOBJECT lpObject;
    
    UINT32 dwFileNo;
    
    UINT32 dwShareMode;
    UINT32 dwOpenMode;

    LPFSHANDLE lpFS;
    void * lpFSData;
};
typedef struct GFS_OpenedFileInfo OPENEDFILEINFO;
typedef OPENEDFILEINFO * LPOPENEDFILEINFO;
    
#ifdef __cplusplus
    extern "C" {
#endif
extern LPLISTNODE sysGFSHandlers;
extern LPLISTNODE sysGFSDevicePathHandlers;
        
LPCSTR GetDevicePathFromFilePath(LPCSTR lpFilePath, LPSTR szBuffer);

BYTE GFS_Init();

BOOLEAN GFS_Register(LPCSTR szFileSystemName, LPFSHANDLE hFileSystem);
BOOLEAN GFS_RegisterForDevicePath(LPCSTR szDevicePath, LPFSHANDLE lpFileSystem);
void GFS_RegisteredFSTypes(BYTE (*fnFeedBack)(LPCSTR szFileSystemName, LPFSHANDLE hFileSystem));

UINT32 GFS_GetVolumeCapacity(LPCSTR lpDevicePath);
UINT32 GFS_GetVolumeFree (LPCSTR lpDevicePath);
UINT32 GFS_GetVolumeBad  (LPCSTR lpDevicePath);

UINT32 GFS_CreateDirectory(LPCSTR lpPath);
UINT32 GFS_RemoveDirectory(LPCSTR lpPath);
//UINT32 GFS_GetCurrentDirectory(DWORD nBufferLength,  LPTSTR lpBuffer);
//UINT32 GFS_SetCurrentDirectory(LPTSTR lpDirPath);
 
HANDLE GFS_CreateFile(LPCTSTR lpFilePath,DWORD dwOpenMode, DWORD dwShareMode, DWORD dwCreationDisposition,DWORD dwFlags );
UINT32 GFS_CloseFile(HANDLE hFile);
UINT32 GFS_SetFilePointer(HANDLE hFile, DWORD dwNewPos, DWORD dwMoveMethod);
UINT32 GFS_GetFilePointer(HANDLE hFile);
UINT32 GFS_ReadFile(HANDLE hFile, UINT32 dwNoOfBytes, void * lpBuffer);
UINT32 GFS_WriteFile(HANDLE hFile, UINT32 dwNoOfBytes, void * lpBuffer);

UINT32 GFS_DeleteFile(LPCTSTR lpFileName);
UINT32 GFS_GetFileAttributes(LPCTSTR lpFileName, LPFILEINFO lpFINFO);
 
HANDLE GFS_FindFirstFile(LPCTSTR lpFileName, LPFILEINFO lpFINFO);
UINT32 GFS_FindNextFile(HANDLE hFile, LPFILEINFO lpFINFO );
UINT32 GFS_FindClose(HANDLE hFile);
 
#ifdef __cplusplus
    }
#endif
