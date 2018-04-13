#include "GFS.h"
#include "GSH.h"
#include "FAT32.h"
#include "ctype.h"
#include "string.h"
#include "memory.h"
#include "SysInfo.h"
#include "Util.h"
#include "SkyConsole.h"

LPLISTNODE sysFATOpenFileInfo=NULL;


#define FILL_FS_STRUCTURE( lpNewFS )\
    lpNewFS->GetVolumeCapacity=FAT_GetVolumeCapacity;\
    lpNewFS->GetVolumeFree=FAT_GetVolumeFree;\
    lpNewFS->GetVolumeBad=FAT_GetVolumeBad;\
    \
    lpNewFS->CreateDirectory=FAT_CreateDirectory;\
    lpNewFS->RemoveDirectory=FAT_RemoveDirectory;\
    \
    lpNewFS->GetCurrentDirectory=FAT_GetCurrentDirectory;\
    lpNewFS->SetCurrentDirectory=FAT_SetCurrentDirectory;\
    \
    lpNewFS->CreateFile=FAT_CreateFile;\
    lpNewFS->CloseFile=FAT_CloseFile;\
    lpNewFS->SetFilePointer=FAT_SetFilePointer;\
    lpNewFS->GetFilePointer=FAT_GetFilePointer;\
    lpNewFS->ReadFile=FAT_ReadFile;\
    lpNewFS->WriteFile=FAT_WriteFile;\
    \
    lpNewFS->DeleteFile=FAT_DeleteFile;\
    lpNewFS->GetFileAttributes=FAT_GetFileAttributes;\
    \
    lpNewFS->FindFirstFile=FAT_FindFirstFile;\
    lpNewFS->FindNextFile=FAT_FindNextFile;\
    lpNewFS->FindClose=FAT_FindClose;

/*-----------------------------------------------------------------------------------------
    FAT Internal Functions - These functions are work on internal FAT data structures
-----------------------------------------------------------------------------------------*/
/*
This function intializes the FAT structures, by doing some calculations on Partion Info and First Sector
INPUT:- 1) lpFATDetails - FAT Structure to be filled
        2) szDevicePath - Device which contains the FAT volume such as H0, F0 ..
        3) lpPart       - Partition Information for that volume
        4) pFirstSector - First Sector of the FAT Volume

Call it before adding FAT to the sysFATInternals list
*/
void FAT_Initialize(LPFAT lpFATDetails, LPCTSTR szDevicePath, LPPARTITION lpPart, BYTE * pFirstSector)
{
    memcpy(&lpFATDetails->Part,lpPart,sizeof(PARTITION)); //copy the partiton info into the structure
    if ( strlen(szDevicePath) >= FAT_DPF_SIZE ) //if length of device path is >=FAT_DPF_SIZE then return
    {
        //DEBUG_PRINT_INFO(" Initialization Error :: DPF Key > FAT_DPF_SIZE");
        return;
    }
    strcpy((char*)lpFATDetails->szDevicePath, szDevicePath );             //copy the Device Path
    
    //copy the boot sector information into appropriate data structures
    memcpy( (BYTE *)&lpFATDetails->FDI._BS , (BYTE *) &pFirstSector[0], sizeof(struct BootSectorStart) );
    memcpy( (BYTE *)&lpFATDetails->FDI._BPB, (BYTE *) &pFirstSector[11], sizeof(struct BPB) );
    memcpy( (BYTE *)&lpFATDetails->FDI._FAT12_16 ,(BYTE *) &pFirstSector[36], sizeof(struct FAT12_16) );
    memcpy( (BYTE *)&lpFATDetails->FDI._FAT32    ,(BYTE *) &pFirstSector[36], sizeof(struct FAT32) );

    //Calculate FATSize, TotRootDirSectors, Data Sectors and CountOfClusters
    lpFATDetails->FDI.dwFATSize = lpFATDetails->FDI._BPB.wFATSize16 ? lpFATDetails->FDI._BPB.wFATSize16 : lpFATDetails->FDI._FAT32.dwFATSize32;
    lpFATDetails->FDI.dwTotRootDirSectors=( ((lpFATDetails->FDI._BPB.wRootDirEntries*32) + (lpFATDetails->FDI._BPB.wBytesPerSector-1)) / lpFATDetails->FDI._BPB.wBytesPerSector );
    lpFATDetails->FDI.dwDataSectors=(lpFATDetails->FDI._BPB.wTotalSectors16?lpFATDetails->FDI._BPB.wTotalSectors16:lpFATDetails->FDI._BPB.dwTotalSectors32)-( lpFATDetails->FDI._BPB.wReservedSectors + ( lpFATDetails->FDI._BPB.bNumberFATs* lpFATDetails->FDI.dwFATSize ) + lpFATDetails->FDI.dwTotRootDirSectors );
    lpFATDetails->FDI.dwCountOfClusters=lpFATDetails->FDI.dwDataSectors/lpFATDetails->FDI._BPB.bSectorsPerCluster;
    //Determine the FAT Type
    if ( lpFATDetails->FDI.dwCountOfClusters < 4085 )
        lpFATDetails->FDI.bFATType=12;
    else
	if ( lpFATDetails->FDI.dwCountOfClusters < 65525 )
            lpFATDetails->FDI.bFATType=16;
	else
            lpFATDetails->FDI.bFATType=32;
}
/*
This function returns the Logical Sector number for the given Sector in the Cluster
This should be used in GSH_Read() and GSH_Write()
*/
inline UINT32 FAT_GetLogicalSector(LPFAT lpFAT, UINT32 dwCluster,UINT16 dwSector)
{
    if ( dwSector >=lpFAT->FDI._BPB.bSectorsPerCluster )
	    return 0;
    return lpFAT->FDI._BPB.dwHiddenSectors+lpFAT->FDI._BPB.wReservedSectors+(lpFAT->FDI._BPB.bNumberFATs*lpFAT->FDI.dwFATSize)+ lpFAT->FDI.dwTotRootDirSectors + ((dwCluster-2) * lpFAT->FDI._BPB.bSectorsPerCluster) + dwSector;
}
/*Returns the Cluster number for a given Logical Sector*/
inline UINT32 FAT_GetClusterCluster(LPFAT lpFAT, UINT32 dwLogicalSector)
{
    UINT32 dwResult;
    dwResult=( dwLogicalSector-(lpFAT->FDI._BPB.dwHiddenSectors+lpFAT->FDI._BPB.wReservedSectors+(lpFAT->FDI._BPB.bNumberFATs*lpFAT->FDI.dwFATSize)+lpFAT->FDI.dwTotRootDirSectors) ) / lpFAT->FDI._BPB.bSectorsPerCluster ;
    dwResult=((int)dwResult<0?0:dwResult)+2;
    return dwResult;
}
/* this function will return the FAT Sector which refers the given Cluster*/
inline UINT32 FAT_FATSectorNumber(LPFAT lpFAT, UINT32 dwCluster)
{
    UINT32 FATOffset = 0;
    if ( lpFAT->FDI.bFATType == 12 )
        FATOffset=dwCluster + ( dwCluster / 2 );
    else
	    if ( lpFAT->FDI.bFATType == 16 )
            FATOffset=dwCluster*2;
        else
            if ( lpFAT->FDI.bFATType == 32 )
		        FATOffset=dwCluster*4;
	return lpFAT->FDI._BPB.wReservedSectors + ( FATOffset / lpFAT->FDI._BPB.wBytesPerSector ) ;
}
/* this function will returns the BYTE offset from FAT Sector which refers the given Cluster*/
inline UINT32 FAT_FATSectorOffset(LPFAT lpFAT, UINT32 dwCluster )
{
    UINT32 dwFATOffset = 0;;
    if ( lpFAT->FDI.bFATType == 12 )
	    dwFATOffset=dwCluster + ( dwCluster / 2 );
    else
	    if ( lpFAT->FDI.bFATType == 16 )
            dwFATOffset=dwCluster*2;
        else
            if ( lpFAT->FDI.bFATType == 32 )
		        dwFATOffset=dwCluster*4;
    return dwFATOffset % lpFAT->FDI._BPB.wBytesPerSector;
}

