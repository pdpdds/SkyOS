#include "sprintf.h"
#include <stdarg.h>
#include <va_list.h>
#include <stdio.h>

size_t sprintf(char *s, const char *format, ...)
{
	va_list arg;
	size_t ret;

	va_start(arg, format);
	ret = vsprintf(s, format, arg);
	va_end(arg);

	return ret;
}


char tbuf[32];
char bchars[] = { '0','1','2','3','4','5','6','7','8','9','A','B','C','D','E','F' };

void itoa(unsigned i, unsigned base, char* buf) {
	int pos = 0;
	int opos = 0;
	int top = 0;

//진수가 16을 넘거나 제공된 값이 0이면 문자 '0'을 buf에 담는다.
	if (i == 0 || base > 16) {
		buf[0] = '0';
		buf[1] = '\0';
		return;
	}

//진수에 맞게 문자를 얻어낸다.
//17이 입력되고 이를 16진수로 변환하면 11이다.
//아래 루프가 그 기능을 담당한다.
	while (i != 0) {
		tbuf[pos] = bchars[i % base];
		pos++;
		i /= base;
	}
	top = pos--;
	for (opos = 0; opos < top; pos--, opos++) {
		buf[opos] = tbuf[pos];
	}
	buf[opos] = 0;
}

void itoa_s(int i, unsigned base, char* buf) {
	if (base > 16) return;
	if (i < 0) {
		*buf++ = '-';
		i *= -1;
	}
	itoa(i, base, buf);
}

void itoa_s(unsigned int i, unsigned base, char* buf) {
	if (base > 16) return;

	itoa(i, base, buf);
}