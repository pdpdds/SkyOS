#include "stdafx.h"
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <io.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <errno.h>

// 매크로 정의
// 한 섹터의 바이트 수
#define BYTESOFSECTOR       512

// 패키지의 시그너처
#define PACKAGESIGNATURE    "SKYOS32PACKAGE_ "

// 파일 이름의 최대 길이
#define MAXFILENAMELENGTH   24
// DWORD 타입을 정의
typedef unsigned long DWORD;

#pragma pack( push, 1 )



// 패키지 내부에 파일 정보를 저장하기 위한 구조체
// 파일 이름
// 파일의 크기
typedef struct PackageItemStruct
{
	char vcFileName[MAXFILENAMELENGTH];
	DWORD dwFileLength;
} PACKAGEITEM;

// 패키지 헤더 자료구조
typedef struct PackageHeaderStruct
{
	char vcSignature[16];
	DWORD dwHeaderSize;

} PACKAGEHEADER;

#pragma pack( pop )

// 함수 선언
int AdjustInSectorSize(int iFd, int iSourceSize);
int CopyFile(int iSourceFd, int iTargetFd);

int main(int argc, char* argv[])
{
	int iSourceFd;
	int iTargetFd;
	int iSourceSize = 0;
	int i;
	struct stat stFileData;
	PACKAGEHEADER stHeader;
	PACKAGEITEM stItem;

	// 커맨드 라인 옵션 검사
	if (argc < 2)
	{
		fprintf(stderr, "[ERROR] PackageMaker.exe app1.elf app2.elf data.txt ...\n");
		exit(-1);
	}

	// Package.img 파일을 생성
	if ((iTargetFd = _open("SkyOS.bin", O_RDWR | O_CREAT | O_TRUNC |
		O_BINARY, S_IREAD | S_IWRITE)) == -1)
	{
		fprintf(stderr, "[ERROR] SkyOS.bin open fail.\n");
		exit(-1);
	}

	
	{
		// 데이터 파일을 엶
		if ((iSourceFd = _open(argv[1], O_RDONLY | O_BINARY)) == -1)
		{
			fprintf(stderr, "[ERROR] %s open fail\n", argv[1]);
			exit(-1);
		}

		// 파일의 내용을 패키지 파일에 쓴 뒤에 파일을 닫음
		iSourceSize += CopyFile(iSourceFd, iTargetFd);
		_close(iSourceFd);

		// 파일 크기를 섹터 크기인 512바이트로 맞추기 위해 나머지 부분을 0x00 으로 채움
		int alignedSize = AdjustInSectorSize(iTargetFd, iSourceSize);
		printf("[INFO] %s size = [%d] and sector count = [%d]\n", argv[1], iSourceSize, alignedSize);		
	}

	//--------------------------------------------------------------------------
	//  인자로 전달된 파일 이름으로 패키지 헤더를 먼저 생성
	//--------------------------------------------------------------------------
	printf("[INFO] Create package header...\n");

	// 시그너처를 복사하고 헤더의 크기를 계산
	memcpy(stHeader.vcSignature, PACKAGESIGNATURE, sizeof(stHeader.vcSignature));
	stHeader.dwHeaderSize = sizeof(PACKAGEHEADER) +
		(argc - 2) * sizeof(PACKAGEITEM);
	// 파일에 저장
	if (_write(iTargetFd, &stHeader, sizeof(stHeader)) != sizeof(stHeader))
	{
		fprintf(stderr, "[ERROR] Data write fail\n");
		exit(-1);
	}

	// 인자를 돌면서 패키지 헤더의 정보를 채워 넣음	
	for (i = 2; i < argc; i++)
	{
		// 파일 정보를 확인
		if (stat(argv[i], &stFileData) != 0)
		{
			return 0;
		}

		// 파일 이름과 길이를 저장
		memset(stItem.vcFileName, 0, sizeof(stItem.vcFileName));
		strncpy_s(stItem.vcFileName, argv[i], sizeof(stItem.vcFileName));
		stItem.vcFileName[sizeof(stItem.vcFileName) - 1] = '\0';
		stItem.dwFileLength = stFileData.st_size;

		// 파일에 씀
		if (_write(iTargetFd, &stItem, sizeof(PACKAGEITEM)) != sizeof(PACKAGEITEM))
		{
			fprintf(stderr, "[ERROR] Data write fail\n");
			exit(-1);
		}

		printf("[%d] file: %s, size: %d Byte\n", i, argv[i], stFileData.st_size);
	}
	printf("[INFO] Create complete\n");

	//--------------------------------------------------------------------------
	//  생성된 패키지 헤더 뒤에 파일의 내용을 복사
	//--------------------------------------------------------------------------
	printf("[INFO] Copy data file to package...\n");
	iSourceSize = 0;
	for (i = 2; i < argc; i++)
	{
		// 데이터 파일을 엶
		if ((iSourceFd = _open(argv[i], O_RDONLY | O_BINARY)) == -1)
		{
			fprintf(stderr, "[ERROR] %s open fail\n", argv[1]);
			exit(-1);
		}

		// 파일의 내용을 패키지 파일에 쓴 뒤에 파일을 닫음
		iSourceSize += CopyFile(iSourceFd, iTargetFd);
		_close(iSourceFd);
	}

	// 파일 크기를 섹터 크기인 512바이트로 맞추기 위해 나머지 부분을 0x00으로 채움
	AdjustInSectorSize(iTargetFd, iSourceSize + stHeader.dwHeaderSize);

	// 성공 메시지 출력
	printf("[INFO] Total %d Byte copy complete\n", iSourceSize);
	printf("[INFO] Package file create complete\n");

	_close(iTargetFd);

	return 0;
}


/**
*  현재 위치부터 512바이트 배수 위치까지 맞추어 0x00으로 채움
*/
int AdjustInSectorSize(int iFd, int iSourceSize)
{
	int i;
	int iAdjustSizeToSector;
	char cCh;
	int iSectorCount;

	iAdjustSizeToSector = iSourceSize % BYTESOFSECTOR;
	cCh = 0x00;

	if (iAdjustSizeToSector != 0)
	{
		iAdjustSizeToSector = 512 - iAdjustSizeToSector;
		for (i = 0; i < iAdjustSizeToSector; i++)
		{
			_write(iFd, &cCh, 1);
		}
	}
	else
	{
		printf("[INFO] File size is aligned 512 byte\n");
	}

	// 섹터 수를 되돌려줌
	iSectorCount = (iSourceSize + iAdjustSizeToSector) / BYTESOFSECTOR;
	return iSectorCount;
}

/**
*  소스 파일(Source FD)의 내용을 목표 파일(Target FD)에 복사하고 그 크기를 되돌려줌
*/
int CopyFile(int iSourceFd, int iTargetFd)
{
	int iSourceFileSize;
	int iRead;
	int iWrite;
	char vcBuffer[BYTESOFSECTOR];

	iSourceFileSize = 0;
	while (1)
	{
		iRead = _read(iSourceFd, vcBuffer, sizeof(vcBuffer));
		iWrite = _write(iTargetFd, vcBuffer, iRead);

		if (iRead != iWrite)
		{
			fprintf(stderr, "[ERROR] iRead != iWrite.. \n");
			exit(-1);
		}
		iSourceFileSize += iRead;

		if (iRead != sizeof(vcBuffer))
		{
			break;
		}
	}
	return iSourceFileSize;
}

