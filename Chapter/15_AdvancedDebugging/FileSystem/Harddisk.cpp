#include "SkyOS.h"

extern void SendEOI();

__declspec(naked) void _HDDInterruptHandler() {

	//레지스터를 저장하고 인터럽트를 끈다.
	_asm
	{
		PUSHAD
		PUSHFD
		CLI
	}

	SendEOI();

	// 레지스터를 복원하고 원래 수행하던 곳으로 돌아간다.
	_asm
	{
		POPFD
		POPAD
		IRETD
	}
}

//발견된 하드디스크 개수를 리턴한다.
BYTE HardDiskHandler::GetTotalDevices()
{
	return (BYTE)HDDs.Count();
}
//---------------------------------------------------------------------
//        This function returns the description of the last error
//---------------------------------------------------------------------
char * HardDiskHandler::GetLastError(BYTE errorCode)
{
	switch (errorCode)
	{
	case HDD_NO_ERROR:
		return "No Error";
	case HDD_NOT_FOUND:
		return "HDD Not Found";
	case HDD_CONTROLLER_BUSY:
		return "Device Controller Busy";
	case HDD_DATA_NOT_READY:
		return "Device Data Not Ready";
	case HDD_DATA_COMMAND_NOT_READY:
		return "Device not ready";
	default:
		return "Undefined Error";
	}
}

//디바이스에 에러가 발생했을 경우 에러 레지스터의 값을 읽어들인다.
BYTE ReadErrorRegister(BYTE deviceController)
{
	BYTE Status = InPortByte(IDE_Con_IOBases[deviceController][0] + IDE_CB_STATUS);
	if ((Status & 0x80) == 0 && (Status & 0x1)) //busy bit=0 and err bit=1
	{
		Status = InPortByte(IDE_Con_IOBases[deviceController][0] + IDE_CB_ERROR);
		return Status;
	}
	else
		return 0;
}

//디바이스가 데이터를 전송하거나 받을 수 있는 준비가 되었는지 확인한다.
//deviceController : 테스트할 디바이스 컨트롤러의 인덱스 번호
//디바이스가 데이터관련 처리를 할 수 있는 준비가 되었으면 TRUE를 그렇지 않으면 FALSE를 리턴한다.
BOOLEAN IsDeviceDataReady(int deviceController, DWORD waitUpToms = 0, BOOLEAN checkDataRequest = TRUE)
{
	UINT32 Time1, Time2;
	Time1 = GetTickCount();
	do
	{
		UINT16 PortID = IDE_Con_IOBases[deviceController][0] + IDE_CB_STATUS;
		BYTE Status = InPortByte(PortID);
		if ((Status & 0x80) == 0) //Checking BSY bit, because DRDY bit is valid only when BSY is zero
		{
			if (Status & 0x40) //checking DRDY is set
				if (checkDataRequest) // if DataRequest is also needed
				{
					if (Status & 0x8) // DRQ bit set
					{
						return TRUE;
					}
				}
				else
				{
					return TRUE;
				}
		}
		Time2 = GetTickCount();
	} while ((Time2 - Time1) < waitUpToms);

	return FALSE;
}

//주어진 디바이스 컨트롤러를 사용할 수 있는지 없는지를 체크한다.
BOOLEAN IsDeviceControllerBusy(int DeviceController, int WaitUpToms = 0)
{
	UINT32 Time1, Time2;
	Time1 = GetTickCount();
	do {

		UINT16 PortID = IDE_Con_IOBases[DeviceController][0] + IDE_CB_STATUS;
		BYTE Status = InPortByte(PortID);
		if ((Status & 0x80) == 0) //BSY bit 
			return FALSE;
		Time2 = GetTickCount();
	} while ((Time2 - Time1) <= (UINT32)WaitUpToms);

	return TRUE;
}

//부착된 디바이스를 소프트웨어 리셋한다.
BYTE HardDiskHandler::DoSoftwareReset(UINT16 DeviceController)
{
	BYTE DeviceControl = 4; //SRST bit 제어 레지스터의 SRST 비트 필드에 값을 설정
	OutPortByte(IDE_Con_IOBases[DeviceController][0] + IDE_CON_DEVICE_CONTROL, DeviceControl);
	DeviceControl = 0;      //제어 레지스터의 SRST 비트값 클리어
	OutPortByte(IDE_Con_IOBases[DeviceController][0] + IDE_CON_DEVICE_CONTROL, DeviceControl);

	return InPortByte(IDE_Con_IOBases[DeviceController][0] + IDE_CB_ERROR);
}
BOOLEAN HardDiskHandler::IsRemovableDevice(BYTE * DPF)
{
	return !(HDDs.Item((char *)DPF)->DeviceID[0] & 0x70);
}
BOOLEAN HardDiskHandler::IsRemovableMedia(BYTE * DPF)
{
	return HDDs.Item((char *)DPF)->DeviceID[0] & 0x80;
}

