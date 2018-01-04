#pragma once


#define DBL_EXPBITS 11
#define DBL_FRACHBITS   20
#define DBL_FRACLBITS   32


struct ieee_double {
#if _BYTE_ORDER == _BIG_ENDIAN
	unsigned int   dbl_sign : 1;
	unsigned int   dbl_exp : DBL_EXPBITS;
	unsigned int   dbl_frach : DBL_FRACHBITS;
	unsigned int   dbl_fracl : DBL_FRACLBITS;
#else
	unsigned int   dbl_fracl : DBL_FRACLBITS;
	unsigned int   dbl_frach : DBL_FRACHBITS;
	unsigned int   dbl_exp : DBL_EXPBITS;
	unsigned int   dbl_sign : 1;
#endif

};

double frexp(double value, int *eptr);
int abs(int j);

#define MIN( x, y )     ( ( ( x ) < ( y ) ) ? ( x ) : ( y ) )
#define MAX( x, y )     ( ( ( x ) > ( y ) ) ? ( x ) : ( y ) )
