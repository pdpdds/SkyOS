/******************************************************************************
   fat12.cpp
		-FAT12 Minidriver

   arthor\ Mike
******************************************************************************/

#include <fat12.h>
#include <string.h>
#include "memory.h"
#include "FloppyDisk.h"
#include <bpb.h>
#include <ctype.h>
#include "SkyConsole.h"
#include "str_util.h"

//! bytes per sector
#define SECTOR_SIZE 512

MOUNT_INFO _MountInfo;

//! File Allocation Table (FAT)
uint8_t FAT [SECTOR_SIZE*2];

/**
*	Locates file or directory in root directory
*/
FILE fsysFatDirectory (const char* DirectoryName) {

	FILE file;
	unsigned char* buf;
	PDIRECTORY directory;

	//! get 8.3 directory name
	char DosFileName[11];
	ToDosFileName (DirectoryName, DosFileName, 11);
	DosFileName[11]=0;

#ifdef  _SKY_DEBUG
	SkyConsole::Print("FileOpen From Floppy, FileName : %s\n", DosFileName);
#endif //  _SKY_DEBUG
	
	//! 14 sectors per directory
	for (int sector=0; sector<14; sector++) {

		//! read in sector of root directory
		buf = (unsigned char*) FloppyDisk::ReadSector(_MountInfo.rootOffset + sector );
		
		//! get directory info
		directory = (PDIRECTORY) buf;

		//! 16 entries per sector
		for (int i=0; i<16; i++) {
			
			//! get current filename
			char name[11];
			memcpy (name, directory->Filename, 11);
			name[11]=0;

			//SkyConsole::Print("name : %s\n", name);

			//! find a match?
			if (strcmp (DosFileName, name) == 0) {

				//! found it, set up file info
				strcpy (file._name, DirectoryName);
				file._id             = 0;
				file._currentCluster = directory->FirstCluster;
				file._fileLength     = directory->FileSize;
				file._eof            = 0;				

				//! set file type
				if (directory->Attrib == 0x10)
					file._flags = FS_DIRECTORY;
				else
					file._flags = FS_FILE;

				//! return file
				return file;
			}

			//! go to next directory
			directory++;
		}
	}

	//! unable to find file
	file._flags = FS_INVALID;
	return file;
}

/**
*	Reads from a file
*/
int fsysFatRead(PFILE file, unsigned char* Buffer, unsigned int Length) {

	if (file == nullptr)
		return 0;

	//! starting physical sector
	unsigned int physSector = 32 + (file->_currentCluster - 1);

	//! read in sector
	unsigned char* sector = (unsigned char*)FloppyDisk::ReadSector(physSector);

	//! copy block of memory
	memcpy(Buffer, sector, 512);

	//! locate FAT sector
	unsigned int FAT_Offset = file->_currentCluster + (file->_currentCluster / 2); //multiply by 1.5
	unsigned int FAT_Sector = 1 + (FAT_Offset / SECTOR_SIZE);
	unsigned int entryOffset = FAT_Offset % SECTOR_SIZE;

	//! read 1st FAT sector
	sector = (unsigned char*)FloppyDisk::ReadSector(FAT_Sector);
	memcpy(FAT, sector, 512);

	//! read 2nd FAT sector
	sector = (unsigned char*)FloppyDisk::ReadSector(FAT_Sector + 1);
	memcpy(FAT + SECTOR_SIZE, sector, 512);

	//! read entry for next cluster
	uint16_t nextCluster = *(uint16_t*)&FAT[entryOffset];

	//! test if entry is odd or even
	if (file->_currentCluster & 0x0001)
		nextCluster >>= 4;      //grab high 12 bits
	else
		nextCluster &= 0x0FFF;   //grab low 12 bits

	//! test for end of file
	if (nextCluster >= 0xff8) {

		file->_eof = 1;
		return 512;
	}

	//! test for file corruption
	if (nextCluster == 0) {

		file->_eof = 1;
		return 0;
	}

	//! set next cluster
	file->_currentCluster = nextCluster;

	return 512;
}

/**
*	Closes file
*/
bool fsysFatClose (PFILE file) {

	if (file)
		file->_flags = FS_INVALID;

	return true;
}

