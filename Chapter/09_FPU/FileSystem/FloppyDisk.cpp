#include <hal.h>
#include "FloppyDisk.h"
#include "piT.h"
#include "SkyConsole.h"

static uint8_t	_CurrentDrive = 0;
static volatile bool _floppyDiskIRQ = false;


//플로피 디스크 인터럽트 핸들러
//플로피 디스크로부터 인터럽트가 발생했다는 것만 체크한다.
__declspec(naked) void FloppyDiskHandler() 
{
	_asm {
		pushad
		cli
	}

	_floppyDiskIRQ = true;
	//SkyConsole::Print("Floppy Interrupt\n");

	_asm {
		mov al, 0x20
		out 0x20, al
		popad
		sti
		iretd
	}
}


namespace FloppyDisk
{
	void ConvertLBAToCHS(int lba, int& head, int& track, int& sector) 
	{

		head = (lba % (FLPY_SECTORS_PER_TRACK * 2)) / (FLPY_SECTORS_PER_TRACK);
		track = lba / (FLPY_SECTORS_PER_TRACK * 2);
		sector = lba % FLPY_SECTORS_PER_TRACK + 1;
	}


	void Install(int irq) {

		setvect(irq, FloppyDiskHandler);

		Reset();

		//! set drive information
		ConfigureDriveData(13, 1, 0xf, true);
	}

	void SetWorkingDrive(uint8_t drive) {

		if (drive < 4)
			_CurrentDrive = drive;
	}

	uint8_t GetWorkingDrive() {

		return _CurrentDrive;
	}
	
	uint8_t* ReadSector(int sectorLBA) {

		if (_CurrentDrive >= 4)
			return 0;
		
		int head = 0, track = 0, sector = 1;
		ConvertLBAToCHS(sectorLBA, head, track, sector);

		//! turn motor on and seek to track
		ControlMotor(true);
		if (Seek((uint8_t)track, (uint8_t)head) != 0)
			return 0;

		//! read sector and turn motor off
		ReadSectorImpl((uint8_t)head, (uint8_t)track, (uint8_t)sector);
		ControlMotor(false);

		//! warning: this is a bit hackish
		return (uint8_t*)DMA_BUFFER;
	}

	void Reset() {

		uint32_t st0, cyl;

//FDC Reset
		DisableController();
		EnableController();
		WaitIrq();

		//! send CHECK_INT/SENSE INTERRUPT command to all drives
		for (int i = 0; i<4; i++)
			CheckInterrput(st0, cyl);
		
		WriteCCR(0);

		//! pass mechanical drive info. steprate=3ms, unload time=240ms, load time=16ms
		ConfigureDriveData(3, 16, 240, true);
		
		CalibrateDisk(_CurrentDrive);
	}

	
	void CheckInterrput(uint32_t& st0, uint32_t& cyl) 
	{

		SendCommand(FDC_CMD_CHECK_INT);

		st0 = ReadData();
		cyl = ReadData();
	}

	//플로피 드라이브의 모터를 켜거나 끈다
	void ControlMotor(bool b) 
	{
		//! sanity check: invalid drive
		if (_CurrentDrive > 3)
			return;

		uint8_t motor = 0;

		//! select the correct mask based on current drive
		switch (_CurrentDrive) {

		case 0:
			motor = FLPYDSK_DOR_MASK_DRIVE0_MOTOR;
			break;
		case 1:
			motor = FLPYDSK_DOR_MASK_DRIVE1_MOTOR;
			break;
		case 2:
			motor = FLPYDSK_DOR_MASK_DRIVE2_MOTOR;
			break;
		case 3:
			motor = FLPYDSK_DOR_MASK_DRIVE3_MOTOR;
			break;
		}

		//! turn on or off the motor of that drive
		if (b)
			WriteDOR(uint8_t(_CurrentDrive | motor | FLPYDSK_DOR_MASK_RESET | FLPYDSK_DOR_MASK_DMA));
		else
			WriteDOR(FLPYDSK_DOR_MASK_RESET);

		//! in all cases; wait a little bit for the motor to spin up/turn off
		msleep(20);
	}
	
	void ConfigureDriveData(uint8_t stepr, uint8_t loadt, uint8_t unloadt, bool dma) 
	{
		uint8_t data = 0;

		//! send command
		SendCommand(FDC_CMD_SPECIFY);
		data = ((stepr & 0xf) << 4) | (unloadt & 0xf);
		SendCommand(data);
		data = ((loadt << 1) | ((dma) ? 0 : 1));
		SendCommand(data);
	}

