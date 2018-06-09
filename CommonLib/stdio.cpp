
#include <string.h>
#include <stdio.h>
#include <ctype.h>
#include "limits.h"
#include "sprintf.h"
#include "ffmt.h"
#include "memory.h"
#include "stdint.h"



/*int vsprintf(char *buffer, char *format, va_list argptr)
{
	int nR;
	nR = _ffmt(buffer, format, (unsigned long*)argptr);
	return(nR);
}*/

//! writes formatted string to buffer
extern "C" int vsprintf(char *str, const char *format, va_list ap) {

	if (!str)
		return 0;

	if (!format)
		return 0;

	size_t loc = 0;
	size_t i;

	for (i = 0; i <= strlen(format); i++, loc++)
	{
		switch (format[i])
		{
		case '%':
			switch (format[i + 1])
			{
				/*** characters ***/
			case 'c':
			{
				char c = va_arg(ap, char);
				str[loc] = c;
				i++;
				break;
			}

			/*** integers ***/
			case 'd':
			case 'I':
			case 'i':
			{
				int c = va_arg(ap, int);

				if (c < 0)
				{
					str[loc] = '-';
					loc++;
					c = (-c);
				}

				char s[32] = { 0 };
				itoa_s(c, 10, s);
				strcpy(&str[loc], s);
				loc += strlen(s) - 1;
				i++;		// go to next character
				break;
			}

			/*** display in hex ***/
			case 'X':
			case 'p':
			{
				int c = va_arg(ap, int);
				char s[32] = { 0 };

				if (c < 0)
				{
					str[loc] = '-';
					loc++;
					c = (-c);
				}

				itoa_s(c, 16, s);
				strcpy(&str[loc], s);
				i++;		// go to next character
				loc += strlen(s) - 1;
				break;
			}
			case 'x':
			
			{
				unsigned int c = va_arg(ap, unsigned int);
				char s[32] = { 0 };
				itoa_s(c, 16, s);
				strcpy(&str[loc], s);
				i++;		// go to next character
				loc += strlen(s) - 1;
				break;
			}
			/*** strings ***/
			case 's':
			{
				int c = (int&)va_arg(ap, char);
				char s[32] = { 0 };
				strcpy(s, (const char*)c);
				strcpy(&str[loc], s);
				i++;		// go to next character
				loc += strlen(s) - 1;
				break;
			}

			case 'f':
			{
				double double_temp;
				double_temp = va_arg(ap, double);
				char buffer[512];
				ftoa_fixed(buffer, double_temp);
				strcpy(&str[loc], buffer);
				i++;
				loc += strlen(buffer) - 1;
				break;
			}

			case 'Q':
			{
				__int64 int64_temp;
				int64_temp = va_arg(ap, __int64);
				char buffer[20];
				_i64toa(int64_temp, buffer, 10);
				strcpy(&str[loc], buffer);
				i++;
				loc += strlen(buffer) - 1;
				break;
			}

			case 'q':
			{
				uint64_t int64_temp;
				int64_temp = va_arg(ap, uint64_t);
				char buffer[20];
				_i64toa(int64_temp, buffer, 16);
				strcpy(&str[loc], buffer);
				i++;
				loc += strlen(buffer) - 1;
				break;
			}
			case 'l':
			{
				if (format[i + 2] == 'd')
				{
					int c = (int&)va_arg(ap, char);
					char s[32] = { 0 };
					itoa_s(c, 10, s);
					strcpy(&str[loc], s);
					loc += strlen(s) - 1;
					// go to next character
					i++;
				}
				i++;
				break;

			}

			}
			break;

		default:
			str[loc] = format[i];
			break;
		}
	}

	return i;
}

