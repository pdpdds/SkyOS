#include "SkyMockInterface.h"


size_t sky_fread(void *ptr, size_t size, size_t nmemb, FILE *stream)
{
	return fread(ptr, size, nmemb, stream);
}

FILE* sky_fopen(const char *filename, const char *mode)
{
	return fopen(filename, mode);
}

size_t sky_fwrite(const void *ptr, size_t size, size_t nmemb, FILE *stream)
{
	if (stream == nullptr)
		return 0;

	return fwrite(ptr, size, nmemb, stream);
}
int sky_fclose(FILE *stream)
{
	return fclose(stream);
}

int sky_feof(FILE *stream)
{
	return feof(stream);
}

int sky_ferror(FILE *stream)
{
	return ferror(stream);
}

int sky_fflush(FILE *stream)
{
	return fflush(stream);
}

FILE* sky_freopen(const char *filename, const char *mode, FILE *stream)
{
	return freopen(filename, mode, stream);
}

int sky_fseek(FILE *stream, long int offset, int whence)
{
	return fseek(stream, offset, whence);
}

long int sky_ftell(FILE *stream)
{
	return ftell(stream);
}

int sky_fgetc(FILE * stream)
{
	return fgetc(stream);
}

char* sky_fgets(char *dst, int max, FILE *fp)
{
	return fgets(dst, max, fp);
}

SKY_FILE_Interface g_FileInterface =
{
	sky_fread,
	sky_fopen,
	sky_fwrite,
	sky_fclose,
	sky_feof,
	sky_ferror,
	sky_fflush,
	sky_freopen,
	sky_fseek,
	sky_ftell,
	sky_fgetc,
	sky_fgets,
};


//메모리 할당
unsigned int sky_kmalloc(unsigned int sz)
{
	if (sz == 0)
		return 0;

	return (unsigned int)new char[sz];
}

void sky_kfree(void* p)
{
	delete p;
}

SKY_ALLOC_Interface g_allocInterface =
{
	sky_kmalloc,
	sky_kfree,
};

void sky_printf(const char* str, ...)
{
	char *p = new char[128];
	va_list ap;
	
	va_start(ap, str);
	(void)vsnprintf(p, 128, str, ap);
	va_end(ap);
	
	printf(p);
	delete p;
}

SKY_Print_Interface g_printInterface =
{
	sky_printf,	
	stdin, 
	stdout, 
	stderr,
};

