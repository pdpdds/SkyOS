#include "string.h"
#include "CharType.h"
#include "sprintf.h"
#include "memory.h"

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

int strcoll(const char *s1, const char *s2)
{
	return strcmp(s1, s2);
}

size_t strxfrm(char *dst, char *src, size_t n)
{
	size_t r = 0;
	int c;

	if (n != 0)
	{
		while ((c = *src++) != 0)
		{
			r++;

			if (--n == 0)
			{
				while (*src++ != 0)
					r++;

				break;
			}

			*dst++ = c;
		}

		*dst = 0;
	}

	return r;
}
/*
char *strncpy(char * Dest, const char * Source, unsigned short Length)
{
	unsigned int Len = strlen(Source), i;
	for (i = 0; i<Len && i<Length; i++)
		Dest[i] = Source[i];
	return Dest;
}*/

char *strnchr(const char *str, char c, size_t count)
{
	char *ptr = NULL;

	while (*str && count > 0)
	{
		if (*str == c)
		{
			ptr = (char*)str;
			break;
		}

		str++;

		count--;
	}

	return ptr;
}

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

static int maxExponent = 511;	/* Largest possible base 10 exponent.  Any
								* exponent larger than this will already
								* produce underflow or overflow, so there's
								* no need to worry about additional digits.
								*/
static double powersOf10[] = {	/* Table giving binary powers of 10.  Entry */
	10.,			/* is 10^2^i.  Used to convert decimal */
	100.,			/* exponents into floating-point numbers. */
	1.0e4,
	1.0e8,
	1.0e16,
	1.0e32,
	1.0e64,
	1.0e128,
	1.0e256
};

double strtod(const char *string, char **endPtr)
		/* A decimal ASCII floating-point number,
						* optionally preceded by white space.
						* Must have form "-I.FE-X", where I is the
						* integer part of the mantissa, F is the
						* fractional part of the mantissa, and X
						* is the exponent.  Either of the signs
						* may be "+", "-", or omitted.  Either I
						* or F may be omitted, or both.  The decimal
						* point isn't necessary unless F is present.
						* The "E" may actually be an "e".  E and X
						* may both be omitted (but not just one).
						*/
		/* If non-NULL, store terminating character's
					* address here. */
{
	int sign, expSign = FALSE;
	double fraction, dblExp, *d;
	register CONST char *p;
	register int c;
	int exp = 0;		/* Exponent read from "EX" field. */
	int fracExp = 0;		/* Exponent that derives from the fractional
							* part.  Under normal circumstatnces, it is
							* the negative of the number of digits in F.
							* However, if I is very long, the last digits
							* of I get dropped (otherwise a long I with a
							* large negative exponent could cause an
							* unnecessary overflow on I alone).  In this
							* case, fracExp is incremented one for each
							* dropped digit. */
	int mantSize;		/* Number of digits in mantissa. */
	int decPt;			/* Number of mantissa digits BEFORE decimal
						* point. */
	CONST char *pExp;		/* Temporarily holds location of exponent
							* in string. */

							/*
							* Strip off leading blanks and check for a sign.
							*/

	p = string;
	while (IsSpace(UCHAR(*p))) {
		p += 1;
	}
	if (*p == '-') {
		sign = TRUE;
		p += 1;
	}
	else {
		if (*p == '+') {
			p += 1;
		}
		sign = FALSE;
	}

	/*
	* Count the number of digits in the mantissa (including the decimal
	* point), and also locate the decimal point.
	*/

	decPt = -1;
	for (mantSize = 0; ; mantSize += 1)
	{
		c = *p;
		if (!IsDigit(c)) {
			if ((c != '.') || (decPt >= 0)) {
				break;
			}
			decPt = mantSize;
		}
		p += 1;
	}

	/*
	* Now suck up the digits in the mantissa.  Use two integers to
	* collect 9 digits each (this is faster than using floating-point).
	* If the mantissa has more than 18 digits, ignore the extras, since
	* they can't affect the value anyway.
	*/

	pExp = p;
	p -= mantSize;
	if (decPt < 0) {
		decPt = mantSize;
	}
	else {
		mantSize -= 1;			/* One of the digits was the point. */
	}
	if (mantSize > 18) {
		fracExp = decPt - 18;
		mantSize = 18;
	}
	else {
		fracExp = decPt - mantSize;
	}
	if (mantSize == 0) {
		fraction = 0.0;
		p = string;
		goto done;
	}
	else {
		int frac1, frac2;
		frac1 = 0;
		for (; mantSize > 9; mantSize -= 1)
		{
			c = *p;
			p += 1;
			if (c == '.') {
				c = *p;
				p += 1;
			}
			frac1 = 10 * frac1 + (c - '0');
		}
		frac2 = 0;
		for (; mantSize > 0; mantSize -= 1)
		{
			c = *p;
			p += 1;
			if (c == '.') {
				c = *p;
				p += 1;
			}
			frac2 = 10 * frac2 + (c - '0');
		}
		fraction = (1.0e9 * frac1) + frac2;
	}

	/*
	* Skim off the exponent.
	*/

	p = pExp;
	if ((*p == 'E') || (*p == 'e')) {
		p += 1;
		if (*p == '-') {
			expSign = TRUE;
			p += 1;
		}
		else {
			if (*p == '+') {
				p += 1;
			}
			expSign = FALSE;
		}
		if (!IsDigit(UCHAR(*p))) {
			p = pExp;
			goto done;
		}
		while (IsSpace(UCHAR(*p))) {
			exp = exp * 10 + (*p - '0');
			p += 1;
		}
	}
	if (expSign) {
		exp = fracExp - exp;
	}
	else {
		exp = fracExp + exp;
	}

	/*
	* Generate a floating-point number that represents the exponent.
	* Do this by processing the exponent one bit at a time to combine
	* many powers of 2 of 10. Then combine the exponent with the
	* fraction.
	*/

	if (exp < 0) {
		expSign = TRUE;
		exp = -exp;
	}
	else {
		expSign = FALSE;
	}
	if (exp > maxExponent) {
		exp = maxExponent;
		//check
		//errno = ERANGE;
	}
	dblExp = 1.0;
	for (d = powersOf10; exp != 0; exp >>= 1, d += 1) {
		if (exp & 01) {
			dblExp *= *d;
		}
	}
	if (expSign) {
		fraction /= dblExp;
	}
	else {
		fraction *= dblExp;
	}

done:
	if (endPtr != NULL) {
		*endPtr = (char *)p;
	}

	if (sign) {
		return -fraction;
	}
	return fraction;
}

