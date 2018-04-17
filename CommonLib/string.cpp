#include "string.h"
#include "CharType.h"
#include "sprintf.h"

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

int strncasecmp(const char *s1, const char *s2, size_t n)
{
	if (n == 0)
		return 0;

	while (n-- != 0 && ToLower(*s1) == ToLower(*s2))
	{
		if (n == 0 || *s1 == '\0' || *s2 == '\0')
			break;
		s1++;
		s2++;
	}

	return ToLower(*(unsigned char *)s1) - ToLower(*(unsigned char *)s2);
}

char *strcat(char *dest, const char *src)

{

	size_t i, j;

	for (i = 0; dest[i] != '\0'; i++)

		;

	for (j = 0; src[j] != '\0'; j++)

		dest[i + j] = src[j];

	dest[i + j] = '\0';

	return dest;

}

char * strncat(char * destination, const char * source, size_t num) {
	size_t s_size = strlen(source);
	return strncpy((char *)destination + (strlen(destination) + 1), source, (num > s_size) ? s_size : num);
}


char *strupr(char *str)
{
	char *cp = str;

	while (*cp != '\0') {
		if (*cp >= 'a' && *cp <= 'z')
			*cp = *cp - 'a' + 'A';
		cp++;
	}

	return str;
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
	while (str[len])
	{
		len++;
	}
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




//! sets count bytes of dest to val
unsigned short *memsetw(unsigned short *dest, unsigned short val, size_t count)
{
    unsigned short *temp = (unsigned short *)dest;
    for( ; count != 0; count--)
		*temp++ = val;
    return dest;
}

//! locates first occurance of character in string
char* strchr (const char * str, int character ) {

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
/*
char *strncpy(char * Dest, const char * Source, unsigned short Length)
{
	unsigned int Len = strlen(Source), i;
	for (i = 0; i<Len && i<Length; i++)
		Dest[i] = Source[i];
	return Dest;
}*/

int strnicmp(const char * String1, const char * String2, unsigned int Len)
{
	unsigned int i;
	for (i = 0; i<Len; i++)
		if (ToUpper(String1[i]) != ToUpper(String2[i]))
			return String1[i] - String2[i];
	return 0;
}

const char *strrchr(const char * String, char const Character)
{
	UINT32 Len = strlen(String), i;
	for (i = Len; i>0; i--)
		if (String[i] == Character)
			return &String[i];
	return 0;
}

int normalize(double *val) {
	int exponent = 0;
	double value = *val;

	while (value >= 1.0) {
		value /= 10.0;
		++exponent;
	}

	while (value < 0.1) {
		value *= 10.0;
		--exponent;
	}
	*val = value;
	return exponent;
}

void ftoa_fixed(char *buffer, double value) {
	/* carry out a fixed conversion of a double value to a string, with a precision of 5 decimal digits.
	* Values with absolute values less than 0.000001 are rounded to 0.0
	* Note: this blindly assumes that the buffer will be large enough to hold the largest possible result.
	* The largest value we expect is an IEEE 754 double precision real, with maximum magnitude of approximately
	* e+308. The C standard requires an implementation to allow a single conversion to produce up to 512
	* characters, so that's what we really expect as the buffer size.
	*/

	int exponent = 0;
	int places = 0;
	static const int width = 4;

	if (value == 0.0) {
		buffer[0] = '0';
		buffer[1] = '\0';
		return;
	}

	if (value < 0.0) {
		*buffer++ = '-';
		value = -value;
	}

	exponent = normalize(&value);

	while (exponent > 0) {
		int digit = value * 10;
		*buffer++ = digit + '0';
		value = value * 10 - digit;
		++places;
		--exponent;
	}

	if (places == 0)
		*buffer++ = '0';

	*buffer++ = '.';

	while (exponent < 0 && places < width) {
		*buffer++ = '0';
		--exponent;
		++places;
	}

	while (places < width) {
		int digit = value * 10.0;
		*buffer++ = digit + '0';
		value = value * 10.0 - digit;
		++places;
	}
	*buffer = '\0';
}

void ftoa_sci(char *buffer, double value) {
	int exponent = 0;
	static const int width = 4;

	if (value == 0.0) {
		buffer[0] = '0';
		buffer[1] = '\0';
		return;
	}

	if (value < 0.0) {
		*buffer++ = '-';
		value = -value;
	}

	exponent = normalize(&value);

	int digit = value * 10.0;
	*buffer++ = digit + '0';
	value = value * 10.0 - digit;
	--exponent;

	*buffer++ = '.';

	for (int i = 0; i < width; i++) {
		digit = value * 10.0;
		*buffer++ = digit + '0';
		value = value * 10.0 - digit;
	}

	*buffer++ = 'e';
	itoa(exponent, 10, buffer);
}