/* this function returns the logical sector in the data area*/
inline UINT32 FAT_FirstSectorOfCluster(LPFAT lpFATDetails, UINT32 Cluster)
{
    UINT32 dwDataAreaStartSector=lpFATDetails->FDI._BPB.wReservedSectors + (lpFATDetails->FDI._BPB.bNumberFATs * lpFATDetails->FDI.dwFATSize) + lpFATDetails->FDI.dwTotRootDirSectors;
    return ((Cluster-2)*lpFATDetails->FDI._BPB.bSectorsPerCluster) + dwDataAreaStartSector;
}
inline UINT16 FAT_RootDirStartSector(LPFAT lpFAT)
{
    if ( lpFAT->FDI.bFATType == 12 || lpFAT->FDI.bFATType == 16 )
	    return lpFAT->FDI._BPB.wReservedSectors+( lpFAT->FDI._BPB.bNumberFATs * lpFAT->FDI._BPB.wFATSize16 ) ;
    else
    	return FAT_GetLogicalSector(lpFAT, lpFAT->FDI._FAT32.dwRootCluster,0);
}
inline UINT16 FAT_RootDirStartCluster(LPFAT lpFAT)
{
    if ( lpFAT->FDI.bFATType == 12 || lpFAT->FDI.bFATType == 16 )
	    return FAT_GetClusterCluster(lpFAT, lpFAT->FDI._BPB.wReservedSectors+( lpFAT->FDI._BPB.bNumberFATs * lpFAT->FDI._BPB.wFATSize16 )) ;
    else
	    return lpFAT->FDI._FAT32.dwRootCluster;
}
// ******** you must read the sector and pass it to this function
// to read the sector use FATSectorNumber function
// This is not the Data Area Sector, it is the FAT Area
inline UINT32 FAT_GetFATEntry(LPFAT lpFAT, UINT32 dwCluster , BYTE * pSector )
{
    UINT16 dwFATOffset=FAT_FATSectorOffset(lpFAT, dwCluster);
    if ( lpFAT->FDI.bFATType == 12 )
    {
        UINT16 Result=*( (WORD *) &pSector[dwFATOffset]);
	    if ( dwCluster & 0x0001 )  //Odd cluster
            return Result>>4;
        else
            return Result & 0x0FFF;;
    }
    else
    {
	    if( lpFAT->FDI.bFATType == 16  )
            return * ((WORD *) &pSector[dwFATOffset] );
        else
            if(  lpFAT->FDI.bFATType == 32 )
		    return (* ((DWORD *) &pSector[dwFATOffset] )) & 0xFFFFFFF; //only 28 bits is valid
    }
    return 0; ///unknown FAT Format
}
/*Checks whether the given FATEntry is End or not*/
BYTE FAT_IsEndOfClusterChain(LPFAT lpFAT, UINT32 dwFATContent)
{
    switch( lpFAT->FDI.bFATType )
    {
        case 12:        return ((UINT16)dwFATContent)>=0x0FF8?TRUE:FALSE;
        case 16:        return ((UINT16)dwFATContent)>=0xFFF8?TRUE:FALSE;
        case 32:        return dwFATContent>=0x0FFFFFF8?TRUE:FALSE;
        default:        return TRUE;
    }
}
/*Checks whether the given FATEntry is Bad Cluster or not*/
BYTE FAT_IsBadCluster(LPFAT lpFAT, UINT32 dwFATContent)
{
    switch( lpFAT->FDI.bFATType )
    {
        case 12:        return ((UINT16)dwFATContent)==0x0FF7?TRUE:FALSE;
        case 16:        return ((UINT16)dwFATContent)==0xFFF7?TRUE:FALSE;
        case 32:        return dwFATContent==0x0FFFFFF7?TRUE:FALSE;
        default:        return 0;
    }
}

/*--------------------------------------------------------------------------------------------------------------------------------
The following functions are GFS implementation of FAT. When the GFS interface/definition is defined, all changes also should reflect here.
--------------------------------------------------------------------------------------------------------------------------------*/
UINT32 FAT_GetVolumeCapacity(LPFSHANDLE lpFS)
{
    //DEBUG_PRINT_INFO("Not Implemented");
    SetLastError(ERROR_NOT_SUPPORTED);
    return 0;
}
UINT32 FAT_GetVolumeFree(LPFSHANDLE lpFS)
{
    //DEBUG_PRINT_INFO("Not Implemented");
    SetLastError(ERROR_NOT_SUPPORTED);
    return 0;
}
UINT32 FAT_GetVolumeBad(LPFSHANDLE lpFS)
{
    //DEBUG_PRINT_INFO("Not Implemented");
    SetLastError(ERROR_NOT_SUPPORTED);
    return 0;
}

UINT32 FAT_CreateDirectory(LPFSHANDLE lpFS, LPCSTR lpPathName)
{
    //DEBUG_PRINT_INFO("Not Implemented");
    SetLastError(ERROR_NOT_SUPPORTED);
    return 0;
}
UINT32 FAT_RemoveDirectory(LPFSHANDLE lpFS, LPCSTR lpPathName)
{
    return DeleteFileEntry( lpFS, lpPathName );
}
    
UINT32 FAT_GetCurrentDirectory(LPFSHANDLE lpFS, DWORD nBufferLength,  LPTSTR lpBuffer)
{
    //DEBUG_PRINT_INFO("Not Implemented");
    SetLastError(ERROR_NOT_SUPPORTED);
    return 0;
}
UINT32 FAT_SetCurrentDirectory(LPFSHANDLE lpFS, LPTSTR lpDirPath)
{
    //DEBUG_PRINT_INFO("Not Implemented");
    SetLastError(ERROR_NOT_SUPPORTED);
    return 0;
}
    