/**
*	Locates a file or folder in subdirectory
*/
FILE fsysFatOpenSubDir (FILE kFile,
						const char* filename) {

	FILE file;

	//! get 8.3 directory name
	char DosFileName[11];
	ToDosFileName (filename, DosFileName, 11);
	DosFileName[11]=0;

	if (kFile._flags != FS_INVALID) {

		//! read directory
		while (! kFile._eof ) {

			//! read directory
			unsigned char buf[512];
			fsysFatRead (&file, buf, 512);

			//! set directort
			PDIRECTORY pkDir = (PDIRECTORY) buf;

			//! 16 entries in buffer
			for (unsigned int i = 0; i < 16; i++) {

				//! get current filename
				char name[11];
				memcpy (name, pkDir->Filename, 11);
				name[11]=0;

				//! match?
				if (strcmp (name, DosFileName) == 0) {

					//! found it, set up file info
					strcpy (file._name, filename);
					file._id             = 0;
					file._currentCluster = pkDir->FirstCluster;
					file._fileLength     = pkDir->FileSize;
					file._eof            = 0;
					file._fileLength     = pkDir->FileSize;

					//! set file type
					if (pkDir->Attrib == 0x10)
						file._flags = FS_DIRECTORY;
					else
						file._flags = FS_FILE;

					//! return file
					return file;
				}

				//! go to next entry
				pkDir++;
			}
		}
	}

	//! unable to find file
	file._flags = FS_INVALID;
	return file;
}

/**
*	Opens a file
*/
FILE fsysFatOpen (const char* FileName) 
{
	FILE curDirectory;
	memset(&curDirectory, 0, sizeof(FILE));
	char* p = 0;
	bool rootDir=true;
	char* path = (char*) FileName;
	
	//! any '\'s in path?
	p = strchr (path, '\\');
	if (!p) {
		
		//! nope, must be in root directory, search it
		curDirectory = fsysFatDirectory (path);
		
		//! found file?
		if (curDirectory._flags == FS_FILE)
			return curDirectory;

		//! unable to find
		FILE ret;
		ret._flags = FS_INVALID;
		return ret;
	}

	//! go to next character after first '\'
	p++;

	
	
	while ( p ) {

		//! get pathname
		char pathname[16];
		int i=0;
		for (i=0; i<16; i++) {

			//! if another '\' or end of line is reached, we are done
			if (p[i]=='\\' || p[i]=='\0')
				break;

			//! copy character
			pathname[i]=p[i];
		}
		pathname[i]=0; //null terminate

		//! open subdirectory or file
		if (rootDir) {

			//! search root directory - open pathname
			curDirectory = fsysFatDirectory (pathname);
			rootDir=false;
		}
		else {

			//! search a subdirectory instead for pathname
			curDirectory = fsysFatOpenSubDir (curDirectory, pathname);
		}

		//! found directory or file?
		if (curDirectory._flags == FS_INVALID)
			break;

		//! found file?
		if (curDirectory._flags == FS_FILE)
			return curDirectory;

		//! find next '\'
		p=strchr (p+1, '\\');
		if (p)
			p++;
	}

	//! unable to find
	FILE ret;
	
	ret._flags = FS_INVALID;
	return ret;
}

//Set Bios Parameter Block

/*bpbOEM			db "My OS   "
bpbBytesPerSector:  	DW 512
bpbSectorsPerCluster : DB 1
bpbReservedSectors : DW 1
bpbNumberOfFATs : DB 2
bpbRootEntries : DW 224
bpbTotalSectors : DW 2880
bpbMedia : DB 0xf0;; 0xF1
bpbSectorsPerFAT: 	DW 9
bpbSectorsPerTrack : DW 18
bpbHeadsPerCylinder : DW 2
bpbHiddenSectors : DD 0
bpbTotalSectorsBig : DD 0
bsDriveNumber : DB 0
bsUnused : DB 0
bsExtBootSignature : DB 0x29
bsSerialNumber : DD 0xa0a1a2a3
bsVolumeLabel : DB "MOS FLOPPY "
bsFileSystem : DB "FAT12   "*/

void fsysFatMount () 
{
	//! store mount info
	_MountInfo.numSectors     = 2880;
	_MountInfo.fatOffset      = 1;
	_MountInfo.fatSize        = 9 * 512;
	_MountInfo.fatEntrySize   = 8;
	_MountInfo.numRootEntries = 224;
	_MountInfo.rootOffset     = 18 + 1;
}
