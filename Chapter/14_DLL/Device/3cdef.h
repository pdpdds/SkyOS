#pragma once
#include "windef.h"

// internal definition
#define RX_FILTER_ALL			(UINT16)0x08	// receive all packets (promiscuous mode)
#define FULL_DUPLEX_ENABLE		0x20
#define MAX_UPDS				16
#define MAX_DPDS				16
#define FRAG_SIZE				4096
#define MAX_DN_FRAG				1				// the MAX VALUE is 63
#define MAX_UP_FRAG				1				// the MAX VALUE is 63
												
// acknowledge bit definition
#define ACK_BIT_interruptLatch	0x0001
#define ACK_BIT_rxEarly			0x0020
#define ACK_BIT_intRequest		0x0040
#define ACK_BIT_dnComplete		0x0200		  
#define ACK_BIT_upComplete		0x0400

// command definition
#define CV_SetRxFilter				(UINT16)0x8000
#define CV_SetInterruptEnable		(UINT16)0x7000
#define CV_SetIndicationEnable		(UINT16)0x7800
#define CV_UpStall					(UINT16)0x3000
#define CV_UpUnStall				(UINT16)0x3001
#define CV_RxEnable					(UINT16)0x2000
#define CV_TxEnable					(UINT16)0x4800
#define CV_RequestInterrupt			(UINT16)0x6000
#define CV_RxReset					(UINT16)0x2800
#define CV_TxReset					(UINT16)0x3800
#define CV_AckInterrupt				(UINT16)0x6800

// interrupt bit definition
#define INT_BIT_interruptLatch		0x01
#define INT_BIT_hostError			0x02
#define INT_BIT_txComplete			0x04
#define INT_BIT_rxComplete			0x10
#define INT_BIT_rxEarly				0x20
#define INT_BIT_intRequested		0x40
#define INT_BIT_updateStats			0x80
#define INT_BIT_linkEvent			0x100
#define INT_BIT_dnComplete			0x200
#define INT_BIT_upComplete			0x400
#define INT_BIT_cmdInProgress		0x1000
												
// register information structure
typedef struct {
	int nRegName;
	int nWindow;
	int nOffset;
	int nSize;
} RegInfo3c905bStt;

// register constants
typedef enum {
	RN_BiosRomAddr			= 0,
	RN_BiosRomData			,
	RN_EepromCommand		,
	RN_EepromData			,
	RN_IntStatus			,
	RN_Command				,		 
	RN_StationAddrLo		,
	RN_StationAddrMid		,
	RN_StationAddrHi		,
	RN_StationMaskLo		,
	RN_StationMaskMid		,
	RN_StationMaskHi		,
	RN_resetOptions			,
	RN_InternalConfig		,
	RN_MaxPktSize			,
	RN_MacControl			,
	RN_MediaOptions			,
	RN_RxFree				,
	RN_TxFree				,
	RN_FifoDiagnostic		,
	RN_NetworkDiagnostic	,
	RN_PhysicalMgmt			,
	RN_MediaStatus			,
	RN_BadSSD				,
	RN_UpperByteOk			,
	RN_TxStartThresh		,
	RN_RxEarlyThresh		,
	RN_RxFilter				,
	RN_TxReclaimThresh		,
	RN_InterruptEnable		,
	RN_IndicationEnable		,
	RN_CarrierLost			,
	RN_SeqError				,
	RN_MultipleCollisions	,
	RN_SingleCollisions		,
	RN_LateCollisions		,
	RN_RxOverruns			,
	RN_FramesXmittedOk		,
	RN_FramesRcvdOk			,
	RN_framesDeferred		,
	RN_UpperFramesOk		,
	RN_BytesRcvdOk			,
	RN_ByteXmittedOk		,
	RN_VlanMask				,
	RN_VlanEtherType		,
	RN_PowerMgmtEvent		,

	RN_UpListPtr			,
							
	END_OF_REG_NAME_CONST
} REG_NAME_CONST;

typedef struct {
	DWORD	dwAddr;				   // physical address
	DWORD	dwLength;
} FragStt;

// type 0 DPD
typedef struct {
	DWORD		dwDnNextPtr;	   // physical address
	DWORD		dwFrameStartHeader;
	DWORD		dwRsv;
	FragStt		frag[ MAX_DN_FRAG ];
	DWORD		dwZero1;
	DWORD		dwZero2;
} DPD0Stt;	

// type 1 DPD
typedef struct {
	DWORD		dwDnNextPtr;	   // physical address
	DWORD		dwScheduleTime;
	DWORD		dwFrameStartHeader;
	DWORD		dwRsv;
	FragStt		frag[ MAX_DN_FRAG ];
	DWORD		dwZero1;
	DWORD		dwZero2;
} DPD1;	

// type UPD	( it must be aligned by 8 bytes)
typedef struct {
	DWORD		dwUpNextPtr;			// physical address
	DWORD		dwUpPktStatus;
	FragStt		frag[ MAX_UP_FRAG ];	// 2*8 bytes
	DWORD		dwZero1;
	DWORD		dwZero2;
} UPD;								// 16+16 bytes

// upd control struct
typedef struct {
	int		nTotalPage;		// total number of the allocated pages
	void	*pPage;			// address of the allocated pages
	int		nTotalUPD;		// total number of the upds
	UPD	*pUPD;			// address of the upd array
	char	*pFrag;			// address of the fragment array
} UPDControl;