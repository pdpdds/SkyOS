#include <VFS.h>
#include <string.h>

//저장장치는 최대 26개
#define DEVICE_MAX 26

PFILESYSTEM _FileSystems[DEVICE_MAX];

//파일 열기
FILE volOpenFile (const char* fname) {

	if (fname) {

		//기본 시작 드라이브는 a
		unsigned char device = 'a';
		
		char* filename = (char*) fname;

		// a: b: c: 드라이브를 지정해서 파일을 여는지 확인한다.
		if (fname[1]==':') {

			device = fname[0];
			filename += 3; //strip it from pathname
		}

		//! call filesystem
		if (_FileSystems [device - 'a']) 
		{			
			//파일을 사용할 수 있으면 디바이스 아이디를 세팅하고 리턴한다.
			FILE file = _FileSystems[device - 'a']->Open (filename);
			file.deviceID = device;
			return file;
		}
	}

	//유효하지 않음
	FILE file;
	file.flags = FS_INVALID;
	return file;
}

//파일 읽기
void volReadFile (PFILE file, unsigned char* Buffer, unsigned int Length) {

	if (file)
		if (_FileSystems [file->deviceID - 'a'])
			_FileSystems[file->deviceID - 'a']->Read (file,Buffer,Length);
}

//파일 닫기
void volCloseFile (PFILE file) {

	if (file)
		if (_FileSystems [file->deviceID - 'a'])
			_FileSystems[file->deviceID - 'a']->Close (file);
}


//파일 시스템 등록
void volRegisterFileSystem (PFILESYSTEM fsys, unsigned int deviceID) {

	static int i=0;

	if (i < DEVICE_MAX)
		if (fsys) {

			_FileSystems[ deviceID ] = fsys;
			i++;
		}
}

//파일 시스템에서 해제
void volUnregisterFileSystem (PFILESYSTEM fsys) {

	for (int i=0;i < DEVICE_MAX; i++)
		if (_FileSystems[i]==fsys)
			_FileSystems[i]=0;
}

//디바이스 아이디로 파일 시스템에서 해제
void volUnregisterFileSystemByID (unsigned int deviceID) {

	if (deviceID < DEVICE_MAX)
		_FileSystems [deviceID] = 0;
}