#pragma once
#ifdef WIN32
#include <stdio.h>
#else
#include "windef.h"
#include "fileio.h"
#endif

//입출력 관련 인터페이스
typedef struct SKY_FILE_Interface
{
	size_t (*sky_fread)(void *ptr, size_t size, size_t nmemb, FILE *stream);
	FILE* (*sky_fopen)(const char *filename, const char *mode);
	size_t (*sky_fwrite)(const void *ptr, size_t size, size_t nmemb, FILE *stream);
	int (*sky_fclose)(FILE *stream);
	int (*sky_feof)(FILE *stream);
	int (*sky_ferror)(FILE *stream);
	int (*sky_fflush)(FILE *stream);
	FILE* (*sky_freopen)(const char *filename, const char *mode, FILE *stream);
	int (*sky_fseek)(FILE *stream, long int offset, int whence);
	long int (*sky_ftell)(FILE *stream);
	int (*sky_fgetc)(FILE * stream);
	char* (*sky_fgets)(char *dst, int max, FILE *fp);	
} SKY_FILE_Interface;

//메모리 할당관련 인터페이스
typedef struct SKY_ALLOC_Interface
{
	unsigned int (*sky_kmalloc)(unsigned int sz);
	void (*sky_kfree)(void* p);
} SKY_ALLOC_Interface;

//출력관련 인터페이스
typedef struct SKY_Print_Interface
{	
	void(*sky_printf)(const char* str, ...);
	FILE* sky_stdin;
	FILE* sky_stdout;
	FILE* sky_stderr;
} SKY_Print_Interface;

//DLL로 넘길 인터페이스 클래스
typedef struct SkyMockInterface
{
	SKY_ALLOC_Interface g_allocInterface;
	SKY_FILE_Interface g_fileInterface;
	SKY_Print_Interface g_printInterface;
}SkyMockInterface;

typedef void(*PSetSkyMockInterface)(SKY_ALLOC_Interface, SKY_FILE_Interface, SKY_Print_Interface);