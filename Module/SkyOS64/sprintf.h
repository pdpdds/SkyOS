#pragma once

#ifdef  __cplusplus
extern "C" {
#endif
size_t sprintf(char *s, const char *format, ...);
void itoa(unsigned i, unsigned base, char* buf);
//void itoa_s(int i, unsigned base, char* buf);
void itoa_s(unsigned int i, unsigned base, char* buf);
int strcmp(const char *string1, const char *string2);
#ifdef  __cplusplus
}
#endif