HardDiskHandler::HardDiskHandler()
{

}

/*
초기화 메소드 : 모든 디바이스 컨트롤러를 확인해서 이용할 수 있는지 체크한다.
1) 디바이스 컨트롤러의 Busy 비트를 확인한다. 이 값이 설정되면 해당 디바이스 컨트롤러는 사용할 수 없다.
2) 디바이스를 진단하는 커맨드를 보낸다.
3) 특정시간 대기동안 Busy 비트가 클리어되면 디바이스 컨트롤러에 접근할 수 있다.
4) 에러 레지스터를 읽는다.
	a) 비트값이 0이면 마스터 디스크가 설치된 것을 의미
	b) 비트값이 7이면 슬레이브 디스크 설치되지 않음
5) DEV_HEAD 레지스터에 적당한 비트값을 설정한다.
6) 50ns 정도 대기한다.
7) 디바이스 커맨드를 보낸다.
8) 디바이스로부터 512바이트 정보값을 받는다.
*/
void HardDiskHandler::Initialize()
{
	char strKey[3] = "H0"; //하드디스크 ID
	
	//아무런 역할을 하지 않는 하드디스크 핸들러이지만 정의를 해야 한다.
	setvect(32 + 14, _HDDInterruptHandler);
	setvect(32 + 15, _HDDInterruptHandler);

	//Collection 구조체 발견한 하드디스크 정보 리스트를 관리한다.
	HDDs.Initialize();	

	//디바이스 컨트롤러를 통해 하드디스크를 찾는다.
	for (int DeviceController = 0; DeviceController < IDE_CONTROLLER_NUM; DeviceController++)
	{
		DoSoftwareReset(DeviceController); //소프트웨어 리셋
		if (IsDeviceControllerBusy(DeviceController, 1000)) //디바이스 컨트롤러를 사용할 수 없으면 패스한다.
			continue;
		
		//디바이스 진단 요청을 한다.
		OutPortByte(IDE_Con_IOBases[DeviceController][0] + IDE_CB_COMMAND, IDE_COM_EXECUTE_DEVICE_DIAGNOSTIC);
				
		BYTE result = InPortByte(IDE_Con_IOBases[DeviceController][0] + IDE_CB_ERROR);
		for (BYTE device = 0; device < 1; device++)         //마스터와 슬레이브 디스크에 대해 루프를 돈다.
		{
			UINT16 DeviceID_Data[512], j;
			
			//if (device == 0 && !(result & 1))
				//continue;

			if (device == 1 && (result & 0x80))
				continue;

			//디바이스 IO가 가능하다면
			if (device == 1)
				OutPortByte(IDE_Con_IOBases[DeviceController][0] + IDE_CB_DEVICE_HEAD, 0x10); //Setting 4th bit(count 5) to set device as 1
			else
				OutPortByte(IDE_Con_IOBases[DeviceController][0] + IDE_CB_DEVICE_HEAD, 0x0);

			//msleep(50);

			//디바이스 정보 요청
			OutPortByte(IDE_Con_IOBases[DeviceController][0] + IDE_CB_COMMAND, IDE_COM_IDENTIFY_DEVICE);
			if (!IsDeviceDataReady(DeviceController, 600, TRUE)) //디바이스 정보가 채워질때까지 대기한다.
			{
				SkyConsole::Print("Data not ready %d\n", DeviceController);
				continue;
			}

			//디바이스로 부터 512바이트 정보를 읽어들인다.
			for (j = 0; j < 256; j++)
				DeviceID_Data[j] = InPortWord(IDE_Con_IOBases[DeviceController][0] + IDE_CB_DATA);
			
			//HDD 노드 생성
			HDDInfo * newHDD = (HDDInfo *)kmalloc(sizeof(HDDInfo));
			if (newHDD == NULL)
			{
				SkyConsole::Print("HDD Initialize :: Allocation failed\n");
				return;
			}

			//HDD 노드에 디바이스 정보를 기록한다.
			newHDD->IORegisterIdx = DeviceController;
			memcpy(newHDD->DeviceID, DeviceID_Data, 512);
			newHDD->DeviceNumber = device;
			newHDD->LastError = 0;

			newHDD->BytesPerSector = 512; 

			newHDD->CHSCylinderCount = DeviceID_Data[1];
			newHDD->CHSHeadCount = DeviceID_Data[3];
			newHDD->CHSSectorCount = DeviceID_Data[6];

			if (DeviceID_Data[10] == 0)
				strcpy(newHDD->SerialNumber, "N/A");
			else
				for (j = 0; j < 20; j += 2)
				{
					newHDD->SerialNumber[j] = DeviceID_Data[10 + (j / 2)] >> 8;
					newHDD->SerialNumber[j + 1] = (DeviceID_Data[10 + (j / 2)] << 8) >> 8;
				}
			if (DeviceID_Data[23] == 0)
				strcpy(newHDD->FirmwareRevision, "N/A");
			else
				for (j = 0; j < 8; j += 2)
				{
					newHDD->FirmwareRevision[j] = DeviceID_Data[23 + (j / 2)] >> 8;
					newHDD->FirmwareRevision[j + 1] = (DeviceID_Data[23 + (j / 2)] << 8) >> 8;
				}

			if (DeviceID_Data[27] == 0)
				strcpy(newHDD->ModelNumber, "N/A");
			else
				for (j = 0; j < 20; j += 2)
				{
					newHDD->ModelNumber[j] = DeviceID_Data[27 + (j / 2)] >> 8;
					newHDD->ModelNumber[j + 1] = (DeviceID_Data[27 + (j / 2)] << 8) >> 8;
				}
			newHDD->LBASupported = DeviceID_Data[49] & 0x200;
			newHDD->DMASupported = DeviceID_Data[49] & 0x100;

			UINT32 LBASectors = DeviceID_Data[61];
			LBASectors = LBASectors << 16;
			LBASectors |= DeviceID_Data[60];			
			newHDD->LBACount = LBASectors;
			HDDs.Add(newHDD, strKey);

			SkyConsole::Print("DeviceId : %x, %s\n", device, newHDD->ModelNumber);
			strKey[1]++; //새 하드디스크 노드를 위해 하드디스크 ID를 변경한다.
		}
	}
}

