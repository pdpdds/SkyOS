#pragma once

#ifdef  __cplusplus
extern "C" {
#endif

	/*----[ Data type definitions ]-----------------------------------------*/
	typedef int     jmp_buf[6];
	/*----[ Function prototypes ]-------------------------------------------*/
	extern int  setjmp(jmp_buf env);

	extern void longjmp(jmp_buf env, int val);


#ifdef  __cplusplus
}
#endif