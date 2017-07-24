#pragma once

int sprintf(char *s, const char *format, ...);
void itoa(unsigned i, unsigned base, char* buf);
void itoa_s(int i, unsigned base, char* buf);
void itoa_s(unsigned int i, unsigned base, char* buf);
char * __cdecl strncpy(char *string1, const char *string2, size_t count);
int strcmp(const char *string1, const char *string2);
size_t strlen(const char *string);