HANDLE FAT_CreateFile(LPFSHANDLE lpFS, LPCTSTR lpFilePath, DWORD dwOpenMode, DWORD dwShareMode, DWORD dwCreationDisposition, DWORD dwFlags )
{
    DIRENTRY DEInfo;
    LPDIRENTRY lpResult;
    char szFileName[_MAX_FNAME], szExtension[_MAX_EXT], szDirectory[_MAX_DIR], szDrive[_MAX_DRIVE];
 
    //DEBUG_PRINT_INFO(lpFilePath);
    
    SplitPath(lpFilePath, szDrive, szDirectory, szFileName, szExtension);
    
    lpResult = GetDirectoryEntry((LPFAT) lpFS->FSSpecificFields[0], (char *)lpFilePath, &DEInfo);
    
    //DEBUG_PRINT_OBJECT1("Directory Entry %X", lpResult);
    
    if ( (dwCreationDisposition & OPEN_EXISTING)  && lpResult == NULL ) //if opening an existing file, the file must exists
    {
        if ( dwCreationDisposition & OPEN_ALWAYS ) // if open always is set then set the create new flag and break else return error
            dwCreationDisposition|=CREATE_NEW;
        else
        {
            //DEBUG_PRINT_INFO("OPEN_EXISTING - But file not found ( !OPEN_ALWAYS )");
            SetLastError(ERROR_FILE_NOT_FOUND); //file not found
            return 0;
        }
    }
    if ( (dwCreationDisposition & CREATE_NEW) && lpResult ) //if create new is specified then the file should not exist
    {
        if ( dwCreationDisposition & CREATE_ALWAYS )
        {
            if ( FAT_DeleteFile(lpFS, lpFilePath) == 0 )
            {
                //DEBUG_PRINT_INFO("CREATE_NEW & CREATE_ALWAYS - File exists but unable to delete");
                return 0; //deletion error. SetLastError() is called by the FAT_DeleteFile() itself
            }
            else
                lpResult=NULL; //since the file is deleted
        }
        else
        {
            //DEBUG_PRINT_INFO("CREATE_NEW - File already exists ( !CREATE_ALWAYS )");
            SetLastError(ERROR_ALREADY_EXISTS); //File already exists
            return 0;
        }
    }
    if ( lpResult == NULL )     //Create New file
    {
        //DEBUG_PRINT_INFO("Create new file not implemented");
        return 0;
    }
    else        //open existing file
    {
		LPFATOPENEDFILEINFO lpFileInfo = new FATOPENEDFILEINFO;
        
        lpFileInfo->lpFATInfo = (LPFAT) lpFS->FSSpecificFields[0];
        if ( lpFileInfo->lpFATInfo == NULL )
        {
           // DEBUG_PRINT_INFO("lpFATInfo is NULL returns 0");
            return 0;
        }

		lpFileInfo->lpBuffer = new BYTE[lpFileInfo->lpFATInfo->FDI._BPB.bSectorsPerCluster * lpFileInfo->lpFATInfo->FDI._BPB.wBytesPerSector];
        
        lpFileInfo->bMode=dwOpenMode;
        memcpy(&lpFileInfo->DEInfo, &DEInfo, sizeof(DIRENTRY));
        lpFileInfo->dwCurrentCluster = ConvertWordsToDoubleWord( DEInfo.wFirstClusterHigh, DEInfo.wFirstClusterLow);
        lpFileInfo->wBufferOffset = 0;
        lpFileInfo->bBufferIsValid = FALSE;

        lpFileInfo->bDriveLetter = szDrive[0];
        lpFileInfo->dwBufferedFATSector = 0;
        lpFileInfo->dwTotalBytesPassed = 0;

		SkyConsole::Print("sdfs %d\n", lpFileInfo->dwTotalBytesPassed);
        
        if ( List_Add(&sysFATOpenFileInfo, "", lpFileInfo)==0 )
        {
            //DEBUG_PRINT_OBJECT1("Returns %X", lpFileInfo);
            return lpFileInfo;
        }
        else
        {
            //DEBUG_PRINT_INFO("List Add Failed");
            return 0;
        }
    }
  
}
UINT32 FAT_CloseFile(LPFSHANDLE lpFS, HANDLE hFile)
{
    SetLastError(ERROR_NOT_SUPPORTED);
    return 0;
}
UINT32 FAT_SetFilePointer(LPFSHANDLE lpFS, HANDLE hFile, DWORD dwNewPos, DWORD dwMoveMethod)
{
    LPFATOPENEDFILEINFO lpFileInfo = (LPFATOPENEDFILEINFO) hFile;
    //LPFAT lpFATInfo = lpFileInfo->lpFATInfo;
    DWORD dwFrom = lpFileInfo->dwTotalBytesPassed;
    DWORD dwTo = 0;
    switch( dwMoveMethod )
    {
        case FILE_BEGIN:
            dwTo = dwNewPos;
            break;
        case FILE_CURRENT:
            dwFrom = dwFrom + dwNewPos;
            break;
        case FILE_END:
            dwTo = lpFileInfo->DEInfo.dwFileSize;
            break;
    }
    while ( dwFrom < dwTo )
    {
        DWORD dwResult = MoveFilePointerToNextCluster(lpFileInfo, FALSE );
        if ( dwResult == 0 )
            return 0;
        dwFrom += dwResult;
    }
    return dwTo;
}
/*
this function moves the file pointer and optionally fills the buffer
returns the total bytes read
*/
UINT32 MoveFilePointerToNextCluster(LPFATOPENEDFILEINFO lpFileInfo, BOOLEAN bFillBuffer )
{
    LPFAT lpFATInfo = lpFileInfo->lpFATInfo;
    DWORD dwResult=0;
    if ( ! lpFileInfo->dwCurrentCluster || FAT_IsBadCluster(lpFATInfo,lpFileInfo->dwCurrentCluster) )
    {
        //DEBUG_PRINT_INFO("Requested to move file pointer in Bad or Free Cluster");
        return 0;
    }
    if  ( !FAT_IsEndOfClusterChain(lpFATInfo,lpFileInfo->dwCurrentCluster)  )
    {
        dwResult += lpFATInfo->FDI._BPB.bSectorsPerCluster * lpFATInfo->FDI._BPB.wBytesPerSector;
        if ( bFillBuffer )
        {
            ReadGSHDevice((const char*)lpFATInfo->szDevicePath , PART_BEGINING(&lpFATInfo->Part) + FAT_FirstSectorOfCluster(lpFATInfo,lpFileInfo->dwCurrentCluster), lpFATInfo->FDI._BPB.bSectorsPerCluster, (BYTE *)lpFileInfo->lpBuffer);
            if ( GetLastError() )
            {
                //DEBUG_PRINT_INFO("GSH read error while reading FAT area");
                return 0;
            }
        }
        UINT32 dwFATSector = PART_BEGINING(&lpFATInfo->Part) + FAT_FATSectorNumber(lpFATInfo, lpFileInfo->dwCurrentCluster);
        if ( lpFileInfo->dwBufferedFATSector != dwFATSector )
        {
            ReadGSHDevice((const char*)lpFATInfo->szDevicePath, dwFATSector, 2, (BYTE *)lpFileInfo->lpSectorBuffer);
            if ( GetLastError() )
            {
                //DEBUG_PRINT_INFO("GSH read error while reading data area");
                return 0;
            }
            lpFileInfo->dwBufferedFATSector=dwFATSector;
        }
        lpFileInfo->dwCurrentCluster = FAT_GetFATEntry(lpFATInfo,lpFileInfo->dwCurrentCluster, lpFileInfo->lpSectorBuffer);
        lpFileInfo->wBufferOffset=0;
        lpFileInfo->bBufferIsValid=bFillBuffer;
        lpFileInfo->dwTotalBytesPassed+=lpFATInfo->FDI._BPB.bSectorsPerCluster * lpFATInfo->FDI._BPB.wBytesPerSector;
    }	
	
    return dwResult;
}



UINT32 FAT_GetFilePointer(LPFSHANDLE lpFS, HANDLE hFile)
{
    LPFATOPENEDFILEINFO lpFileInfo = (LPFATOPENEDFILEINFO) hFile;
    return lpFileInfo->dwTotalBytesPassed;
    //SetLastError(ERROR_NOT_SUPPORTED);
    //return 0;
}

