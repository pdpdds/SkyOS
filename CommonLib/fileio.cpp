#include "fileio.h"
#include "errno.h"

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