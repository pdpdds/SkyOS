#include "errno.h"
#include "FileSysAdaptor.h"
#include "StorageManager.h"

FILE *fopen(const char *filename, const char *mode)
{
	return StorageManager::GetInstance()->OpenFile(filename, mode);
}

size_t fread(void *ptr, size_t size, size_t count, FILE *stream)
{
	return StorageManager::GetInstance()->ReadFile(stream, (unsigned char*)ptr, size, count);
}

size_t fwrite(const void *ptr, size_t size, size_t count, FILE *stream)
{
	return StorageManager::GetInstance()->WriteFile(stream, (unsigned char*)ptr, size, count);
}

int fclose(FILE *stream)
{
	return StorageManager::GetInstance()->CloseFile(stream);
}

int feof(FILE *stream)
{
	return 0;
}

int fseek(FILE *stream, long int offset, int whence)
{
	return 0;
}

long int ftell(FILE *stream)
{
	return 0;
}

int getc(FILE * stream)
{
	return 0;
}

int ferror(FILE *stream)
{
	return 0;
}

int fflush(FILE *stream)
{
	return 0;
}

FILE *freopen(const char *filename, const char *mode, FILE *stream)
{
	return 0;
}

char *	strerror(int errnum)
{
	return 0;
}

int fprintf(FILE * stream, const char * format, ...)
{
	return 0;
}