	int CalibrateDisk(uint8_t drive) 
	{

		uint32_t st0, cyl;

		if (drive >= 4)
			return -2;

		//! turn on the motor
		ControlMotor(true);

		for (int i = 0; i < 10; i++)
		{
			//! send command
			SendCommand(FDC_CMD_CALIBRATE);
			SendCommand(drive);
			WaitIrq();
			CheckInterrput(st0, cyl);

			//! did we fine cylinder 0? if so, we are done
			if (!cyl) {

				ControlMotor(false);
				return 0;
			}
		}

		ControlMotor(false);
		return -1;
	}

	void DisableController() {

		WriteDOR(0);
	}

	void EnableController() {

		WriteDOR(FLPYDSK_DOR_MASK_RESET | FLPYDSK_DOR_MASK_DMA);
	}



	bool SetupDMA()
	{
		WORD count = 512 * 18 - 1;

	// single mode, write, channel 2
		OutPortByte(0x0b, 0x46);

	//setup address = 0x00000000
		OutPortByte(0x04, 0x00);
		OutPortByte(0x04, 0x00);
		OutPortByte(0x81, 0x00);

	// setup count
		OutPortByte(0x05, (BYTE)count);
		OutPortByte(0x05, (BYTE)(count >> 8));

	// enable dma controller
	// clear mask bit, channel 2
		OutPortByte(0x0a, 0x02);

		return TRUE;
	}
	
	void ReadSectorImpl(uint8_t head, uint8_t track, uint8_t sector) 
	{

		uint32_t st0, cyl;

		SetupDMA();

		//! read in a sector
		SendCommand(FDC_CMD_READ_SECT | FDC_CMD_EXT_MULTITRACK | FDC_CMD_EXT_SKIP | FDC_CMD_EXT_DENSITY);
		SendCommand(head << 2 | _CurrentDrive);
		SendCommand(track);
		SendCommand(head);
		SendCommand(sector);
		SendCommand(FLPYDSK_SECTOR_DTL_512);
		SendCommand(((sector + 1) >= FLPY_SECTORS_PER_TRACK) ? FLPY_SECTORS_PER_TRACK : sector + 1);
		SendCommand(FLPYDSK_GAP3_LENGTH_3_5);
		SendCommand(0xff);

		WaitIrq();

		for (int j = 0; j<7; j++)
			ReadData();

		CheckInterrput(st0, cyl);
	}

	//주어진 실린더와 헤드가 존재하는지 그 결과를 리턴한다
	int Seek(uint8_t cyl, uint8_t head) 
	{

		uint32_t st0, cyl0;

		if (_CurrentDrive >= 4)
			return -1;

		for (int i = 0; i < 10; i++) 
		{			
			SendCommand(FDC_CMD_SEEK);
			SendCommand((head) << 2 | _CurrentDrive);
			SendCommand(cyl);
		
			WaitIrq();
			CheckInterrput(st0, cyl0);

			if (cyl0 == cyl)
				return 0;
		}

		return -1;
	}

	//(FDC)플로피 디스크 컨트롤러의 상태를 읽는다
	uint8_t ReadStatus() 
	{
		return InPortByte(FLPYDSK_MSR);
	}

	//DOR(Digital Output Register)
	void WriteDOR(uint8_t val) 
	{

		OutPortByte(FLPYDSK_DOR, val);
	}
	
	void SendCommand(uint8_t cmd) 
	{
		for (int i = 0; i < 500; i++)
			if (ReadStatus() & FLPYDSK_MSR_MASK_DATAREG)
				return OutPortByte(FLPYDSK_FIFO, cmd);
	}
	
	uint8_t ReadData() 
	{
		
		for (int i = 0; i < 500; i++)
			if (ReadStatus() & FLPYDSK_MSR_MASK_DATAREG)
				return InPortByte(FLPYDSK_FIFO);

		return 0;
	}

	//CCR(Configuation Control Register)
	void WriteCCR(uint8_t val) 
	{	
		OutPortByte(FLPYDSK_CTRL, val);
	}


	void WaitIrq() 
	{
		//__asm sti		

		while (_floppyDiskIRQ == false)
			;

		_floppyDiskIRQ = false;
	}
}
