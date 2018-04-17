//8259 Programmable Interrupt Controller
#pragma once
#include <stdint.h>

//PIC와 연결된 디바이스들. 인터럽트 리퀘스트를 활성화하거나 비활성화할 때
//해당 번호가 유용할 수 있다.

//인터럽트를 발생시키기 위해 PIC1을 사용하는 디바이스 리스트
#define		I86_PIC_IRQ_TIMER			0
#define		I86_PIC_IRQ_KEYBOARD		1
#define		I86_PIC_IRQ_SERIAL2			3
#define		I86_PIC_IRQ_SERIAL1			4
#define		I86_PIC_IRQ_PARALLEL2		5
#define		I86_PIC_IRQ_DISKETTE		6
#define		I86_PIC_IRQ_PARALLEL1		7

//인터럽트를 발생시키기 위해 PIC2를 사용하는 디바이스 리스트
#define		I86_PIC_IRQ_CMOSTIMER		0
#define		I86_PIC_IRQ_CGARETRACE		1
#define		I86_PIC_IRQ_AUXILIARY		4
#define		I86_PIC_IRQ_FPU				5
#define		I86_PIC_IRQ_HDC				6

//-----------------------------------------------
//디바이스를 제어하기 위한 커맨드
//-----------------------------------------------

//! Command Word 2 bit masks. 커맨드를 보낼때 사용
#define		I86_PIC_OCW2_MASK_L1		1		//00000001
#define		I86_PIC_OCW2_MASK_L2		2		//00000010
#define		I86_PIC_OCW2_MASK_L3		4		//00000100
#define		I86_PIC_OCW2_MASK_EOI		0x20	//00100000
#define		I86_PIC_OCW2_MASK_SL		0x40	//01000000
#define		I86_PIC_OCW2_MASK_ROTATE	0x80	//10000000

//! Command Word 3 bit masks. 커맨드를 보낼때 사용
#define		I86_PIC_OCW3_MASK_RIS		1		//00000001
#define		I86_PIC_OCW3_MASK_RIR		2		//00000010
#define		I86_PIC_OCW3_MASK_MODE		4		//00000100
#define		I86_PIC_OCW3_MASK_SMM		0x20	//00100000
#define		I86_PIC_OCW3_MASK_ESMM		0x40	//01000000
#define		I86_PIC_OCW3_MASK_D7		0x80	//10000000

//PIC로부터 1바이트를 읽는다
uint8_t ReadDataFromPIC(uint8_t picNum);

//PIC로 데이터를 보낸다.
void SendDataToPIC(uint8_t data, uint8_t picNum);

//PIC로 명령어를 전송한다.
void SendCommandToPIC(uint8_t cmd, uint8_t picNum);

//PIC 초기화
void PICInitialize(uint8_t base0, uint8_t base1);
