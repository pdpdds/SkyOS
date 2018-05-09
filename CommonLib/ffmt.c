#include "windef.h"
#include "string.h"

static int nIsDigit( char ch )
{
	if( '0' <= ch && ch <= '9' )
		return( 1 );
	else
		return( 0 );
}

long _ffmt(char *outptr, char *fmt, unsigned long *argptr)
{
	char numstk[33], *ptr, justify, zero, minus, chr;
	unsigned long width, value, i, total;

	total = 0;
	chr = *fmt++;
	while(chr != 0)
	{
		if(chr == '%') 
		{					/* format code */
			chr = *fmt++;
            ptr = &numstk[32];
			*ptr = justify = minus = 0;
			width = value = i = 0;
			zero = ' ';
			if(chr == '-')
			{				/* left justify */
				--justify;
				chr = *fmt++;
			}
			if(chr == '0')					/* leading zeros */
				zero = '0';
			while( nIsDigit(chr) )
			{			/* field width specifier */
				width = (width * 10) + (chr - '0');
				chr = *fmt++;
			}

			value = *argptr;				/* get parameter value */
			argptr++;

			switch(chr)
			{
				case 'd' :					/* decimal number */
					if(value & 0x80000000)
					{
						value = (DWORD)~value;
						value++;
						++minus;
					}
				case 'u' :					/* unsigned number */
					i = 10;
					break;
				case 'x' :					/* hexidecimal number */
				case 'X' :
					i = 16;
					break;
				case 'o' :					/* octal number */
					i = 8;
					break;
				case 'b' :					/* binary number */
					i = 2;
					break;
				case 'c' :					/* character data */
					*--ptr = (char)value;
					break;
				case 's' :					/* string */
					ptr = (char*)value;			/* value is ptr to string */
					break;
				default:					/* all others */
					*--ptr = chr;
					++argptr;				/* backup to last arg */
			}

			if(i)		/* for all numbers, generate the ASCII string */
				do 
				{
					if((chr = (char)(value % i) + '0') > '9')
						chr += 7;
					*--ptr = chr; 
				}
				while(value /= i);

			/* output sign if any */

			if(minus) 
			{
				*outptr++ = '-';
				++total;
				if(width)
					--width;
			}

			/* pad with 'zero' value if right justify enabled  */

			if(width && !justify) 
			{
				for(i = strlen(ptr); i < width; ++i)
					*outptr++ = zero;
					++total;
			}

			if( width == 0 )
				width = strlen( ptr );

			/* move in data */
			i = 0;
			value = width - 1;

			while((*ptr) && (i <= value)) 
			{
				*outptr++ = *ptr++;
				++total;
				++i;
			}

			/* pad with 'zero' value if left justify enabled */

			if(width && justify) 
			{
				while(i < width) 
				{
					*outptr++ = zero;
					++total;
					++i;
				}
			}
		}
		else 
		{
			/* not format char, just move into string  */
			*outptr++ = chr;
			++total;
		}

		chr = *fmt++;
	}

	*outptr = 0;
	return total;
}

/************************************
    Formatted print to string s
*************************************/
/*
long zsprintf( char *s, char *fmt, ...)
{
	va_list ap;
	long total;

	va_start(ap, fmt);			// set up ap pointer
	total = _ffmt(s, fmt, (long*)ap);
	va_end( ap );

	return total;
}
*/