extern "C" int vnsprintf(char *str, size_t size, const char *format, va_list ap) {

	if (!str)
		return 0;

	if (!format)
		return 0;

	size_t loc = 0;
	size_t i;

	for (i = 0; i <= strlen(format); i++, loc++)
	{
		switch (format[i])
		{
		case '%':
			switch (format[i + 1])
			{
				/*** characters ***/
			case 'c':
			{
				char c = va_arg(ap, char);
				str[loc] = c;
				i++;
				break;
			}

			/*** integers ***/
			case 'd':
			case 'I':
			case 'i':
			{
				int c = va_arg(ap, int);
				char s[32] = { 0 };
				itoa_s(c, 10, s);
				strcpy(&str[loc], s);
				loc += strlen(s) - 1;
				i++;		// go to next character
				break;
			}

			/*** display in hex ***/
			case 'X':
			case 'p':
			{
				int c = va_arg(ap, int);
				char s[32] = { 0 };
				itoa_s(c, 16, s);
				strcpy(&str[loc], s);
				i++;		// go to next character
				loc += strlen(s) - 1;
				break;
			}
			case 'x':

			{
				unsigned int c = va_arg(ap, unsigned int);
				char s[32] = { 0 };
				itoa_s(c, 16, s);
				strcpy(&str[loc], s);
				i++;		// go to next character
				loc += strlen(s) - 1;
				break;
			}
			/*** strings ***/
			case 's':
			{
				int c = (int&)va_arg(ap, char);
				char s[32] = { 0 };
				strcpy(s, (const char*)c);
				strcpy(&str[loc], s);
				i++;		// go to next character
				loc += strlen(s) - 1;
				break;
			}

			case 'f':
			{
				double double_temp;
				double_temp = va_arg(ap, double);
				char buffer[512];
				ftoa_fixed(buffer, double_temp);
				strcpy(&str[loc], buffer);
				i++;
				loc += strlen(buffer) - 1;
				break;
			}

			case 'Q':
			{
				__int64 int64_temp;
				int64_temp = va_arg(ap, __int64);
				char buffer[20];
				_i64toa(int64_temp, buffer, 10);
				strcpy(&str[loc], buffer);
				i++;
				loc += strlen(buffer) - 1;
				break;
			}

			case 'q':
			{
				uint64_t int64_temp;
				int64_temp = va_arg(ap, uint64_t);
				char buffer[20];
				_i64toa(int64_temp, buffer, 16);
				strcpy(&str[loc], buffer);
				i++;
				loc += strlen(buffer) - 1;
				break;
			}
			case 'l':
			{
				if (format[i + 2] == 'd')
				{
					int c = (int&)va_arg(ap, char);
					char s[32] = { 0 };
					itoa_s(c, 10, s);
					strcpy(&str[loc], s);
					loc += strlen(s) - 1;
					// go to next character
					i++;
				}
				i++;
				break;

			}

			}
			break;

		default:
			str[loc] = format[i];
			break;
		}
	}

	return i;
}


#ifndef SKYOS_WIN32
//! converts a string to a long
long
strtol(const char* nptr, char** endptr, int base)
{
	const char *s = nptr;
	unsigned long acc;
	int c;
	unsigned long cutoff;
	int neg = 0, any, cutlim;

	/*
	 * Skip white space and pick up leading +/- sign if any.
	 * If base is 0, allow 0x for hex and 0 for octal, else
	 * assume decimal; if base is already 16, allow 0x.
	 */
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
	else if ((base == 0 || base == 2) &&
		c == '0' && (*s == 'b' || *s == 'B')) {
		c = s[1];
		s += 2;
		base = 2;
	}
	if (base == 0)
		base = c == '0' ? 8 : 10;

	/*
	 * Compute the cutoff value between legal numbers and illegal
	 * numbers.  That is the largest legal value, divided by the
	 * base.  An input number that is greater than this value, if
	 * followed by a legal input character, is too big.  One that
	 * is equal to this value may be valid or not; the limit
	 * between valid and invalid numbers is then based on the last
	 * digit.  For instance, if the range for longs is
	 * [-2147483648..2147483647] and the input base is 10,
	 * cutoff will be set to 214748364 and cutlim to either
	 * 7 (neg==0) or 8 (neg==1), meaning that if we have accumulated
	 * a value > 214748364, or equal but the next digit is > 7 (or 8),
	 * the number is too big, and we will return a range error.
	 *
	 * Set any if any `digits' consumed; make it negative to indicate
	 * overflow.
	 */
	cutoff = neg ? -(unsigned long)LONG_MIN : LONG_MAX;
	cutlim = cutoff % (unsigned long)base;
	cutoff /= (unsigned long)base;
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
		acc = neg ? LONG_MIN : LONG_MAX;
		//		errno = ERANGE;
	}
	else if (neg)
		acc = -acc;
	if (endptr != 0)
		*endptr = (char *)(any ? s - 1 : nptr);
	return (acc);
}

//! converts a string to an unsigned long
unsigned long
strtoul(const char* nptr, char** endptr, int base)
{
	const char *s = nptr;
	unsigned long acc;
	int c;
	unsigned long cutoff;
	int neg = 0, any, cutlim;

	/*
	 * See strtol for comments as to the logic used.
	 */
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
	else if ((base == 0 || base == 2) &&
		c == '0' && (*s == 'b' || *s == 'B')) {
		c = s[1];
		s += 2;
		base = 2;
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
		//		errno = ERANGE;
	}
	else if (neg)
		acc = -acc;
	if (endptr != 0)
		*endptr = (char *)(any ? s - 1 : nptr);
	return (acc);
}