HardDiskHandler::~HardDiskHandler()
{
	HDDs.Clear();
}
HDDInfo * HardDiskHandler::GetHDDInfo(BYTE * DPF)
{

	HDDInfo * getHDD, *retHDD = (HDDInfo *)kmalloc(sizeof(HDDInfo));
	getHDD = HDDs.Item((char *)DPF);
	if (getHDD == NULL)
	{
		m_lastError = HDD_NOT_FOUND;
		return NULL;
	}
	memcpy(retHDD, getHDD, sizeof(HDDInfo));
	return retHDD;
}
/* 섹터로부터 데이터를 읽어들인다(CHS 모드)
1) HDDInfo 객체값을 얻는다.
2) 디바이스를 사용할 수 있는지 확인한다.
3) 디바이스 비트를 설정한다.
4) 디바이스가 데이터 커맨드를 받아들일 준비가 되었는지 확인한다.
5) 헤드와 트랙, 기타값들을 설정한다.
6) 읽기 커맨드를 보낸다.
7) 디바이스가 데이터 전송을 할 수 있는 준비가 되었는지 확인한다.
8) 데이터를 읽기 위해 데이터 레지스터를 읽는다.
*/
BYTE HardDiskHandler::ReadSectors(BYTE * DPF, UINT16 StartCylinder, BYTE StartHead, BYTE StartSector, BYTE NoOfSectors, BYTE * buffer, BOOLEAN WithRetry)
{
	HDDInfo * pHDDInfo;
	BYTE DevHead, StartCylHigh = 0, StartCylLow = 0;

	//하드디스크 아이디로 부터 하드디스크정보를 얻어낸다.
	pHDDInfo = HDDs.Item((char *)DPF);
	if (pHDDInfo == NULL)
	{
		m_lastError = HDD_NOT_FOUND;
		return HDD_NOT_FOUND;
	}

	if (pHDDInfo->DeviceNumber == 0)
		DevHead = StartHead | 0xA0;
	else
		DevHead = StartHead | 0xB0;

	//디바이스가 준비될때 까지 대기한다.
	if (IsDeviceControllerBusy(pHDDInfo->IORegisterIdx, 1 * 60))
	{
		m_lastError = HDD_CONTROLLER_BUSY;
		return HDD_CONTROLLER_BUSY;
	}

	//디바이스가 데이터 커맨드를 받아들일 준비가 되었는지 확인한다.
	OutPortByte(IDE_Con_IOBases[pHDDInfo->IORegisterIdx][0] + IDE_CB_DEVICE_HEAD, DevHead);

	if (!IsDeviceDataReady(pHDDInfo->IORegisterIdx, 1 * 60, FALSE))
	{
		m_lastError = HDD_DATA_COMMAND_NOT_READY;
		return HDD_DATA_COMMAND_NOT_READY;
	}

	StartCylHigh = StartCylinder >> 8;
	StartCylLow = (StartCylinder << 8) >> 8;

	//읽어들일 데이터의 위치를 지정한다. 실린더 위치, 섹터 시작위치, 읽어들일 섹터의 수
	OutPortByte(IDE_Con_IOBases[pHDDInfo->IORegisterIdx][0] + IDE_CB_CYLINDER_HIGH, StartCylHigh);
	OutPortByte(IDE_Con_IOBases[pHDDInfo->IORegisterIdx][0] + IDE_CB_CYLINDER_LOW, StartCylLow);
	OutPortByte(IDE_Con_IOBases[pHDDInfo->IORegisterIdx][0] + IDE_CB_SECTOR, StartSector);
	OutPortByte(IDE_Con_IOBases[pHDDInfo->IORegisterIdx][0] + IDE_CB_SECTOR_COUNT, NoOfSectors);

	OutPortByte(IDE_Con_IOBases[pHDDInfo->IORegisterIdx][0] + IDE_CB_COMMAND, WithRetry ? IDE_COM_READ_SECTORS_W_RETRY : IDE_COM_READ_SECTORS);

	//요청한 섹터수만큼 데이터를 읽어들인다.
	for (BYTE j = 0; j < NoOfSectors; j++)
	{
		//디바이스에 데이터가 준비되었는가?
		if (!IsDeviceDataReady(pHDDInfo->IORegisterIdx, 1 * 60, TRUE))
		{
			m_lastError = HDD_DATA_NOT_READY;
			return HDD_DATA_NOT_READY;
		}

		// 이 루프틀 통해 섹터 크기인 512바이트를 버퍼에 기록할 수 있다.
		for (UINT16 i = 0; i < (pHDDInfo->BytesPerSector) / 2; i++)
		{
			UINT16 w = 0;
			BYTE l, h;

			//2바이트를 읽는다.
			w = InPortWord(IDE_Con_IOBases[pHDDInfo->IORegisterIdx][0] + IDE_CB_DATA);
			l = (w << 8) >> 8;
			h = w >> 8;
			
			//2바이트를 쓴다.
			buffer[(j * (pHDDInfo->BytesPerSector)) + (i * 2)] = l;
			buffer[(j * (pHDDInfo->BytesPerSector)) + (i * 2) + 1] = h;
		}
	}
	return HDD_NO_ERROR;
}

