[BITS 64]
extern kmain64
SECTION .text
global main64
;링커 스크립터에 의해 2MB 영역에 로드되는 코드
main64:
	
	mov ax, 0x10
	mov ds, ax
	mov es, ax
	mov fs, ax
	mov gs, ax
	mov ss,  ax

	;스택 초기화	
	mov rbp, 0x1000f0
	mov rsp, 0x1000f0
	
	call kmain64
	
	jmp $