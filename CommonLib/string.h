//Standard C String routines
#pragma once
#include <size_t.h>

char *strcpy(char *s1, const char *s2);
size_t strlen ( const char* str );
int strcmp (const char* str1, const char* str2);
int strncmp(const char *s1, const char *s2, size_t n);
char* strstr(const char *in, const char *str);

void* memcpy(void *dest, const void *src, size_t count);
void* memset(void *dest, char val, size_t count);
unsigned short* memsetw(unsigned short *dest, unsigned short val, size_t count);

char* strchr (char * str, int character );
int stricmp(const char *s1, const char *s2);

char *strpbrk(const char *strSrc, const char *str);
char * strtok(char *s1, const char *delimit);
int strspn(const char *strSrc, const char *str);