// 섹터로부터 데이터를 읽어들인다(LBA 모드)
// 읽어들이는 루틴은 CHS 모드와 동일하다.
BYTE HardDiskHandler::ReadSectors(BYTE * DPF, UINT32 StartLBASector, BYTE NoOfSectors, BYTE * Buffer, BOOLEAN WithRetry)
{
	HDDInfo * HDD;
	BYTE LBA0_7, LBA8_15, LBA16_23, LBA24_27;

	HDD = HDDs.Item((char *)DPF);
	if (HDD == NULL)
	{
		m_lastError = HDD_NOT_FOUND;
		return HDD_NOT_FOUND;
	}
	LBA0_7 = (StartLBASector << 24) >> 24;
	LBA8_15 = (StartLBASector << 16) >> 24;
	LBA16_23 = (StartLBASector << 8) >> 24;
	LBA24_27 = (StartLBASector << 4) >> 28;

	if (HDD->DeviceNumber == 0)
		LBA24_27 = LBA24_27 | 0xE0;
	else
		LBA24_27 = LBA24_27 | 0xF0;

	if (IsDeviceControllerBusy(HDD->IORegisterIdx, 1 * 60))
	{
		m_lastError = HDD_CONTROLLER_BUSY;
		return HDD_CONTROLLER_BUSY;
	}

	OutPortByte(IDE_Con_IOBases[HDD->IORegisterIdx][0] + IDE_CB_DEVICE_HEAD, LBA24_27);

	if (!IsDeviceDataReady(HDD->IORegisterIdx, 1 * 60, FALSE))
	{
		m_lastError = HDD_DATA_COMMAND_NOT_READY;
		return HDD_DATA_COMMAND_NOT_READY;
	}

	OutPortByte(IDE_Con_IOBases[HDD->IORegisterIdx][0] + IDE_CB_LBA_16_23, LBA16_23);
	OutPortByte(IDE_Con_IOBases[HDD->IORegisterIdx][0] + IDE_CB_LBA_8_15, LBA8_15);
	OutPortByte(IDE_Con_IOBases[HDD->IORegisterIdx][0] + IDE_CB_LBA_0_7, LBA0_7);
	OutPortByte(IDE_Con_IOBases[HDD->IORegisterIdx][0] + IDE_CB_SECTOR_COUNT, NoOfSectors);

	OutPortByte(IDE_Con_IOBases[HDD->IORegisterIdx][0] + IDE_CB_COMMAND, WithRetry ? IDE_COM_READ_SECTORS_W_RETRY : IDE_COM_READ_SECTORS);
	for (BYTE j = 0; j < NoOfSectors; j++)
	{
		if (!IsDeviceDataReady(HDD->IORegisterIdx, 1 * 60, TRUE))
		{
			m_lastError = HDD_DATA_NOT_READY;
			return HDD_DATA_NOT_READY;
		}

		for (UINT16 i = 0; i < (HDD->BytesPerSector) / 2; i++)
		{
			UINT16 w = 0;
			BYTE l, h;
			w = InPortWord(IDE_Con_IOBases[HDD->IORegisterIdx][0] + IDE_CB_DATA);
			l = (w << 8) >> 8;
			h = w >> 8;
			Buffer[(j * (HDD->BytesPerSector)) + (i * 2)] = l;
			Buffer[(j * (HDD->BytesPerSector)) + (i * 2) + 1] = h;
		}
	}
	return HDD_NO_ERROR;
}


