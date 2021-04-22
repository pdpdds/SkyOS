#pragma once
#ifdef WIN32
#include <Windows.h>
#include <stdio.h>
#else
#include "windef.h"
#include "fileio.h"

#endif

#pragma pack (push, 1)
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

//프로세스 생성 및 삭제 인터페이스
//불행중 다행으로 LPTHREAD_START_ROUTINE 함수포인터는 WIN32나 SKYOS나 똑같다...
typedef struct tag_SKY_PROCESS_INTERFACE
{
	unsigned int (*sky_kcreate_process_from_memory)(const char* appName, LPTHREAD_START_ROUTINE lpStartAddress, void* param, UINT32 processType);
	unsigned int (*sky_kcreate_process_from_file)(char* appName, void* param, UINT32 processType);
	unsigned int (*sky_kcreate_thread_from_memory)(unsigned int processId, LPTHREAD_START_ROUTINE lpStartAddress, LPVOID param);	
	unsigned int (*sky_kcreate_thread_from_file)(unsigned int processId, FILE* pFile, LPVOID param);
	unsigned int (*sky_kdestroy_task)(unsigned int taskId); //task가 하나인 프로세스는 프로세스도 삭제, taskId가 메인 스레드면 역시 프로세스도 삭제
	void (*sky_ksleep)(int ms);
} SKY_PROCESS_INTERFACE;

//메모리 할당관련 인터페이스
typedef struct SKY_ALLOC_Interface
{
	unsigned int (*sky_kmalloc)(unsigned int sz);
	void (*sky_kfree)(void* p);	
	unsigned int (*sky_kcalloc)(unsigned int count, unsigned int size);
	void* (*sky_krealloc)(void* ptr, size_t size);

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

#pragma pack (pop, 1)


typedef void(*PSetSkyMockInterface)(SKY_ALLOC_Interface, SKY_FILE_Interface, SKY_Print_Interface);
typedef void(*PSetSkyProcessInterface)(SKY_PROCESS_INTERFACE);