UINT32 FAT_ReadFile(LPFSHANDLE lpFS, HANDLE hFile, UINT32 dwNoOfBytes, void * lpBuffer)
{
	

    UINT32 dwTotReadBytes=0, dwBufferTotalBytes=0;
    BYTE * pBuffer;
    UINT16 wBufferSize;
    LPFATOPENEDFILEINFO lpFileInfo;
    LPFAT pFATInfo;
    
    //DEBUG_PRINT_OBJECT3("(%X,%ld,%X)",hFile, dwNoOfBytes, lpBuffer );
    pBuffer=(BYTE *) lpBuffer;
    
    lpFileInfo = (LPFATOPENEDFILEINFO) hFile;
    if ( lpFileInfo == NULL )
    {
       // DEBUG_PRINT_INFO("File Not Opened");
        return 0;
    }
	
    pFATInfo = lpFileInfo->lpFATInfo;
	    	
    //now check the internal buffer for the required bytes of data if exists copy it 
    wBufferSize = pFATInfo->FDI._BPB.bSectorsPerCluster*pFATInfo->FDI._BPB.wBytesPerSector;
	
    if ( lpFileInfo->bBufferIsValid  )
    {
        dwBufferTotalBytes = wBufferSize - lpFileInfo->wBufferOffset;
        if ( dwBufferTotalBytes >= dwNoOfBytes )
            dwBufferTotalBytes = dwNoOfBytes;
        if ( lpFileInfo->dwTotalBytesPassed+dwBufferTotalBytes > lpFileInfo->DEInfo.dwFileSize )
            dwBufferTotalBytes = lpFileInfo->DEInfo.dwFileSize-lpFileInfo->dwTotalBytesPassed;
        //copy the contents to callee
        memcpy(pBuffer, &lpFileInfo->lpBuffer[lpFileInfo->wBufferOffset],dwBufferTotalBytes);
        dwTotReadBytes = dwBufferTotalBytes;
        lpFileInfo->dwTotalBytesPassed += dwBufferTotalBytes;
        lpFileInfo->wBufferOffset += dwBufferTotalBytes;
        if ( lpFileInfo->wBufferOffset >= wBufferSize )
            lpFileInfo->bBufferIsValid = FALSE;
    }

    //read into internal buffer and copy it
    while ( dwTotReadBytes < dwNoOfBytes )
    {		
		
        UINT32 dwBufferBytes=wBufferSize;
        //read contents to buffer
        if ( ! MoveFilePointerToNextCluster(lpFileInfo, TRUE ) )
            return dwTotReadBytes;

        //adjustment for read buffer
        if ( dwTotReadBytes+dwBufferBytes > dwNoOfBytes )
            dwBufferBytes=dwNoOfBytes-dwTotReadBytes;
		
		SkyConsole::Print("%d, %d %d %d\n", dwTotReadBytes, lpFileInfo->DEInfo.dwFileSize, lpFileInfo->dwTotalBytesPassed, dwBufferBytes);

        if ( lpFileInfo->dwTotalBytesPassed+dwBufferBytes > lpFileInfo->DEInfo.dwFileSize )
        {			
			if (lpFileInfo->dwTotalBytesPassed > lpFileInfo->DEInfo.dwFileSize)
			{
				dwNoOfBytes = 0;
			}
            else
				dwNoOfBytes = dwBufferBytes = lpFileInfo->DEInfo.dwFileSize-lpFileInfo->dwTotalBytesPassed;

            lpFileInfo->bBufferIsValid=FALSE;
        }

        //copy the contents to callee buffer
        memcpy(&pBuffer[dwTotReadBytes],(BYTE *)lpFileInfo->lpBuffer,dwBufferBytes);
        
        //increment Read count and Offsets
        dwTotReadBytes+=dwBufferBytes;
        lpFileInfo->wBufferOffset+=dwBufferBytes;
        lpFileInfo->dwTotalBytesPassed+=dwBufferBytes;
    };
	
    //DEBUG_PRINT_OBJECT1("returns %ld", dwTotReadBytes);
    return dwTotReadBytes;

}
UINT32 FAT_WriteFile(LPFSHANDLE lpFS, HANDLE hFile, UINT32 dwNoOfBytes, void * lpBuffer)
{
    SetLastError(ERROR_NOT_SUPPORTED);
    return 0;
}
UINT32 FAT_DeleteFile(LPFSHANDLE lpFS, LPCTSTR lpFileName)
{
    return DeleteFileEntry( lpFS, lpFileName);
}
/*
    This function deletes a file or folder entry
    - Split the Path to get parent directory
    - if parent directory is null
    -   DirectoryCluster = RootDirectoryCluster
    - else
    -   DirectoryCluster = GetDirectoryEntry( ParentDirectory )->Cluster
    - Loop While DirectoryCluster is not End of chain
    -   Read from GSH into Buffer
    -   Loop for DirectoryEntries
    -       If (DE==RequiredFile)
    -          Set the DE as Free and Write back the cluster/sector to GSH
    -   End Loop
    - End Loop
*/
UINT32 DeleteFileEntry(LPFSHANDLE lpFS, LPCTSTR lpFileName)
{
    char szDrive[_MAX_DRIVE], szDirectory[_MAX_DIR], szFName[_MAX_FNAME], szExt[_MAX_EXT], szDirPath[_MAX_DIR];
    char szFileName[_MAX_FNAME];
    
    int IsRootDirectory;
    LPFAT lpFAT = (LPFAT) lpFS->FSSpecificFields[0];
    UINT32 dwRootLimit = PART_BEGINING(&lpFAT->Part)+FAT_RootDirStartSector(lpFAT)+lpFAT->FDI.dwTotRootDirSectors;
    
    DWORD dwCurrentCluster = 0 , dwLBASector = 0 , dwBufferSize = 0;;
    BYTE * lpBuffer;
    SplitPath(lpFileName, szDrive, szDirectory, szFName, szExt);
    strcpy( szFileName, szFName );
    strcat( szFileName, szExt );
    if ( szDirectory[0] != 0 && strcmp(szDirectory,"/")!=0 && strcmp(szDirectory,"\\")!=0  )       //should traverse the directories first
    {
        DIRENTRY DEInfo;
        LPDIRENTRY lpDEInfo = &DEInfo;
    
        //DEBUG_PRINT_OBJECT1("Traversing through directory(s) %s", szDirectory);
        
        MergePath(szDirPath, szDrive, szDirectory, "","");
        if ( GetDirectoryEntry(lpFAT, szDirPath, lpDEInfo) == NULL )
        {
          //  DEBUG_PRINT_INFO("Path Not Found");
            SetLastError(ERROR_PATH_NOT_FOUND); //file not found
            return 1;
        }
        dwCurrentCluster = ConvertWordsToDoubleWord( lpDEInfo->wFirstClusterHigh, lpDEInfo->wFirstClusterLow);
        
        dwBufferSize = lpFAT->FDI._BPB.wBytesPerSector * lpFAT->FDI._BPB.bSectorsPerCluster;
        IsRootDirectory = FALSE;
    }
    else
    {
        //DEBUG_PRINT_INFO("Root Exploration");
        dwLBASector = PART_BEGINING(&lpFAT->Part)+FAT_RootDirStartSector(lpFAT);
        dwBufferSize = lpFAT->FDI._BPB.wBytesPerSector;
        IsRootDirectory = TRUE;
    }
	  
	lpBuffer = new BYTE[dwBufferSize];
	//lpFAT->FDI._BPB.wBytesPerSector = 512;
	
    do 
    {
		//SkyConsole::Print("\n%s, %d %d %d %d\n", lpFAT->szDevicePath, dwLBASector, dwBufferSize, lpFAT->FDI._BPB.wBytesPerSector);
        ReadGSHDevice((const char*)lpFAT->szDevicePath , dwLBASector, dwBufferSize / lpFAT->FDI._BPB.wBytesPerSector, (BYTE *)lpBuffer);
        if ( GetLastError() )
        {
          //  DEBUG_PRINT_OBJECT1("GSH Error while reading %s.", lpFAT->szDevicePath );
            return 1;
        }
        DWORD dwBufferIndex=0;
        for(;dwBufferIndex<dwBufferSize;dwBufferIndex+=sizeof(DIRENTRY) )
        {
            char szDEFileName[13];
            LPDIRENTRY lpDEInfo=(LPDIRENTRY)((UINT32)lpBuffer + dwBufferIndex );
            if ( GetFileNameFromDEInfo(lpDEInfo, szDEFileName) )
            {
				//SkyConsole::Print("\n%s, %s\n", szDEFileName, szFileName);

                if ( stricmp(szDEFileName, szFileName) == 0 )
                {
                    //if directory entry then the directory should be free
                    if ( lpDEInfo->bAttribute & FILE_ATTRIBUTE_DIR )
                    {
                        DIRENTRY DirEntry;
                        char szFolder[_MAX_PATH];
                        strcpy( szFolder, lpFileName );
                        strcat( szFolder, "*.*");
                        if ( GetDirectoryEntry(lpFAT, szFolder, &DirEntry) != NULL )
                        {
                          //  DEBUG_PRINT_INFO("Directory not empty");
                            SetLastError(ERROR_DIR_NOT_EMPTY); //file not found
                            return 1;
                        }
                    }
                    lpDEInfo->bFileName[0]=0xE5; //free the entry
                    
                    WriteGSHDevice((const char*)lpFAT->szDevicePath, dwLBASector, dwBufferSize / lpFAT->FDI._BPB.wBytesPerSector, (BYTE *)lpBuffer);
                    return 0;
                }
            }
        }
        
        if ( IsRootDirectory )
        {
            if ( dwLBASector <= dwRootLimit ) 
                dwLBASector ++;
            else
            {				
              //  DEBUG_PRINT_INFO("File not found");
                SetLastError( ERROR_FILE_NOT_FOUND );
                return 1;
            }
        }
        else
        {
            ReadGSHDevice((const char*)lpFAT->szDevicePath , PART_BEGINING(&lpFAT->Part) + FAT_FATSectorNumber(lpFAT, dwCurrentCluster), 1, (BYTE *)lpBuffer);
            if ( GetLastError() )
            {
				SkyConsole::Print("\n66666\n");
                //DEBUG_PRINT_OBJECT1("GSH read error while reading %s", lpFAT->szDevicePath);
                return 1;
            }
            dwCurrentCluster = FAT_GetFATEntry(lpFAT, dwCurrentCluster, (BYTE *)lpBuffer);
            
            if ( FAT_IsEndOfClusterChain(lpFAT, dwCurrentCluster) || FAT_IsBadCluster(lpFAT, dwCurrentCluster) )
            {
				SkyConsole::Print("\n777777\n");
              //  DEBUG_PRINT_INFO("File not found");
                SetLastError( ERROR_FILE_NOT_FOUND );
                return 1;
            }
            dwLBASector = PART_BEGINING(&lpFAT->Part) + FAT_FirstSectorOfCluster(lpFAT,dwCurrentCluster);
        }
        
    }while( 1 );
    
}
UINT32 FAT_GetFileAttributes(LPFSHANDLE lpFS, LPCTSTR lpFileName, LPFILEINFO lpFINFO)
{
    DIRENTRY DEInfo;
    if ( lpFINFO == NULL )
    {
        SetLastError(ERROR_INVALID_HANDLE);
        return 0;
    }
    if ( GetDirectoryEntry((LPFAT) lpFS->FSSpecificFields[0], (char *)lpFileName, &DEInfo) != NULL )
    {
        lpFINFO->dwFileAttributes=DEInfo.bAttribute;
        lpFINFO->nFileSizeHigh=0; 
        lpFINFO->nFileSizeLow=DEInfo.dwFileSize;
        //todo add date and time info
        return DEInfo.bAttribute;
    }
    SetLastError(ERROR_FILE_NOT_FOUND);
    return 0;
}
char * GetFileNameFromDEInfo(const LPDIRENTRY lpDEInfo, char * szDEFileName)
{
    if ( lpDEInfo->bFileName[0] == 0 )
            return NULL;
    //if ( lpDEInfo->bFileName[0]==0x05 )
    //  lpDEInfo->bFileName[0]=0xE5;

    if ( lpDEInfo->bFileName[0]!=0xE5 )
    {
        char szExtension[4];
        int i;
        if ( lpDEInfo->bFileName[0]==0x05 )
            lpDEInfo->bFileName[0]=0xE5;
        strncpy(szDEFileName,(char *)lpDEInfo->bFileName, 8 );
        szDEFileName[8]=0;
        for(i=7;i>=0;i--)
            if ( szDEFileName[i]==' ')
                szDEFileName[i]=0;
        strcat(szDEFileName,".");
        strncpy(szExtension, (char *)lpDEInfo->bExtension,3);
        szExtension[3]=0;
        for(i=2;i>=0;i--)
            if ( szExtension[i]==' ')
                szExtension[i]=0;
        strcat(szDEFileName,szExtension);
        szDEFileName[12]=0;
    }
    return szDEFileName;
}

