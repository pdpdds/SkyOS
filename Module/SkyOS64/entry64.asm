[BITS 64] ;64비트 코드

SECTION .text
extern kmain64 ; C++ 커널 엔트리포인트
global main64 ; PE포맷 엔트리 포인트

main64:	
; IA-32e 모드 커널 데이터 세그먼트 디스크립터 설정
; 32비트에서 64비트로 점프하면서 커널 세그먼트 디스크립터는 0X08로 설정된 상태
	mov ax, 0x10        
	mov ds, ax          
	mov es, ax          
	mov fs, ax          
	mov gs, ax          
	
	; 스택 설정 0x500000~0x600000
	mov ss, ax
	mov rsp, 0x600000   
	mov rbp, 0x600000  

	call kmain64 
	
	jmp $