#include "stddef.h"
#include "Math.h"

double frexp(double value, int *eptr)
{
	union {
		double v;
		struct ieee_double s;
	} u;

	if (value) {
		/*
		* Fractions in [0.5..1.0) have an exponent of 2^-1.
		* Leave Inf and NaN alone, however.
		* WHAT ABOUT DENORMS?
		*/
		u.v = value;
		if (u.s.dbl_exp != DBL_EXP_INFNAN) {
			*eptr = u.s.dbl_exp - (DBL_EXP_BIAS - 1);
			u.s.dbl_exp = DBL_EXP_BIAS - 1;
		}
		return (u.v);
	}
	else {
		*eptr = 0;
		return (0.0);
	}
}

int abs(int j)
{
	return(j < 0 ? -j : j);
}

float fabs(float j)
{
	return(j < 0 ? -j : j);
}

int floor(float x)
{
	if (x >= 0)
	{
		return (int)x;
	}
	else
	{
		int y = (int)x;
		return ((float)y == x) ? y : y - 1;
	}
}

double sin(float angle) 
{
	int y = 0;
	double radians;
	radians = angle * (M_PI / 180.0);
	if (radians > 0.0) {
		for (y = 1; !((fabs(pow(radians, y)) / fact(y)) - (pow(radians, y + 2) / fact(y + 2)) < EPSILON); y = y + 2);//empty body

	}
	return ((pow(radians, y)) / fact(y));
}

/* Function to calculate x raised to the power y

Time Complexity: O(n)
Space Complexity: O(1)
Algorithmic Paradigm: Divide and conquer.
*/
int power1(int x, unsigned int y)
{
	if (y == 0)
		return 1;
	else if ((y % 2) == 0)
		return power1(x, y / 2) * power1(x, y / 2);
	else
		return x * power1(x, y / 2) * power1(x, y / 2);

}

/* Function to calculate x raised to the power y in O(logn)
Time Complexity of optimized solution: O(logn)
*/
int power2(int x, unsigned int y)
{
	int temp;
	if (y == 0)
		return 1;

	temp = power2(x, y / 2);
	if ((y % 2) == 0)
		return temp * temp;
	else
		return x * temp * temp;
}

/* Extended version of power function that can work
for float x and negative y
*/
float pow(float x, int y)
{
	float temp;
	if (y == 0)
		return 1;
	temp = pow(x, y / 2);
	if ((y % 2) == 0) {
		return temp * temp;
	}
	else {
		if (y > 0)
			return x * temp * temp;
		else
			return (temp * temp) / x;
	}
}

/* Extended version of power function that can work
for double x and negative y
*/
double powerd(double x, int y)
{
	double temp;
	if (y == 0)
		return 1;
	temp = powerd(x, y / 2);
	if ((y % 2) == 0) {
		return temp * temp;
	}
	else {
		if (y > 0)
			return x * temp * temp;
		else
			return (temp * temp) / x;
	}
}

long fact(int n)
{
	int c;
	long result = 1;

	for (c = 1; c <= n; c++)
		result = result * c;

	return result;
}

float fmod(float a, float b)
{
	return (a - b * floor(a / b));
}