#define FIND_DE_IN_FOLDER() \
    bFound=FALSE;\
    do \
    { \
        DWORD dwLBASector=PART_BEGINING(&lpFileInfo->lpFATInfo->Part) + FAT_FirstSectorOfCluster(lpFileInfo->lpFATInfo,lpFileInfo->dwCurrentCluster);\
        if ( lpFileInfo->dwBufferedSecctor != dwLBASector ) \
        {\
            ReadGSHDevice((const char*)lpFileInfo->lpFATInfo->szDevicePath, dwLBASector, lpFileInfo->lpFATInfo->FDI._BPB.bSectorsPerCluster, (BYTE *)lpFileInfo->lpBuffer);\
            if ( GetLastError() )\
            {\
                return 0;\
            }\
            lpFileInfo->dwBufferedSecctor = dwLBASector;\
        }\
        if ( lpFileInfo->dwBufferIndex >= lpFileInfo->dwBufferTotal) \
            lpFileInfo->dwBufferIndex = 0;\
        for(;lpFileInfo->dwBufferIndex<lpFileInfo->dwBufferTotal;lpFileInfo->dwBufferIndex++ )\
        {\
            char szDEFileName[13];\
            lpDEInfo=(LPDIRENTRY)((UINT32)lpFileInfo->lpBuffer + (lpFileInfo->dwBufferIndex*sizeof(DIRENTRY)) );\
            if ( GetFileNameFromDEInfo(lpDEInfo, szDEFileName) )\
                if ( PatternSearch(szDEFileName,szPattern) )\
                {\
                    lpFileInfo->dwBufferIndex++;\
                    if ( lpFileInfo->dwBufferIndex>=lpFileInfo->dwBufferTotal )\
                        lpFileInfo->dwCurrentLBASector++;\
                    bFound=TRUE;\
                    break;\
                }\
        }\
        if ( ++lpFileInfo->dwBufferIndex>=lpFileInfo->dwBufferTotal) \
        {\
            ReadGSHDevice((const char*)lpFileInfo->lpFATInfo->szDevicePath, PART_BEGINING(&lpFileInfo->lpFATInfo->Part) + FAT_FATSectorNumber(lpFileInfo->lpFATInfo,lpFileInfo->dwCurrentCluster), 1, (BYTE *)lpFileInfo->lpBuffer);\
            if ( GetLastError() )\
            {\
                return 0;\
            }\
            lpFileInfo->dwCurrentCluster = FAT_GetFATEntry(lpFileInfo->lpFATInfo, lpFileInfo->dwCurrentCluster, (BYTE *)lpFileInfo->lpBuffer);\
        }\
    }while( !bFound && !FAT_IsEndOfClusterChain(lpFileInfo->lpFATInfo, lpFileInfo->dwCurrentCluster) && FAT_IsBadCluster(lpFileInfo->lpFATInfo, lpFileInfo->dwCurrentCluster) );


/*#define FIND_DE_IN_FOLDER() \
    bFound=FALSE;\
    do \
    { \
        DWORD dwLBASector=PART_BEGINING(&lpFileInfo->lpFATInfo->Part) + FAT_FirstSectorOfCluster(lpFileInfo->lpFATInfo,lpFileInfo->dwCurrentCluster);\
        if ( lpFileInfo->dwBufferedSecctor != dwLBASector ) \
        {\
            ReadGSHDevice((const char*)lpFileInfo->lpFATInfo->szDevicePath, dwLBASector, lpFileInfo->lpFATInfo->FDI._BPB.bSectorsPerCluster, (BYTE *)lpFileInfo->lpBuffer);\
            if ( GetLastError() )\
            {\
                DEBUG_PRINT_OBJECT1("GSH read error while reading %s", lpFileInfo->lpFATInfo->szDevicePath );\
                return 0;\
            }\
            lpFileInfo->dwBufferedSecctor = dwLBASector;\
        }\
        if ( lpFileInfo->dwBufferIndex >= lpFileInfo->dwBufferTotal) \
            lpFileInfo->dwBufferIndex = 0;\
        for(;lpFileInfo->dwBufferIndex<lpFileInfo->dwBufferTotal;lpFileInfo->dwBufferIndex++ )\
        {\
            char szDEFileName[13];\
            lpDEInfo=(LPDIRENTRY)((UINT32)lpFileInfo->lpBuffer + (lpFileInfo->dwBufferIndex*sizeof(DIRENTRY)) );\
            if ( GetFileNameFromDEInfo(lpDEInfo, szDEFileName) )\
                if ( PatternSearch(szDEFileName,szPattern) )\
                {\
                    lpFileInfo->dwBufferIndex++;\
                    if ( lpFileInfo->dwBufferIndex>=lpFileInfo->dwBufferTotal )\
                        lpFileInfo->dwCurrentLBASector++;\
                    bFound=TRUE;\
                    break;\
                }\
        }\
        if ( ++lpFileInfo->dwBufferIndex>=lpFileInfo->dwBufferTotal) \
        {\
            ReadGSHDevice(lpFileInfo->lpFATInfo->szDevicePath, PART_BEGINING(&lpFileInfo->lpFATInfo->Part) + FAT_FATSectorNumber(lpFileInfo->lpFATInfo,lpFileInfo->dwCurrentCluster), 1, (BYTE *)lpFileInfo->lpBuffer);\
            if ( GetLastError() )\
            {\
                DEBUG_PRINT_OBJECT1("GSH read error while reading %s", lpFileInfo->lpFATInfo->szDevicePath );\
                return 0;\
            }\
            lpFileInfo->dwCurrentCluster = FAT_GetFATEntry(lpFileInfo->lpFATInfo, lpFileInfo->dwCurrentCluster, (BYTE *)lpFileInfo->lpBuffer);\
        }\
    }while( !bFound && !FAT_IsEndOfClusterChain(lpFileInfo->lpFATInfo, lpFileInfo->dwCurrentCluster) && FAT_IsBadCluster(lpFileInfo->lpFATInfo, lpFileInfo->dwCurrentCluster) );*/

