// setjmp.cpp : 콘솔 응용 프로그램에 대한 진입점을 정의합니다.
//

#include "stdafx.h"
#include <setjmp.h>

jmp_buf pos;

void Func2(int x)
{
	if (x < 5)
		longjmp(pos, x);

	return;
}

void Func1()
{
	static int count = 0;
	count++;

	Func2(count);

	return;
}

int main()
{
	int result;

	result = setjmp(pos);
	printf("%d\n", result);
	Func1();

	printf("End!!\n");

	return 0;
}
