#pragma once
#include "windef.h"
#include "stdint.h"

#if !defined(__cplusplus) || defined(_MSC_VER)
typedef unsigned short wchar_t;
#endif

#ifdef __cplusplus
extern "C"
{
#endif

	typedef struct
	{
		wchar_t __cp;
		uint16_t __class;
		wchar_t __uc;
		wchar_t __lc;
	} __wchar_info_t;

	wchar_t *wcscat(wchar_t* dest, const wchar_t* src);
	wchar_t* wcschr(const wchar_t* str, wchar_t c);
	int wcscmp(const wchar_t *str1, const wchar_t *str2);
	wchar_t *wcscpy(wchar_t *strDestination, const wchar_t *strSource);
	size_t wcslen(const wchar_t *str);
	int wcsncmp(const wchar_t *s1, const wchar_t *s2, size_t n);
	wchar_t* wcsncpy(wchar_t* Dst, const wchar_t* Src, unsigned int Count);
	wchar_t* wcspbrk(const wchar_t *s1, const wchar_t *s2);
	size_t wcstombs(char *mbstr, const wchar_t *wcstr, size_t count);	
	int _wcsnicmp(const wchar_t *s1, const wchar_t *s2, size_t n);		

	int _wcwidth(wchar_t ucs);
	int _wcswidth(const wchar_t *pwcs, size_t n);
	int _wcswidth_cjk(const wchar_t *pwcs, size_t n);

#ifdef __cplusplus
}
#endif