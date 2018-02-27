/**
 *  file    RAMDisk.h
 *  date    2009/05/22
 *  author  kkamagui 
 *  brief   램 디스크에 관련된 함수를 정의한 헤더 파일
 *          Copyright(c)2008 All rights reserved by kkamagui
 */

#ifndef __RAMDISK_H__
#define __RAMDISK_H__

#include "windef.h"

#define FILESYSTEM_SIGNATURE                0x7E38CF10
 // 클러스터의 크기(섹터 수), 4Kbyte
#define FILESYSTEM_SECTORSPERCLUSTER        8
 // 파일 클러스터의 마지막 표시
#define FILESYSTEM_LASTCLUSTER              0xFFFFFFFF
 // 빈 클러스터 표시
#define FILESYSTEM_FREECLUSTER              0x00
 // 루트 디렉터리에 있는 최대 디렉터리 엔트리의 수
#define FILESYSTEM_MAXDIRECTORYENTRYCOUNT   ( ( FILESYSTEM_SECTORSPERCLUSTER * 512 ) / \
        sizeof( DIRECTORYENTRY ) )
 // 클러스터의 크기(바이트 수)
#define FILESYSTEM_CLUSTERSIZE              ( FILESYSTEM_SECTORSPERCLUSTER * 512 )

 // 핸들의 최대 개수, 최대 태스크 수의 3배로 생성
#define FILESYSTEM_HANDLE_MAXCOUNT          ( TASK_MAXCOUNT * 3 )

 // 파일 이름의 최대 길이
#define FILESYSTEM_MAXFILENAMELENGTH        24

 // 핸들의 타입을 정의
#define FILESYSTEM_TYPE_FREE                0
#define FILESYSTEM_TYPE_FILE                1
#define FILESYSTEM_TYPE_DIRECTORY           2

 // SEEK 옵션 정의
#define FILESYSTEM_SEEK_SET                 0
#define FILESYSTEM_SEEK_CUR                 1
#define FILESYSTEM_SEEK_END                 2

#pragma pack( push, 1 )

 // 파티션 자료구조
typedef struct kPartitionStruct
{
	// 부팅 가능 플래그. 0x80이면 부팅 가능을 나타내며 0x00은 부팅 불가
	BYTE bBootableFlag;
	// 파티션의 시작 어드레스. 현재는 거의 사용하지 않으며 아래의 LBA 어드레스를 대신 사용
	BYTE vbStartingCHSAddress[3];
	// 파티션 타입
	BYTE bPartitionType;
	// 파티션의 마지막 어드레스. 현재는 거의 사용 안 함
	BYTE vbEndingCHSAddress[3];
	// 파티션의 시작 어드레스. LBA 어드레스로 나타낸 값
	DWORD dwStartingLBAAddress;
	// 파티션에 포함된 섹터 수
	DWORD dwSizeInSector;
} _PARTITION;


// MBR 자료구조
typedef struct kMBRStruct
{
	// 부트 로더 코드가 위치하는 영역
	BYTE vbBootCode[430];

	// 파일 시스템 시그너처, 0x7E38CF10
	DWORD dwSignature;
	// 예약된 영역의 섹터 수
	DWORD dwReservedSectorCount;
	// 클러스터 링크 테이블 영역의 섹터 수
	DWORD dwClusterLinkSectorCount;
	// 클러스터의 전체 개수
	DWORD dwTotalClusterCount;

	// 파티션 테이블
	_PARTITION vstPartition[4];

	// 부트 로더 시그너처, 0x55, 0xAA
	BYTE vbBootLoaderSignature[2];
} MBR;

 // HDD에 대한 정보를 나타내는 구조체
typedef struct kHDDInformationStruct
{
	// 설정값
	WORD wConfiguation;

	// 실린더 수
	WORD wNumberOfCylinder;
	WORD wReserved1;

	// 헤드 수
	WORD wNumberOfHead;
	WORD wUnformattedBytesPerTrack;
	WORD wUnformattedBytesPerSector;

	// 실린더당 섹터 수
	WORD wNumberOfSectorPerCylinder;
	WORD wInterSectorGap;
	WORD wBytesInPhaseLock;
	WORD wNumberOfVendorUniqueStatusWord;

	// 하드 디스크의 시리얼 넘버
	WORD vwSerialNumber[10];
	WORD wControllerType;
	WORD wBufferSize;
	WORD wNumberOfECCBytes;
	WORD vwFirmwareRevision[4];

	// 하드 디스크의 모델 번호
	WORD vwModelNumber[20];
	WORD vwReserved2[13];

	// 디스크의 총 섹터 수
	DWORD dwTotalSectors;
	WORD vwReserved3[196];
} HDDINFORMATION;

#pragma pack( pop )

////////////////////////////////////////////////////////////////////////////////
//
// 매크로
//
////////////////////////////////////////////////////////////////////////////////
// 램 디스크의 총 섹터 수. 8Mbyte 크기로 생성
#define RDD_TOTALSECTORCOUNT        ( 8 * 1024 * 1024 / 512)

////////////////////////////////////////////////////////////////////////////////
//
// 구조체
//
////////////////////////////////////////////////////////////////////////////////
// 1바이트로 정렬
#pragma pack( push, 1 )

// 램 디스크의 자료구조를 저장하는 구조체
typedef struct kRDDManagerStruct
{
    // 램 디스크용으로 할당 받은 메모리 영역
    BYTE* pbBuffer;
    
    // 총 섹터 수
    DWORD dwTotalSectorCount;
} RDDMANAGER;

#pragma pack( pop)

////////////////////////////////////////////////////////////////////////////////
//
//  함수
//
////////////////////////////////////////////////////////////////////////////////
bool kInitializeRDD( DWORD dwTotalSectorCount );
bool kReadRDDInformation(bool bPrimary, bool bMaster,
        HDDINFORMATION* pstHDDInformation );
int kReadRDDSector(bool bPrimary, bool bMaster, DWORD dwLBA, int iSectorCount,
        char* pcBuffer );
int kWriteRDDSector(bool bPrimary, bool bMaster, DWORD dwLBA, int iSectorCount,
        char* pcBuffer );

#endif /*__RAMDISK_H__*/
