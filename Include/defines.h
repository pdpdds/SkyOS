#pragma once

#define CODE_SELECTOR 0x8
 #define DATA_SELECTOR 0x10
 
 typedef void(_cdecl *I86_IRQ_HANDLER)(void);

#define ENTER_CRITICAL_SECTION()	__asm	PUSHFD	__asm CLI
#define EXIT_CRITICAL_SECTION()		__asm	POPFD

/*
0x00000000-0x00400000 ?Kernel reserved
0x00400000-0x80000000 ?User land
0x80000000-0xffffffff ?Kernel reserved
*/
#define KE_USER_START   0x00400000
#define KE_KERNEL_START 0x80000000

#define MAX_THREAD 5

#define KERNEL_CS				0x0008
#define KERNEL_DS				0x0010
#define USER_CS					0x001b
#define USER_DS					0x0023
#define TASK_SW_SEG				0x0028	/* TSS: instruction 'IRETD' transfers excution to this */
#define INIT_TSS_SEG			0x0030	/* TSS: default tss, used by 'LTR' */
#define TMR_TSS_SEG				0x0038	/* TSS: when occurred an interrupt from system timer 0 */
#define SOFT_TS_TSS_SEG			0x0040	/* TSS: task-switching driven by 'int' instruction */
#define SYSCALL_GATE			0x0048

#define KERNEL_SS				KERNEL_DS
#define USER_SS					USER_DS

#define PROCESS_USER 0
#define PROCESS_KERNEL 1

#define DEFAULT_HEAP_PAGE_COUNT 256

#define TASK_STATE_INIT		0 //태스크가 초기화된 상태
#define TASK_STATE_WAIT		1 //실행을 위해 대기중인 상태
#define TASK_STATE_RUNNING	2 //태스크가 실행중인 상태
#define TASK_STATE_SLEEP	3 //태스크가 태스크 스케쥴러에서 제외된 상태
#define TASK_STATE_END		4 //태스크가 종료된 상태   

#define TASK_RUNNING_TIME 2   //태스크가 CPU를 사용하는 시간

//! set access bit
#define I86_GDT_DESC_ACCESS			0x0001			//00000001

//! descriptor is readable and writable. default: read only
#define I86_GDT_DESC_READWRITE			0x0002			//00000010

//! set expansion direction bit
#define I86_GDT_DESC_EXPANSION			0x0004			//00000100

//! executable code segment. Default: data segment
#define I86_GDT_DESC_EXEC_CODE			0x0008			//00001000

//! set code or data descriptor. defult: system defined descriptor
#define I86_GDT_DESC_CODEDATA			0x0010			//00010000

//! set dpl bits
#define I86_GDT_DESC_DPL			0x0060			//01100000

//! set "in memory" bit
#define I86_GDT_DESC_MEMORY			0x0080			//10000000

/**	gdt descriptor grandularity bit flags	***/

//! masks out limitHi (High 4 bits of limit)
#define I86_GDT_GRAND_LIMITHI_MASK		0x0f			//00001111

//! set os defined bit
#define I86_GDT_GRAND_OS			0x10			//00010000

//! set if 32bit. default: 16 bit
#define I86_GDT_GRAND_32BIT			0x40			//01000000

//! 4k grandularity. default: none
#define I86_GDT_GRAND_4K			0x80			//10000000



#define SYSTEM_TMR_INT_NUMBER	0x20
#define SOFT_TASK_SW_INT_NUMBER	0x30