#define FIND_DE_IN_ROOT() \
    UINT32 dwLBASec=lpFileInfo->dwCurrentLBASector;\
    UINT32 dwLimit=PART_BEGINING(&lpFileInfo->lpFATInfo->Part)+FAT_RootDirStartSector(lpFileInfo->lpFATInfo)+lpFileInfo->lpFATInfo->FDI.dwTotRootDirSectors;\
    bFound=FALSE;\
    for (;dwLBASec<dwLimit && bFound==FALSE; dwLBASec++)\
    {\
        if ( lpFileInfo->dwBufferedSecctor != dwLBASec ) \
        {\
            ReadGSHDevice((const char*)lpFileInfo->lpFATInfo->szDevicePath, dwLBASec, 1, (BYTE *)lpFileInfo->lpBuffer);\
            if ( GetLastError() )\
            {\
                return 0;\
            }\
            lpFileInfo->dwBufferedSecctor = dwLBASec;\
        }\
        if ( lpFileInfo->dwBufferIndex>=lpFileInfo->dwBufferTotal) \
            lpFileInfo->dwBufferIndex=0;\
        for(;lpFileInfo->dwBufferIndex<lpFileInfo->dwBufferTotal;lpFileInfo->dwBufferIndex++ )\
        {\
            char szDEFileName[13];\
            lpDEInfo=(LPDIRENTRY)((UINT32)lpFileInfo->lpBuffer + (lpFileInfo->dwBufferIndex*sizeof(DIRENTRY)) );\
            if ( GetFileNameFromDEInfo(lpDEInfo, szDEFileName) )\
                if ( PatternSearch(szDEFileName,szPattern) )\
                {\
                    lpFileInfo->dwBufferIndex++;\
                    if ( lpFileInfo->dwBufferIndex>=lpFileInfo->dwBufferTotal )\
                        lpFileInfo->dwCurrentLBASector++;\
                    bFound=TRUE;\
                    break;\
                }\
        }\
    }

/*#define FIND_DE_IN_ROOT() \
    UINT32 dwLBASec=lpFileInfo->dwCurrentLBASector;\
    UINT32 dwLimit=PART_BEGINING(&lpFileInfo->lpFATInfo->Part)+FAT_RootDirStartSector(lpFileInfo->lpFATInfo)+lpFileInfo->lpFATInfo->FDI.dwTotRootDirSectors;\
    bFound=FALSE;\
    for (;dwLBASec<dwLimit && bFound==FALSE; dwLBASec++)\
    {\
        if ( lpFileInfo->dwBufferedSecctor != dwLBASec ) \
        {\
            ReadGSHDevice(lpFileInfo->lpFATInfo->szDevicePath, dwLBASec, 1, (BYTE *)lpFileInfo->lpBuffer);\
            if ( GetLastError() )\
            {\
                DEBUG_PRINT_OBJECT1("GSH read error while reading %s", lpFileInfo->lpFATInfo->szDevicePath );\
                return 0;\
            }\
            lpFileInfo->dwBufferedSecctor = dwLBASec;\
        }\
        if ( lpFileInfo->dwBufferIndex>=lpFileInfo->dwBufferTotal) \
            lpFileInfo->dwBufferIndex=0;\
        for(;lpFileInfo->dwBufferIndex<lpFileInfo->dwBufferTotal;lpFileInfo->dwBufferIndex++ )\
        {\
            char szDEFileName[13];\
            lpDEInfo=(LPDIRENTRY)((UINT32)lpFileInfo->lpBuffer + (lpFileInfo->dwBufferIndex*sizeof(DIRENTRY)) );\
            if ( GetFileNameFromDEInfo(lpDEInfo, szDEFileName) )\
                if ( PatternSearch(szDEFileName,szPattern) )\
                {\
                    lpFileInfo->dwBufferIndex++;\
                    if ( lpFileInfo->dwBufferIndex>=lpFileInfo->dwBufferTotal )\
                        lpFileInfo->dwCurrentLBASector++;\
                    bFound=TRUE;\
                    break;\
                }\
        }\
    }*/
    
#define FILL_FILE_INFO() \
    char szFileName[9], szExt[4];\
    strncpy((char*)szFileName, (const char*)lpDEInfo->bFileName,8);\
    szFileName[8]=0;\
    strncpy((char*)szExt, (const char*)lpDEInfo->bExtension,3);\
    szExt[3]=0;\
    lpFINFO->dwFileAttributes=lpDEInfo->bAttribute;\
    lpFINFO->nFileSizeHigh = 0;\
    lpFINFO->nFileSizeLow = lpDEInfo->dwFileSize;\
    strcpy(lpFINFO->cFileName, lpFileInfo->szReqPath);\
    strcat(lpFINFO->cFileName, szFileName);\
    strcat(lpFINFO->cFileName, szExt);\
    strcpy( lpFINFO->cAlternateFileName, szFileName);\
    strcat( lpFINFO->cAlternateFileName, szExt);\


