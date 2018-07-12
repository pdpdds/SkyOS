#pragma once
#include "SkyMockInterface.h"

#ifdef  __cplusplus
extern "C" {
#endif
	size_t fread(void *ptr, size_t size, size_t nmemb, FILE *stream);
	FILE *fopen(const char *filename, const char *mode);
	size_t fwrite(const void *ptr, size_t size, size_t nmemb, FILE *stream);
	int fclose(FILE *stream);
	int feof(FILE *stream);
	int ferror(FILE *stream);
	int fflush(FILE *stream);
	FILE *freopen(const char *filename, const char *mode, FILE *stream);
	int fseek(FILE *stream, long int offset, int whence);
	long int ftell(FILE *stream);
	int fgetc(FILE * stream);
	char* fgets(char *dst, int max, FILE *fp);
	int fscanf(FILE *fp, const char *fmt, ...);
	char*	strerr(int errnum);
	//extern FILE*	stderr;
	int fprintf(FILE * stream, const char * format, ...);
	char *	strerror(int errnum);

#ifdef  __cplusplus
}
#endif

#ifdef SKY_DLL
extern SkyMockInterface g_mockInterface;
extern SKY_PROCESS_INTERFACE g_processInterface;
#define kmalloc(a) g_mockInterface.g_allocInterface.sky_kmalloc(a)
#define kfree(p) g_mockInterface.g_allocInterface.sky_kfree(p)
#define kcalloc(a, b) g_mockInterface.g_allocInterface.sky_kcalloc(a, b)
#define krealloc(a, b) g_mockInterface.g_allocInterface.sky_krealloc(a, b)
#define ksleep(a) g_processInterface.sky_ksleep(a)
#endif

extern "C" void* malloc(size_t size);
extern "C" void free(void* p);
extern "C" unsigned int calloc(unsigned int count, unsigned int size);
extern "C" void* realloc(void* ptr, size_t size);