//! convert string to int
int atoi(const char * str) {

	return (int)strtol(str, 0, 10);
}

double atof(char *p)
{
	double          d = 0, t = 0;
	int             len = 0, val = 0, sz = 0, div = 0, isneg = 0;
	char            tmp[18];
	memset(tmp, 0, 18);

	if (*p == '-') {
		isneg = 1;
		p++;
	}
	else
		isneg = 0;

	sz = strcspn(p, ".");
	if (sz > 0) {
		strncpy(tmp, p, sz);
		tmp[sz] = 0;
		if (!atob(&val, tmp, 10))
			return (d);
	}
	else
		val = 0;

	d = (double)val;
	p += sz;
	if (*p)
		p++;
	if (*p) {
		len = strlen(p);
		if (!atob(&val, p, 10))
			return (0);

		div = 1;
		for (; len > 0; len--)
			div *= 10;

		t = (double)val;
		t /= div;

		d += t;
	}
	if (isneg)
		d = 0 - d;
	return (d);
}

/**
* simple_strtoul - convert a string to an unsigned long
* @cp: The start of the string
* @endp: A pointer to the end of the parsed string will be placed here
* @base: The number base to use
*/
unsigned long simple_strtoul(const char *cp, char **endp, unsigned int base)
{
	unsigned long result = 0, value;

	if (!base) {
		base = 10;
		if (*cp == '0') {
			base = 8;
			cp++;
			if ((*cp == 'x') && isxdigit(cp[1])) {
				cp++;
				base = 16;
			}
		}
	}
	while (isxdigit(*cp) &&
		(value = isdigit(*cp) ? *cp - '0' : toupper(*cp) - 'A' + 10) < base) {
		result = result * base + value;
		cp++;
	}
	if (endp)
		*endp = (char *)cp;
	return result;
}


/**
* simple_strtol - convert a string to a signed long
* @cp: The start of the string
* @endp: A pointer to the end of the parsed string will be placed here
* @base: The number base to use
*/
long simple_strtol(const char *cp, char **endp, unsigned int base)
{
	if (*cp == '-')
		return -simple_strtoul(cp + 1, endp, base);
	return simple_strtoul(cp, endp, base);
}

/**
* simple_strtoull - convert a string to an unsigned long long
* @cp: The start of the string
* @endp: A pointer to the end of the parsed string will be placed here
* @base: The number base to use
*/
unsigned long long simple_strtoull(const char *cp, char **endp, unsigned int base)
{
	//unsigned long long => unsigned long
	unsigned long result = 0, value;

	if (!base) {
		base = 10;
		if (*cp == '0') {
			base = 8;
			cp++;
			if ((*cp == 'x') && isxdigit(cp[1])) {
				cp++;
				base = 16;
			}
		}
	}
	while (isxdigit(*cp) && (value = isdigit(*cp) ? *cp - '0' : (islower(*cp)
		? toupper(*cp) : *cp) - 'A' + 10) < base) {
		result = base * result + value;
		cp++;
	}
	if (endp)
		*endp = (char *)cp;
	return result;
}

/**
* simple_strtoll - convert a string to a signed long long
* @cp: The start of the string
* @endp: A pointer to the end of the parsed string will be placed here
* @base: The number base to use
*/
long long simple_strtoll(const char *cp, char **endp, unsigned int base)
{
	if (*cp == '-')
		return -simple_strtoull(cp + 1, endp, base);
	return simple_strtoull(cp, endp, base);
}

static int skip_atoi(const char **s)
{
	int i = 0;

	while (isdigit(**s))
		i = i * 10 + *((*s)++) - '0';
	return i;
}