size_t
strcspn(const char* s1, const char *s2)
{
	register const char *p, *spanp;
	register char c, sc;

	/*
	* Stop as soon as we find any character from s2.  Note that there
	* must be a NUL in s2; it suffices to stop when we find that, too.
	*/
	for (p = s1;;) {
		c = *p++;
		spanp = s2;
		do {
			if ((sc = *spanp++) == c)
				return (p - 1 - s1);
		} while (sc != 0);
	}
	/* NOTREACHED */
}

char * strichr(char *p, int c)
{
	char *t;

	if (p != NULL) {
		for (t = p; *t; t++);
		for (; t >= p; t--) {
			*(t + 1) = *t;
		}
		*p = c;
	}
	return (p);
}

/*#include "ctype.h"
unsigned long strtoul(const char *nptr, char **endptr, int base)
{
	register const char *s = nptr;
	register unsigned long acc;
	register int c;
	register unsigned long cutoff;
	register int neg = 0, any, cutlim;

	
	do {
		c = *s++;
	} while (isspace(c));
	if (c == '-') {
		neg = 1;
		c = *s++;
	}
	else if (c == '+')
		c = *s++;
	if ((base == 0 || base == 16) &&
		c == '0' && (*s == 'x' || *s == 'X')) {
		c = s[1];
		s += 2;
		base = 16;
	}
	if (base == 0)
		base = c == '0' ? 8 : 10;
	cutoff = (unsigned long)ULONG_MAX / (unsigned long)base;
	cutlim = (unsigned long)ULONG_MAX % (unsigned long)base;
	for (acc = 0, any = 0;; c = *s++) {
		if (isdigit(c))
			c -= '0';
		else if (isalpha(c))
			c -= isupper(c) ? 'A' - 10 : 'a' - 10;
		else
			break;
		if (c >= base)
			break;
		if (any < 0 || acc > cutoff || acc == cutoff && c > cutlim)
			any = -1;
		else {
			any = 1;
			acc *= base;
			acc += c;
		}
	}
	if (any < 0) {
		acc = ULONG_MAX;
		//errno = ERANGE;
	}
	else if (neg)
		acc = -acc;
	if (endptr != 0)
		*endptr = (char *)(any ? s - 1 : nptr);
	return (acc);
}*/

/*
char*	strndup(const char *s, size_t n)
{
	char *result;
	result = (char*)new char[n + 1];
	memcpy(result, s, n + 1);
	result[n] = 0;
	return result;
}


char*	strdup(const char *s)
{
	char *result;
	int len = strlen(s);
	result = (char*)new char[len + 1];
	memcpy(result, s, len + 1);
	return result;
}*/

char* strtok_r(char *s, const char *delim, char **save_ptr)
{
	char *end;

	if (s == NULL)
		s = *save_ptr;

	if (*s == '\0')

	{

		*save_ptr = s;

		return NULL;

	}
	/* Scan leading delimiters.  */

	s += strspn(s, delim);

	if (*s == '\0')

	{
		*save_ptr = s;
		return NULL;

	}

	/* Find the end of the token.  */
	end = s + strcspn(s, delim);
	if (*end == '\0')
	{

		*save_ptr = end;

		return s;
	}

	/* Terminate the token and make *SAVE_PTR point past it.  */
	*end = '\0';
	*save_ptr = end + 1;

	return s;
}