HANDLE FAT_FindFirstFile(LPFSHANDLE lpFS, LPCTSTR lpFilePath, LPFILEINFO lpFINFO)
{
    LPDIRENTRY lpDEInfo;
    LPFATFINDFILEINFO lpFileInfo;
    BYTE bFound;
    
    char szFileName[_MAX_FNAME], szExtension[_MAX_EXT], szPattern[_MAX_FNAME], szDirectory[_MAX_DIR], szDrive[_MAX_DRIVE], szDirPath[_MAX_PATH];
    
   // DEBUG_PRINT_INFO(lpFilePath);
    
	lpDEInfo = new DIRENTRY;
    SplitPath(lpFilePath, szDrive, szDirectory, szFileName, szExtension);
    strcpy(szPattern, szFileName);
    if ( strlen(szExtension) )
    {
        strcat(szPattern, ".");
        strcat(szPattern, &szExtension[1]);
    }
	lpFileInfo = new FATFINDFILEINFO;
	memset(lpFileInfo, 0, sizeof(FATFINDFILEINFO));
    lpFileInfo->lpFATInfo = (LPFAT) lpFS->FSSpecificFields[0];
    if ( lpFileInfo->lpFATInfo == NULL )
    {
        //DEBUG_PRINT_INFO("Cannot Get FAT Information.");
        return 0;
    }
    strcpy(lpFileInfo->szFilePattern, szPattern);
    MergePath(lpFileInfo->szReqPath, szDrive, szDirectory, szFileName, szExtension);
    
    if ( szDirectory[0] != 0 && strcmp(szDirectory,"/")!=0 && strcmp(szDirectory,"\\")!=0  )       //should traverse the directories first
    {
        UINT32 dwNoOfBytes;
       // DEBUG_PRINT_OBJECT1("Traversing through directory(s) %s", szDirectory);
        MergePath(szDirPath, szDrive, szDirectory, "","");
        if ( GetDirectoryEntry(lpFileInfo->lpFATInfo, szDirPath, lpDEInfo) == NULL )
        {
       //     DEBUG_PRINT_INFO("Path not found")
            SetLastError(ERROR_PATH_NOT_FOUND); //file not found
            return NULL;
        }
        lpFileInfo->dwCurrentCluster = ConvertWordsToDoubleWord( lpDEInfo->wFirstClusterHigh, lpDEInfo->wFirstClusterLow );
                
        dwNoOfBytes=lpFileInfo->lpFATInfo->FDI._BPB.wBytesPerSector*lpFileInfo->lpFATInfo->FDI._BPB.bSectorsPerCluster;
        
		lpFileInfo->lpBuffer = new BYTE[dwNoOfBytes];
        
        lpFileInfo->dwBufferIndex=0;
        lpFileInfo->dwBufferTotal=dwNoOfBytes/sizeof(DIRENTRY);
        lpFileInfo->bRoot=FALSE;
        FIND_DE_IN_FOLDER()
    }
    else
    {
        UINT32 dwNoOfBytes;
        //DEBUG_PRINT_INFO("Root exploration.");

        lpFileInfo->dwCurrentLBASector = PART_BEGINING(&lpFileInfo->lpFATInfo->Part)+FAT_RootDirStartSector(lpFileInfo->lpFATInfo);
        
        dwNoOfBytes = lpFileInfo->lpFATInfo->FDI._BPB.wBytesPerSector;
		lpFileInfo->lpBuffer = new BYTE[dwNoOfBytes];
        
        lpFileInfo->dwBufferIndex=0;
        lpFileInfo->dwBufferTotal=dwNoOfBytes/sizeof(DIRENTRY);
        if ( lpFileInfo->lpFATInfo->FDI.bFATType==12 || lpFileInfo->lpFATInfo->FDI.bFATType==16 )
            lpFileInfo->bRoot=TRUE;
        FIND_DE_IN_ROOT();
    }
        
    if ( bFound )
    {
        memcpy(&lpFileInfo->DEInfo,lpDEInfo,sizeof(DIRENTRY));
    
        if ( List_Add(&sysFATOpenFileInfo,"",lpFileInfo)==0 )
        {
            FILL_FILE_INFO();
           // DEBUG_PRINT_OBJECT1("Returns %X",lpFINFO);
            return lpFileInfo;
        }
        else
        {
           // DEBUG_PRINT_INFO("List add failed. returns null");
            return NULL;
        }
    }
   // DEBUG_PRINT_INFO("File not found");
    return NULL;
}
UINT32 FAT_FindNextFile(LPFSHANDLE lpFS, HANDLE hFile, LPFILEINFO lpFINFO )
{
    LPDIRENTRY lpDEInfo = 0;;
    LPFATFINDFILEINFO lpFileInfo = 0;;
    BYTE bFound=FALSE;
    
    char szPattern[_MAX_FNAME];
    
    lpFileInfo = (LPFATFINDFILEINFO) hFile;
    
   // DEBUG_PRINT_OBJECT2("[%s] [%s]", szPattern, lpFileInfo->szFilePattern);
    
    strcpy(szPattern, lpFileInfo->szFilePattern);
    if ( lpFileInfo->bRoot ) //searching in root directory (only for FAT12 and FAT16)
    {
      //  DEBUG_PRINT_INFO("Searching in root directory");
        FIND_DE_IN_ROOT();
    }
    else
    {
       // DEBUG_PRINT_INFO("Searching in folders");
        FIND_DE_IN_FOLDER();
    }
    if ( bFound == TRUE )
    {
        memcpy(&lpFileInfo->DEInfo,lpDEInfo,sizeof(DIRENTRY));
        FILL_FILE_INFO();
        return (UINT32)lpFileInfo;
    }
    
   // DEBUG_PRINT_INFO("Returns NULL");
    return 0;
    
}
UINT32 FAT_FindClose(LPFSHANDLE lpFS, HANDLE hFile)
{
    SetLastError(ERROR_NOT_SUPPORTED);
    return 0;
}

UINT32 ReadGSHDevice(const char * szDevicePath, DWORD dwLBASector, DWORD dwSectorCount, BYTE * lpBuffer)
{
        GSHIOPARA m_IOPara;
        m_IOPara.bMode = 1;	//LBA Mode
        m_IOPara.dwLBASector = dwLBASector;
        m_IOPara.dwSectorCount = dwSectorCount;
        
        return GSH_Read( szDevicePath, &m_IOPara, lpBuffer );
}
UINT32 WriteGSHDevice(const char * szDevicePath, DWORD dwLBASector, DWORD dwSectorCount, BYTE * lpBuffer)
{
        GSHIOPARA m_IOPara;
        m_IOPara.bMode = 1;	//LBA Mode
        m_IOPara.dwLBASector = dwLBASector;
        m_IOPara.dwSectorCount = dwSectorCount;
        
        return GSH_Write( szDevicePath, &m_IOPara, lpBuffer );
}
char * ConvertFileNameToProperFormat(char * szFile, char * szFileName, char * szExt)
{
    int i;
    szFile[0]=0;
    if ( szFileName[0] ) 
    {
        strcpy(szFile, szFileName);
	    for(i=strlen(szFile);i<8;i++)
            szFile[i]=' ';
        szFile[8]=0;
    }
    if ( szExt[0] )
    {
        strcat(szFile, &szExt[1]);
	    for(i=strlen(szFile);i<11;i++)
            szFile[i]=' ';
        szFile[11]=0;
    }
    return szFile;
}


