//Standard C String routines
#pragma once
#include <size_t.h>

char *strcpy(char *s1, const char *s2);
size_t strlen ( const char* str );
int strcmp (const char* str1, const char* str2);
int strncmp(const char *s1, const char *s2, size_t n);
int strncasecmp(const char *s1, const char *s2, size_t n);
char* strstr(const char *in, const char *str);
char *strupr(char *str);
char *strcat(char *dest, const char *src);
char * strncat(char * destination, const char * source, size_t num);

void* memcpy(void *dest, const void *src, size_t count);
void* memset(void *dest, char val, size_t count);
unsigned short* memsetw(unsigned short *dest, unsigned short val, size_t count);
int memcmp(const void *s1, const void *s2, size_t n);

char* strchr (const char * str, int character );
int stricmp(const char *s1, const char *s2);

char *strpbrk(const char *strSrc, const char *str);
char * strtok(char *s1, const char *delimit);
int strspn(const char *strSrc, const char *str);

//char *strncpy(char * Dest, const char * Source, unsigned short Length);
char *strncpy(char *string1, const char *string2, size_t count);
int strnicmp(const char * String1, const char * String2, unsigned int Len);
const char *strrchr(const char * String, char const Character);

void* memmove(void *dest, void *src, size_t n);
void* memchr(const void *s, int c, size_t n);