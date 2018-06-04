[BITS 32]      

global _SwitchAndExecute64bitKernel

SECTION .data       ; 텍스트 섹션이 되어야 하지만 편법으로 데이터섹션에 위치시킨다.
                    ; 텍스트 섹션으로 설정할 경우 어셈블리 오브젝트가 프로그램 앞단에 배치되는 걸 막을 방법이 없기 때문이다.

; IA-32e 모드로 전환하고 64비트 커널을 수행
;   PARAM: INT pmt4EntryAddress, INT kernelAddress, void* grubInfo
_SwitchAndExecute64bitKernel:
	push ebp        
    mov ebp, esp
	mov ebx, dword [ ebp + 8 ]  ; 파라미터 1(pmt4EntryAddress)
	mov edx, dword [ ebp + 12 ]  ; 파라미터 2(kernelAddress)
	mov [kernelAddress], edx

	lgdt [GDTR]
    ;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
    ; CR4 컨트롤 레지스터의 PAE 비트를 1로 설정
    ;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
    mov eax, cr4    ; CR4 컨트롤 레지스터의 값을 EAX 레지스터에 저장
    or eax, 0x20    ; PAE 비트(비트 5)를 1로 설정
    mov cr4, eax    ; PAE 비트가 1로 설정된 값을 CR4 컨트롤 레지스터에 저장
    
    ;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
    ; CR3 컨트롤 레지스터에 PML4 테이블의 어드레스 및 캐시 활성화
    ;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
    mov eax, ebx   ; EAX 레지스터에 PML4 테이블이 존재하는 주소(pmt4EntryAddress)를 저장
    mov cr3, eax        ; CR3 컨트롤 레지스터에 kernelAddress를 저장
    
    ;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
    ; IA32_EFER.LME를 1로 설정하여 IA-32e 모드를 활성화
    ;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
    mov ecx, 0xC0000080 ; IA32_EFER MSR 레지스터의 어드레스를 저장
    rdmsr               ; MSR 레지스터를 읽기
    
    or eax, 0x0100      ; EAX 레지스터에 저장된 IA32_EFER MSR의 하위 32비트에서 
                        ; LME 비트(비트 8)을 1로 설정
    wrmsr               ; MSR 레지스터에 쓰기
        
    ;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
    ; CR0 컨트롤 레지스터를 NW 비트(비트 29) = 0, CD 비트(비트 30) = 0, PG 비트(비트 31) = 1로
    ; 설정하여 캐시 기능과 페이징 기능을 활성화
    ;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
    mov eax, cr0            ; EAX 레지스터에 CR0 컨트롤 레지스터를 저장
    or eax, 0xE0000000      ; NW 비트(비트 29), CD 비트(비트 30), PG 비트(비트 31)을 모두 1로 설정
    xor eax, 0x60000000     ; NW 비트(비트 29)와 CD 비트(비트 30)을 XOR하여 0으로 설정
    mov cr0, eax            ; NW 비트 = 0, CD 비트 = 0, PG 비트 = 1로 설정한 값을 다시 
                            ; CR0 컨트롤 레지스터에 저장
    
    jmp 0x08:jmp_64k  ; CS 세그먼트 셀렉터를 IA-32e 모드용 코드 세그먼트 디스크립터로							
	jmp_64k:;이하 아래 코드는 32비트로 컴파일되었지만 실제로는 64비트 코드다. 파라메터를 넘기기 위한 트릭 코드
	mov	ecx, [ ebp + 16 ];
	dd(0);
	mov	eax, [kernelAddress];
	dd(0); 	
	jmp	eax;
		
kernelAddress:
	dd 0
                            
    ; 여기는 실행되지 않음
    jmp $

	

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;	데이터 영역
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
; 아래의 데이터들을 8byte에 맞춰 정렬하기 위해 추가
align 8, db 0

	; GDTR의 끝을 8byte로 정렬하기 위해 추가
	dw 0x0000
; GDTR 자료구조 정의
GDTR:
    dw GDTEND - GDT - 1         ; 아래에 위치하는 GDT 테이블의 전체 크기
    dd ( GDT )   ; 아래에 위치하는 GDT 테이블의 시작 어드레스

; GDT 테이블 정의
GDT:
    ; 널(NULL) 디스크립터, 반드시 0으로 초기화해야 함
    NULLDescriptor:
        dw 0x0000
        dw 0x0000
        db 0x00
        db 0x00
        db 0x00
        db 0x00

    ; IA-32e 모드 커널용 코드 세그먼트 디스크립터
	IA_32eCODEDESCRIPTOR:     
	    dw 0xFFFF       ; Limit [15:0]
	    dw 0x0000       ; Base [15:0]
	    db 0x00         ; Base [23:16]
	    db 0x9A         ; P=1, DPL=0, Code Segment, Execute/Read
	    db 0xAF         ; G=1, D=0, L=1, Limit[19:16]
	    db 0x00         ; Base [31:24]  
	    
    ; IA-32e 모드 커널용 데이터 세그먼트 디스크립터
	IA_32eDATADESCRIPTOR:
	    dw 0xFFFF       ; Limit [15:0]
	    dw 0x0000       ; Base [15:0]
	    db 0x00         ; Base [23:16]
	    db 0x92         ; P=1, DPL=0, Data Segment, Read/Write
	    db 0xAF         ; G=1, D=0, L=1, Limit[19:16]
	    db 0x00         ; Base [31:24]

GDTEND:
