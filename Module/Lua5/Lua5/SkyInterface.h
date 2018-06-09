#pragma once
#include "windef.h"
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
#define kmalloc(a) g_mockInterface.g_allocInterface.sky_kmalloc(a)
#define kfree(p) g_mockInterface.g_allocInterface.sky_kfree(p)
#endif
