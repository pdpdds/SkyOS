#pragma once
#include <stdint.h>
#include "FloppyDiskIO.h"

namespace FloppyDisk
{
	void Install(int irq);
	void Reset();

	void SetWorkingDrive(uint8_t drive);
	uint8_t GetWorkingDrive();

//섹터를 읽어들인다
	uint8_t* ReadSector(int sectorLBA);

/////////////////////////////////////////////////////////////////////////
//LBA 주소를 to CHS(Cylinder : Header : Sector) 형태로 변환한다
	void ConvertLBAToCHS(int lba, int& head, int& track, int& sector);

	void ControlMotor(bool b);
	void ReadSectorImpl(uint8_t head, uint8_t track, uint8_t sector);
	void DisableController();
	void EnableController();
	void CheckInterrput(uint32_t& st0, uint32_t& cyl);
	bool SetupDMA();
	void WaitIrq();
	uint8_t ReadData();
	void SendCommand(uint8_t cmd);
	void WriteDOR(uint8_t val);
	void WriteCCR(uint8_t val);
	int Seek(uint8_t cyl, uint8_t head);
	void ConfigureDriveData(uint8_t stepr, uint8_t loadt, uint8_t unloadt, bool dma);
	int CalibrateDisk(uint8_t drive);
};