/*Internal function. This function is used as callback function
to find out whether the given file name is exist in the list
*/
BYTE FindFileInEnumeration(LPDIRENTRY DEInfo, void * vFileName, LPDIRENTRY lpResult)
{
    char File[13], *FileName=(char *)vFileName;
    strncpy( File, (char *)DEInfo->bFileName, 11 );
    File[11]=0;
    
   // DEBUG_PRINT_OBJECT3("[%s] %X [%s]",File, DEInfo->bAttribute, FileName);
    if (!strnicmp(File,FileName, strlen(FileName)) )
    {
        memcpy(lpResult,DEInfo,sizeof(DIRENTRY));
        return 0;	//break enumeration
    }
    else
	return 1; 	//continue enumeration
}
LPDIRENTRY GetDirectoryEntry(LPFAT lpFAT, char * szFilePath, LPDIRENTRY DEInfo)
{
    char szDrive[_MAX_DRIVE],szDir[_MAX_DIR],szFileName[_MAX_FNAME],szExt[_MAX_EXT],szFile[14];
    char szCurDir[13],* szNextDir;

  //  DEBUG_PRINT_INFO(szFilePath);
    SplitPath(szFilePath, szDrive, szDir, szFileName, szExt);
   // DEBUG_PRINT_OBJECT3("Dir [%s] FileName = [%s] Ext [%s]", szDir, szFileName, szExt);
    ConvertFileNameToProperFormat(szFile, szFileName, szExt);
    
    szNextDir = ExtractFirstToken( szCurDir, szDir+1, '\\' );
    if ( szCurDir[0] == 0 )	
    {
        if ( EnumerateFilesInRoot(lpFAT, FindFileInEnumeration, szFile, DEInfo) != 2 )
            return 0;
        else
            return DEInfo;
    }
    else
    {
        if ( EnumerateFilesInRoot(lpFAT,FindFileInEnumeration,szCurDir, DEInfo) != 2 )
            return 0;
        do
	    {
            szNextDir=ExtractFirstToken( szCurDir, &szDir[szNextDir-szDir+1], '\\' );
            if ( EnumerateFilesInFolder(lpFAT, DEInfo, FindFileInEnumeration, szCurDir, DEInfo) != 2 )
		        return 0;
        }while (szNextDir[1]);
        
	    if ( szFile[0] )
            if ( EnumerateFilesInFolder(lpFAT, DEInfo, FindFileInEnumeration, szFile, DEInfo) != 2 )
                return 0;
        return DEInfo;
    }
}
/*
1) Get the FAT Data structure 
2) Calculate Buffer Size (BytesPerSector*SectorsPerCluster)
3) Allocate Buffer
4) Construct 32 bit Directory Cluster from input 16 bit entries
5) Read Sectors(Directory Cluster) into Buffer
6) LOOP
    a) Fill DIRENTRY from buffer
    b) Check for valid file name
    c) Call Callback function
   LOOP END
7) Get the FAT Sector for the Directory Cluster
8) Read the FAT Sector
9) Get the FAT Entry for the Directory Cluster
10) Set Directory Cluster = FATEntry
11) Goto Step 5 if Directory Cluster is not EOD and not Bad Entry.

*/
BYTE EnumerateFilesInFolder(LPFAT lpFATInfo, LPDIRENTRY FolderDE, BYTE (*CallBackFn)(LPDIRENTRY, void * ,LPDIRENTRY), void * CallBackPara , LPDIRENTRY lpResult)
{
    LPDIRENTRY DEInfo;
    UINT16 * pDirData,i;
    UINT32 dwDirCluster,dwNoOfBytes;
    
    dwNoOfBytes = lpFATInfo->FDI._BPB.wBytesPerSector*lpFATInfo->FDI._BPB.bSectorsPerCluster;
	pDirData = new UINT16[dwNoOfBytes];
	memset(pDirData, 0, dwNoOfBytes * sizeof(UINT16));
    
    //here add check for whether FolderDE->bAttribute is Folder or not
    dwDirCluster = ConvertWordsToDoubleWord( FolderDE->wFirstClusterHigh, FolderDE->wFirstClusterLow);
    do
    {
        ReadGSHDevice((const char*)lpFATInfo->szDevicePath , PART_BEGINING(&lpFATInfo->Part) + FAT_FirstSectorOfCluster(lpFATInfo,dwDirCluster), lpFATInfo->FDI._BPB.bSectorsPerCluster, (BYTE *)pDirData);
        if ( GetLastError() )
            return 0;
        for(i=0; i<dwNoOfBytes;i+=sizeof(DIRENTRY))	
        {
            DEInfo=(LPDIRENTRY)((UINT32)pDirData + i);
            BYTE bResult = ValidateDEInfo(DEInfo);
            if ( bResult == 1 ) //this is the end
                return 0;
            if ( bResult == 0 && ! (DEInfo->bAttribute & 0xC8)  ) //filter VOLUME and LONG NAME entries
            {
                if ( !CallBackFn(DEInfo, CallBackPara, lpResult ) )
                {
                //    DEBUG_PRINT_INFO("Returns 2");
                    return 2;
                }
            }
        }
        ReadGSHDevice((const char*)lpFATInfo->szDevicePath , PART_BEGINING(&lpFATInfo->Part) + FAT_FATSectorNumber(lpFATInfo,dwDirCluster), 1, (BYTE *)pDirData);
        if ( GetLastError() )
            return 0;
        dwDirCluster = FAT_GetFATEntry(lpFATInfo, dwDirCluster, (BYTE *)pDirData);
    }while( !FAT_IsEndOfClusterChain(lpFATInfo, dwDirCluster) && FAT_IsBadCluster(lpFATInfo, dwDirCluster) );
    return 1;
}
BYTE ValidateDEInfo(LPDIRENTRY DEInfo)
{
    if ( DEInfo->bFileName[0] == 0 ) //this is the end
        return 1;
    if ( DEInfo->bFileName[0] == 0xE5 )// free
        return 2;
    if ( DEInfo->bFileName[0]==0x05 )
        DEInfo->bFileName[0]=0xE5;
    return 0;
}
BYTE EnumerateFilesInRoot(LPFAT lpFATInfo, BYTE (*CallBackFn)(LPDIRENTRY ,void *,LPDIRENTRY), void * CallBackPara, LPDIRENTRY lpResult )
{
    //DEBUG_PRINT_INFO("");
    
    LPDIRENTRY DEInfo;
    UINT16 * pRDSector, i, dwNoOfBytes;
    UINT32 dwLBASec, dwLimit;
    
    if ( lpFATInfo->FDI.bFATType == 32 ) //FAT32 contains no special root directory it is also like ordinary directory
    {
        DIRENTRY RD32Info;
        RD32Info.wFirstClusterHigh=FAT_RootDirStartCluster(lpFATInfo)>>16;
        RD32Info.wFirstClusterLow=(FAT_RootDirStartCluster(lpFATInfo)<<16)>>16;
        return EnumerateFilesInFolder(lpFATInfo, &RD32Info, CallBackFn, CallBackPara, lpResult);
    }
    
    dwNoOfBytes=lpFATInfo->FDI._BPB.wBytesPerSector;//*lpFATInfo->FDI.dwTotRootDirSectors;
	pRDSector = new UINT16[dwNoOfBytes];    
	memset(pRDSector, 0, dwNoOfBytes * sizeof(UINT16));
    
    dwLBASec=PART_BEGINING(&lpFATInfo->Part)+FAT_RootDirStartSector(lpFATInfo);
    dwLimit=dwLBASec+lpFATInfo->FDI.dwTotRootDirSectors;
    for (;dwLBASec < dwLimit; dwLBASec++ )
    {
        ReadGSHDevice((const char*)lpFATInfo->szDevicePath , dwLBASec, 1, (BYTE *)pRDSector);
        if ( GetLastError() )
        {
           // DEBUG_PRINT_OBJECT1("GSH read error while reading %s", lpFATInfo->szDevicePath);
            return 0;
        }
        for(i=0; i<dwNoOfBytes;i+=sizeof(DIRENTRY))	
        {
            DEInfo=(LPDIRENTRY)((UINT32)pRDSector + i);
            BYTE bResult = ValidateDEInfo(DEInfo);
            if ( bResult == 1 ) //this is the end
                return 0;
            if ( bResult == 0 && ! (DEInfo->bAttribute & 0xC8)  ) //filter VOLUME and LONG NAME entries
            {
                if ( !CallBackFn(DEInfo, CallBackPara, lpResult ) )
                {
                  //  DEBUG_PRINT_INFO("Returns 2");
                    return 2;
                }
            }
        }
    }
    delete pRDSector;
   // DEBUG_PRINT_INFO("Returns 1");
    return 1;
}
	
/*
This function registers the FAT file system with the GFS
Should be called by Kernel only
*/
int FAT_Init()
{
    sysFATOpenFileInfo=NULL;
    int i;
    for (i=0; i< GetTotalDrives(); i++)
        CheckAndInstallFAT( sysDevicePathLookupTable[i].szDevicePath, &sysDevicePathLookupTable[i].Part );
    LPFSHANDLE lpNewFS = new FSHANDLE;
    
    FILL_FS_STRUCTURE( lpNewFS );
    if ( GFS_Register("FAT", lpNewFS ) == FALSE )
    {
       // DEBUG_PRINT_INFO("GFS Registration failed");
        return 0;
    }
    return 1;
}

void CheckAndInstallFAT(LPCTSTR szDevicePath, LPPARTITION lpPart)
{
    BYTE pFirstSector[512];
    if (PART_TYPE(lpPart)==0x1  || PART_TYPE(lpPart)==0x4  || PART_TYPE(lpPart)==0x6  || PART_TYPE(lpPart)==0xB  || PART_TYPE(lpPart)==0xC  || PART_TYPE(lpPart)==0xE  || \
        PART_TYPE(lpPart)==0x11 || PART_TYPE(lpPart)==0x14 || PART_TYPE(lpPart)==0x1B || PART_TYPE(lpPart)==0x1C || PART_TYPE(lpPart)==0x1E ) 
    {
        LPFSHANDLE lpNewFS = new FSHANDLE;
        LPFAT lpNewFAT = new FAT;
                
       // #warning It may break here because GSH_Read requires a DeviceKey+DeviceNo rather than DevicePath
        ReadGSHDevice( szDevicePath, PART_BEGINING(lpPart), 1, (BYTE *)&pFirstSector[0]);
        if ( GetLastError() )
        {
         //   DEBUG_PRINT_OBJECT1("GSH read error while reading %s", szDevicePath);
            return;
        }
		SkyConsole::Print("szDevicePath %s\n", szDevicePath);
        FAT_Initialize(lpNewFAT, szDevicePath, lpPart, &pFirstSector[0]);
        FILL_FS_STRUCTURE( lpNewFS );
        lpNewFS->FSSpecificFields[0] = (UINT32) lpNewFAT;
      //  DEBUG_PRINT_OBJECT1("Registering device path (%s) with GFS.", szDevicePath );
        if ( GFS_RegisterForDevicePath(szDevicePath, lpNewFS) == FALSE )
        {
           // DEBUG_PRINT_INFO("GFS Registration failed");
            return;
        }
        return;
    }
}
