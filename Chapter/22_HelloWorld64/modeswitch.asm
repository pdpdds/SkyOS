[BITS 32]
SECTION .data
global _ModeSwitchAndJumpKernel64
_ModeSwitchAndJumpKernel64:

	;IA-32e 세그먼트 설렉트 후 2MB 영역으로 점프
	jmp 0x08:0x180400
	pop ebp 
	;Not Entry
	jmp $

	

	