/*섹터에 데이터를 쓴다.
1) HDDInfo 객체를 얻어낸다.
2) 디바이스를 사용할 수 있는지 체크한다.
3) 디바이스 비트를 설정한다.
4) 디바이스가 데이터 커맨드를 받아들일 수 있는지 체크한다.
5) 헤드, 트랙, 기타 정보를 설정한다.
6) 쓰기 커맨드를 전송한다.
7) 디바이스가 데이터를 읽을 준비가 되었는지 체크한다.
8) 데이터를 전송하기 위해 데이터 레지스터에 데이터를 기록한다.
*/
BYTE HardDiskHandler::WriteSectors(BYTE * DPF, UINT16 StartCylinder, BYTE StartHead, BYTE dwStartLBASector, BYTE NoOfSectors, BYTE * lpBuffer, BOOLEAN WithRetry)
{
	HDDInfo * pHDDInfo;
	BYTE LBA0_7, LBA8_15, LBA16_23, LBA24_27;

	pHDDInfo = HDDs.Item((char *)DPF);
	if (pHDDInfo == NULL)
	{
		m_lastError = HDD_NOT_FOUND;
		return HDD_NOT_FOUND;
	}

	LBA0_7 = (dwStartLBASector << 24) >> 24;
	LBA8_15 = (dwStartLBASector << 16) >> 24;
	LBA16_23 = (dwStartLBASector << 8) >> 24;
	LBA24_27 = (dwStartLBASector << 4) >> 28;

	if (pHDDInfo->DeviceNumber == 0)
		LBA24_27 = LBA24_27 | 0xE0;
	else
		LBA24_27 = LBA24_27 | 0xF0;

	if (IsDeviceControllerBusy(pHDDInfo->IORegisterIdx, 400))
	{		
		SetLastError(ERROR_BUSY);
		return HDD_CONTROLLER_BUSY;
	}

	OutPortByte(IDE_Con_IOBases[pHDDInfo->IORegisterIdx][0] + IDE_CB_DEVICE_HEAD, LBA24_27);

	if (!IsDeviceDataReady(pHDDInfo->IORegisterIdx, 1000, FALSE))
	{
		SetLastError(ERROR_NOT_READY);
		return HDD_DATA_COMMAND_NOT_READY;
	}
	OutPortByte(IDE_Con_IOBases[pHDDInfo->IORegisterIdx][0] + IDE_CB_LBA_16_23, LBA16_23);
	OutPortByte(IDE_Con_IOBases[pHDDInfo->IORegisterIdx][0] + IDE_CB_LBA_8_15, LBA8_15);
	OutPortByte(IDE_Con_IOBases[pHDDInfo->IORegisterIdx][0] + IDE_CB_LBA_0_7, LBA0_7);
	OutPortByte(IDE_Con_IOBases[pHDDInfo->IORegisterIdx][0] + IDE_CB_SECTOR_COUNT, NoOfSectors);

	OutPortByte(IDE_Con_IOBases[pHDDInfo->IORegisterIdx][0] + IDE_CB_COMMAND, IDE_COM_WRITE_SECTORS_W_RETRY);
	for (UINT16 j = 0; j < NoOfSectors; j++)
	{
		if (!IsDeviceDataReady(pHDDInfo->IORegisterIdx, 1000, TRUE))
		{
			SetLastError(ERROR_NOT_READY);
			return HDD_DATA_NOT_READY;
		}
		for (UINT16 i = 0; i < pHDDInfo->BytesPerSector / 2; i++)
		{
			OutPortWord(IDE_Con_IOBases[pHDDInfo->IORegisterIdx][0] + IDE_CB_DATA, ((UINT16 *)lpBuffer)[(j * (pHDDInfo->BytesPerSector) / 2) + i]);
		}
	}

	return HDD_NO_ERROR;
}

BYTE HardDiskHandler::GetNoOfDevices()
{
	return GetTotalDevices();
}

//특정 디바이스의 파라메터 정보를 얻어낸다.
UINT16 HardDiskHandler::GetDeviceParameters(BYTE * DPF, BYTE * pBuffer)
{
	VFS_IO_PARAMETER deviceInfo;

	HDDInfo * getHDD;
	getHDD = this->HDDs.Item((char *)DPF);
	if (getHDD == NULL)
	{
		this->m_lastError = HDD_NOT_FOUND;
		return HDD_NOT_FOUND;
	}

	deviceInfo.Cylinder = getHDD->CHSCylinderCount;
	deviceInfo.Head = getHDD->CHSHeadCount;
	deviceInfo.Sector = getHDD->CHSSectorCount;
	deviceInfo.LBASector = getHDD->LBACount;
	memcpy(pBuffer, &deviceInfo, sizeof(VFS_IO_PARAMETER));

	return HDD_NO_ERROR;
}

//주어진 디바이스 컨트롤러를 리셋하고 그 결과를 리턴한다.
//디바이스 컨트롤러의 리셋은 DoSoftwareReset 메소드에서 수행한다.
BYTE HardDiskHandler::Reset(BYTE * DPF)
{
	HDDInfo * getHDD;
	getHDD = this->HDDs.Item((char *)DPF);
	if (getHDD == NULL)
	{
		this->m_lastError = HDD_NOT_FOUND;
		return 0;
	}
	return this->DoSoftwareReset(getHDD->IORegisterIdx);

}

//주소 모드 변경 CHS => LBA
UINT32 HardDiskHandler::CHSToLBA(BYTE * DPF, UINT32 Cylinder, UINT32 Head, UINT32 Sector)
{
	HDDInfo * getHDD;
	getHDD = this->HDDs.Item((char *)DPF);

	return (Sector - 1) + (Head*getHDD->CHSSectorCount) + (Cylinder * (getHDD->CHSHeadCount + 1) * getHDD->CHSSectorCount);
}

//주소 모드 변경 LBA => CHS
void HardDiskHandler::LBAToCHS(BYTE * DPF, UINT32 LBA, UINT32 * Cylinder, UINT32 * Head, UINT32 * Sector)
{
	HDDInfo * getHDD;
	getHDD = this->HDDs.Item((char *)DPF);

	*Sector = ((LBA % getHDD->CHSSectorCount) + 1);
	UINT32 CylHead = (LBA / getHDD->CHSSectorCount);
	*Head = (CylHead % (getHDD->CHSHeadCount + 1));
	*Cylinder = (CylHead / (getHDD->CHSHeadCount + 1));
}
