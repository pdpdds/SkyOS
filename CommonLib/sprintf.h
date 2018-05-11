#pragma once

#ifdef  __cplusplus
extern "C" {
#endif
int sprintf(char *s, const char *format, ...);
void itoa(unsigned i, unsigned base, char* buf);
//void itoa_s(int i, unsigned base, char* buf);
void itoa_s(unsigned int i, unsigned base, char* buf);
int strcmp(const char *string1, const char *string2);
char* _i64toa(long long value, char *str, int radix);
#ifdef  __cplusplus
}
#endif