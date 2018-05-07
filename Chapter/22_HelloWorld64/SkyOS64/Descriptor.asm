[BITS 64]
                     
global LoadGDTR, LoadTR, LoadIDTR
                        
SECTION .text

LoadGDTR:
    lgdt[rdi]
    ret

LoadTR:
    ltr di
    ret

LoadIDTR:
    lidt[rdi]
    ret