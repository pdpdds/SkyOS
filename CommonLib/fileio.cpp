#include "fileio.h"
#include "errno.h"


size_t fread(void *ptr, size_t size, size_t nmemb, FILE *stream)
{
	return 0;
}

FILE *fopen(const char *filename, const char *mode)
{
	return 0;
}

int fclose(FILE *stream)
{
	return 0;
}

int feof(FILE *stream)
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

int fseek(FILE *stream, long int offset, int whence)
{
	return 0;
}

long int ftell(FILE *stream)
{
	return 0;
}

size_t fwrite(const void *ptr, size_t size, size_t nmemb, FILE *stream)
{
	return 0;
}

int getc(FILE * stream)
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

/*char *	strerror(int errnum)
{
	char *ret = __strerror_r(errnum, 0, 0);
	int saved_errno;

	if (__glibc_likely(ret != 0))
		return ret;
	saved_errno = errno;
	if (buf == 0)
		buf = malloc(1024);
	__set_errno(saved_errno);
	if (buf == 0)
		return _("Unknown error");
	return __strerror_r(errnum, buf, 1024);
}*/