/**
* vsscanf - Unformat a buffer into a list of arguments
* @buf:	input buffer
* @fmt:	format of buffer
* @args:	arguments
*/
int vsscanf(const char * buf, const char * fmt, va_list args)
{
	const char *str = buf;
	char *next;
	char digit;
	int num = 0;
	int qualifier;
	int base;
	int field_width;
	int is_sign = 0;

	while (*fmt && *str) {
		/* skip any white space in format */
		/* white space in format matchs any amount of
		* white space, including none, in the input.
		*/
		if (isspace(*fmt)) {
			while (isspace(*fmt))
				++fmt;
			while (isspace(*str))
				++str;
		}

		/* anything that is not a conversion must match exactly */
		if (*fmt != '%' && *fmt) {
			if (*fmt++ != *str++)
				break;
			continue;
		}

		if (!*fmt)
			break;
		++fmt;

		/* skip this conversion.
		* advance both strings to next white space
		*/
		if (*fmt == '*') {
			while (!isspace(*fmt) && *fmt)
				fmt++;
			while (!isspace(*str) && *str)
				str++;
			continue;
		}

		/* get field width */
		field_width = -1;
		if (isdigit(*fmt))
			field_width = skip_atoi(&fmt);

		/* get conversion qualifier */
		qualifier = -1;
		if (*fmt == 'h' || *fmt == 'l' || *fmt == 'L' ||
			*fmt == 'Z' || *fmt == 'z') {
			qualifier = *fmt;
			fmt++;
		}
		base = 10;
		is_sign = 0;

		if (!*fmt || !*str)
			break;

		switch (*fmt++) {
		case 'c':
		{
			char *s = (char *)va_arg(args, char*);
			if (field_width == -1)
				field_width = 1;
			do {
				*s++ = *str++;
			} while (--field_width > 0 && *str);
			num++;
		}
		continue;
		case 's':
		{
			char *s = (char *)va_arg(args, char *);
			if (field_width == -1)
				field_width = sizeof(int);
			/* first, skip leading white space in buffer */
			while (isspace(*str))
				str++;

			/* now copy until next white space */
			while (*str && !isspace(*str) && field_width--) {
				*s++ = *str++;
			}
			*s = '\0';
			num++;
		}
		continue;
		case 'n':
			/* return number of characters read so far */
		{
			int *i = (int *)va_arg(args, int*);
			*i = str - buf;
		}
		continue;
		case 'o':
			base = 8;
			break;
		case 'x':
		case 'X':
			base = 16;
			break;
		case 'i':
			base = 0;
		case 'd':
			is_sign = 1;
		case 'u':
			break;
		case '%':
			/* looking for '%' in str */
			if (*str++ != '%')
				return num;
			continue;
		default:
			/* invalid format; stop here */
			return num;
		}

		/* have some sort of integer conversion.
		* first, skip white space in buffer.
		*/
		while (isspace(*str))
			str++;

		digit = *str;
		if (is_sign && digit == '-')
			digit = *(str + 1);

		if (!digit
			|| (base == 16 && !isxdigit(digit))
			|| (base == 10 && !isdigit(digit))
			|| (base == 8 && (!isdigit(digit) || digit > '7'))
			|| (base == 0 && !isdigit(digit)))
			break;

		switch (qualifier) {
		case 'h':
			if (is_sign) {
				short *s = (short *)va_arg(args, short *);
				*s = (short)simple_strtol(str, &next, base);
			}
			else {
				unsigned short *s = (unsigned short *)va_arg(args, unsigned short *);
				*s = (unsigned short)simple_strtoul(str, &next, base);
			}
			break;
		case 'l':
			if (is_sign) {
				long *l = (long *)va_arg(args, long *);
				*l = simple_strtol(str, &next, base);
			}
			else {
				unsigned long *l = (unsigned long*)va_arg(args, unsigned long*);
				*l = simple_strtoul(str, &next, base);
			}
			break;
		case 'L':
			if (is_sign) {
				long long *l = (long long*)va_arg(args, long long *);
				*l = simple_strtoll(str, &next, base);
			}
			else {
				unsigned long long *l = (unsigned long long*) va_arg(args, unsigned long long*);
				*l = simple_strtoull(str, &next, base);
			}
			break;
		case 'Z':
		case 'z':
		{
			size_t *s = (size_t*)va_arg(args, size_t*);
			*s = (size_t)simple_strtoul(str, &next, base);
		}
		break;
		default:
			if (is_sign) {
				int *i = (int *)va_arg(args, int*);
				*i = (int)simple_strtol(str, &next, base);
			}
			else {
				unsigned int *i = (unsigned int*)va_arg(args, unsigned int*);
				*i = (unsigned int)simple_strtoul(str, &next, base);
			}
			break;
		}
		num++;

		if (!next)
			break;
		str = next;
	}
	return num;
}

/**
* sscanf - Unformat a buffer into a list of arguments
* @buf:	input buffer
* @fmt:	formatting of buffer
* @...:	resulting arguments
*/
int sscanf(const char * buf, const char * fmt, ...)
{
	va_list args;
	int i;

	va_start(args, fmt);
	i = vsscanf(buf, fmt, args);
	va_end(args);
	return i;
}
#endif