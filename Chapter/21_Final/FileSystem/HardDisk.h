#pragma once
#include "windef.h"
#include "Collect.H"
#include "HardDiskIO.h"

class HardDiskHandler
	{
	private:
		Collection <HDDInfo *> HDDs;
		void (*InterruptHandler)();
		static BYTE DoSoftwareReset(UINT16 deviceController);
		BYTE m_lastError;
	public:
		HardDiskHandler();
		~HardDiskHandler();
		
		void Initialize();

		BYTE GetTotalDevices();

		BOOLEAN IsRemovableDevice(BYTE * DPF);
		BOOLEAN IsRemovableMedia (BYTE * DPF);

		HDDInfo * GetHDDInfo(BYTE * DPF);

//주소모드 변환
		UINT32 CHSToLBA(BYTE *DPF, UINT32 Cylinder, UINT32 Head, UINT32 Sector);
		void LBAToCHS(BYTE *DPF, UINT32 LBA, UINT32 * Cylinder, UINT32 * Head, UINT32 * Sector);

		BYTE ReadSectors(BYTE * DPF, UINT16 StartCylinder, BYTE StartHead, BYTE StartSector, BYTE NoOfSectors, BYTE * buffer, BOOLEAN WithRetry = TRUE);
		BYTE ReadSectors(BYTE * DPF, UINT32 StartLBASector, BYTE NoOfSectors, BYTE * buffer, BOOLEAN WithRetry = TRUE);
		BYTE WriteSectors(BYTE * DPF, UINT16 StartCylinder, BYTE StartHead, BYTE StartSector, BYTE NoOfSectors, BYTE * Buffer, BOOLEAN WithRetry = TRUE);
	
		//가상 파일 시스템을 위한 메소드
		BYTE GetNoOfDevices();
		UINT16 GetDeviceParameters(BYTE * DPF, BYTE * Buffer);
		BYTE Reset(BYTE * DPF);		

		BYTE GetLastErrorCode()
		{
			return m_lastError;
		}
		char * GetLastError(BYTE errorCode);
		char * GetLastError()
		{
			return GetLastError(m_lastError);
		}
	};
extern HardDiskHandler* g_pHDDHandler;
