;-----------------------------------------------------------------------------
; ftol.asm - floating point to integer conversion
;-----------------------------------------------------------------------------

                SECTION .text

                global  _ftol
                global  __ftol
                global  __ftol2
                global  __ftol2_sse
                
_ftol:
__ftol:
                fnstcw  word [esp-2]
                mov     ax, word [esp-2]
                or      ax, 0C00h
                mov     word [esp-4], ax
                fldcw   word [esp-4]
                fistp   qword [esp-12]
                fldcw   word [esp-2]
                mov     eax, dword [esp-12]
                mov     edx, dword [esp-8]
                ret

__ftol2:
                fnstcw  word [esp-2]
                mov     ax, word [esp-2]
                or      ax, 0C00h
                mov     word [esp-4], ax
                fldcw   word [esp-4]
                fistp   qword [esp-12]
                fldcw   word [esp-2]
                mov     eax, dword [esp-12]
                mov     edx, dword [esp-8]
                ret

; TODO: what is the correct code for __ftol2_sse
__ftol2_sse:
                fnstcw  word [esp-2]
                mov     ax, word [esp-2]
                or      ax, 0C00h
                mov     word [esp-4], ax
                fldcw   word [esp-4]
                fistp   qword [esp-12]
                fldcw   word [esp-2]
                mov     eax, dword [esp-12]
                mov     edx, dword [esp-8]
                ret