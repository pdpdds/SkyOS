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


#ifdef  __cplusplus
extern "C" {
#endif
	extern double log(double x);
	extern double log10(double x);
	extern double atan2(double y, double x);
	extern double exp(double x);
	extern double asin(double x);
	extern double acos(double x);
	double frexp(double value, int *eptr);
	int abs(int j);
	float pow(float x, int y);
	long fact(int n); //factorial function
	float fabs(float j);
	extern double sin(double x);
	//double sin(float angle);
	double cos(double x);
	double tan(double x);
	double sqrt(double a);
	extern double ldexp(double value, int exp);
	extern double atan(double x);
	int rand(void);
	void srand(unsigned int seed);
	extern double ceil(double x);
	float fmod(float a, float b);
	int floor(float x);

#ifdef  __cplusplus
}
#endif

#define MIN( x, y )     ( ( ( x ) < ( y ) ) ? ( x ) : ( y ) )
#define MAX( x, y )     ( ( ( x ) > ( y ) ) ? ( x ) : ( y ) )


#define RAND_MAX 0x7fff

#define M_E	2.71828182845904523536 //e	
#define M_LOG2E	 1.44269504088896340736 // log2(e)	
#define M_LOG10E	0.434294481903251827651 //	log10(e)
#define M_LN2		0.693147180559945309417 //ln(2)
#define M_LN10	2.30258509299404568402 //ln(10)	
#define M_PI  3.14159265358979323846 //	pi	
#define M_PI_2		1.57079632679489661923 //pi / 2
#define M_PI_4		0.785398163397448309616 //pi / 4
#define M_1_PI		0.318309886183790671538 //1 / pi
#define M_2_PI		0.636619772367581343076 //2 / pi
#define M_2_SQRTPI		1.12837916709551257390 //2 / sqrt(pi)
#define M_SQRT2		1.41421356237309504880 //sqrt(2)
#define M_SQRT1_2		0.707106781186547524401 //1 / sqrt(2)
#define PI		M_PI
#define PI2		M_PI_2


#define FLT_EPSILON	1.192092896e-07F
#define EPSILON FLT_EPSILON
