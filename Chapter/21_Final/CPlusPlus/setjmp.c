//
// setjmp.c
//
// Non-local goto
//
// Copyright (C) 2002 Michael Ringgaard. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification, are permitted provided that the following conditions
// are met:
// 
// 1. Redistributions of source code must retain the above copyright 
//    notice, this list of conditions and the following disclaimer.  
// 2. Redistributions in binary form must reproduce the above copyright
//    notice, this list of conditions and the following disclaimer in the
//    documentation and/or other materials provided with the distribution.  
// 3. Neither the name of the project nor the names of its contributors
//    may be used to endorse or promote products derived from this software
//    without specific prior written permission. 
// 
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
// ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
// IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
// ARE DISCLAIMED.  IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE
// FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
// DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
// OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
// HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
// LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
// OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF 
// SUCH DAMAGE.
// 

#include <signal.h>
#include <setjmp.h>

#define OFS_EBP   0
#define OFS_EBX   4
#define OFS_EDI   8
#define OFS_ESI   12
#define OFS_ESP   16
#define OFS_EIP   20

__declspec(naked) int setjmp(jmp_buf env) {
  __asm {
    mov edx, 4[esp]          // jmp_buf 버퍼 포인터를 얻는다.
    mov eax, [esp]           // 함수 복귀주소를 저장한다.
    mov OFS_EIP[edx], eax
    mov OFS_EBP[edx], ebp    // EBP, EBX, EDI, ESI, ESP 레지스터를 버퍼에 저장한다.
    mov OFS_EBX[edx], ebx
    mov OFS_EDI[edx], edi
    mov OFS_ESI[edx], esi
    mov OFS_ESP[edx], esp
    xor eax, eax             
    ret
  }
}

__declspec(naked) void longjmp(jmp_buf env, int value) {
  __asm {
    mov edx, 4[esp]          // jmp_buf 버퍼 포인터를 얻는다.
    mov eax, 8[esp]          // value값을 eax에 저장한다.

    mov esp, OFS_ESP[edx]    // ESP를 setjmp에서 저장했던 ESP값으로 변경한다.
    mov ebx, OFS_EIP[edx]    // 새 복귀주소값을 얻어서 ESP에 저장한다.
    mov [esp], ebx
    
    mov ebp, OFS_EBP[edx]    //EBP, EBX, EDI, ESI 레지스터 값을 복원한다.
    mov ebx, OFS_EBX[edx]
    mov edi, OFS_EDI[edx]
    mov esi, OFS_ESI[edx]

    ret
  }
}
/*
int sigsetjmp(sigjmp_buf env, int savesigs) {
  if (savesigs) {
    sigprocmask(SIG_BLOCK, NULL, &env->sigmask);
  } else {
    env->sigmask = -1;
  }
  return setjmp(env->env);
}

void siglongjmp(sigjmp_buf env, int value) {
  if (env->sigmask != -1) {
    sigprocmask(SIG_SETMASK, &env->sigmask, NULL);
  }
  longjmp(env->env, value);
}
*/