#include <string.h>

//! warning C4706: assignment within conditional expression
#pragma warning (disable:4706)

//! compare two strings
int strcmp (const char* str1, const char* str2) {

	int res=0;
	while (!(res = *(unsigned char*)str1 - *(unsigned char*)str2) && *str2)
		++str1, ++str2;

	if (res < 0)
		res = -1;
	if (res > 0)
		res = 1;

	return res;
}

int strncmp(const char *s1, const char *s2, size_t n)
{
	for (; n > 0; s1++, s2++, --n)
		if (*s1 != *s2)
			return ((*(unsigned char *)s1 < *(unsigned char *)s2) ? -1 : +1);
		else if (*s1 == '\0')
			return 0;
	return 0;
}

char *strstr(const char *in, const char *str)
{
	char c;
	size_t len;

	c = *str++;
	if (!c)
		return (char *)in;	// Trivial empty string case

	len = strlen(str);
	do {
		char sc;

		do {
			sc = *in++;
			if (!sc)
				return (char *)0;
		} while (sc != c);
	} while (strncmp(in, str, len) != 0);

	return (char *)(in - 1);
}

//! copies string s2 to s1
char *strcpy(char *s1, const char *s2)
{
    char *s1_p = s1;
    while (*s1++ = *s2++);
    return s1_p;
}

//! returns length of string
size_t strlen ( const char* str ) {

	size_t	len=0;
	while (str[len++]);
	return len;
}

char *strncpy(char *string1, const char *string2, size_t count)
{
	char *p = string1;
	while (count)
	{
		*p = *string2;
		if (*string2)
		{
			string2++;
		}
		p++;
		count--;
	}
	return(string1);
}

//! copies count bytes from src to dest
void *memcpy(void *dest, const void *src, size_t count)
{
    const char *sp = (const char *)src;
    char *dp = (char *)dest;
    for(; count != 0; count--) *dp++ = *sp++;
    return dest;
}

//! sets count bytes of dest to val
void* memset(void *dest, char val, size_t count)
{
    unsigned char *temp = (unsigned char *)dest;
	for( ; count != 0; count--, temp[count] = val);
	return dest;
}

//! sets count bytes of dest to val
unsigned short *memsetw(unsigned short *dest, unsigned short val, size_t count)
{
    unsigned short *temp = (unsigned short *)dest;
    for( ; count != 0; count--)
		*temp++ = val;
    return dest;
}

//! locates first occurance of character in string
char* strchr (char * str, int character ) {

	do {
		if ( *str == character )
			return (char*)str;
	}
	while (*str++);

	return 0;
}

#define TOUPPER(CH) \
  (((CH) >= 'a' && (CH) <= 'z') ? ((CH) - 'a' + 'A') : (CH))

int stricmp(const char *s1, const char *s2)
{
	while (*s2 != 0 && TOUPPER(*s1) == TOUPPER(*s2))
		s1++, s2++;
	return (int)(TOUPPER(*s1) - TOUPPER(*s2));
}

char *strpbrk(const char *strSrc, const char *str)
{	
	const char *s;
	while (*strSrc != '\0')
	{
		s = str;
		while (*s != '\0')
		{
			if (*strSrc == *s)
				return (char *)strSrc;
			++s;
		}
		++strSrc;
	}
	return 0;
}

char * strtok(char *s1, const char *delimit)
{
	static char *lastToken = 0; /* UNSAFE SHARED STATE! */
	char *tmp;

	/* Skip leading delimiters if new string. */
	if (s1 == 0) {
		s1 = lastToken;
		if (s1 == 0)         /* End of story? */
			return 0;
	}
	else {
		s1 += strspn(s1, delimit);
	}

	/* Find end of segment */
	tmp = strpbrk(s1, delimit);
	if (tmp) {
		/* Found another delimiter, split string and save state. */
		*tmp = '\0';
		lastToken = tmp + 1;
	}
	else {
		/* Last segment, remember that. */
		lastToken = 0;
	}

	return s1;
}

int strspn(const char *strSrc, const char *str)
{	
	const char *s;
	const char *t = strSrc;
	while (*t != '\0')
	{
		s = str;
		while (*s != '\0')
		{
			if (*t == *s)
				break;
			++s;
		}
		if (*s == '\0')
			return t - strSrc;
		++t;
	}
	return 0;
}