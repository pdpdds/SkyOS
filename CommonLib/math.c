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
/*
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
}*/

/*double sin(float angle) 
{
	int y = 0;
	double radians;
	radians = angle * (M_PI / 180.0);
	if (radians > 0.0) {
		for (y = 1; !((fabs(pow(radians, y)) / fact(y)) - (pow(radians, y + 2) / fact(y + 2)) < EPSILON); y = y + 2);//empty body

	}
	return ((pow(radians, y)) / fact(y));
}*/

double cos(double x) {//USE BETWEEN -pi, pi
	double sq_x = x*x, output = 1.0, qu_x = sq_x*sq_x;
	double fac[] = { 2.0, 24.0, 720.0, 40320.0, 3628800.0, 479001600.0 };//2!, 4!, 6!, 8!, 10!, 12!...
	for (int i = 0; i<6; i = i + 2) {
		output -= (sq_x / fac[i]);
		sq_x = sq_x*qu_x;
	}
	sq_x = qu_x;
	for (int i = 1; i<6; i = i + 2) {
		output += (sq_x / fac[i]);
		sq_x = sq_x*qu_x;
	}
	return output;
}

double tan(double x) {//USE BETWEEN -pi/2, pi/2
	double sq_x = x*x, output = x, tr_x = sq_x*x;
	double fac[] = { 1.0 / 3.0, 2.0 / 15.0, 17.0 / 315.0, 62.0 / 2835.0, 1382.0 / 155925.0, 21844.0 / 6081075.0 };
	for (int i = 0; i<6; i++) {
		output += fac[i] * tr_x;
		tr_x = tr_x*sq_x;
	}
	return output;
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

double powerOfTen(int num) {
	double rst = 1.0;
	if (num >= 0) {
		for (int i = 0; i < num; i++) {
			rst *= 10.0;
		}
	}
	else {
		for (int i = 0; i < (0 - num); i++) {
			rst *= 0.1;
		}
	}

	return rst;
}

double sqrt(double a)
{
	/*
	find more detail of this method on wiki methods_of_computing_square_roots

	*** Babylonian method cannot get exact zero but approximately value of the square_root
	*/
	double z = a;
	double rst = 0.0;
	int max = 8;	// to define maximum digit 
	int i;
	double j = 1.0;
	for (i = max; i > 0; i--) {
		// value must be bigger then 0
		if (z - ((2 * rst) + (j * powerOfTen(i)))*(j * powerOfTen(i)) >= 0)
		{
			while (z - ((2 * rst) + (j * powerOfTen(i)))*(j * powerOfTen(i)) >= 0)
			{
				j++;
				if (j >= 10) break;

			}
			j--; //correct the extra value by minus one to j
			z -= ((2 * rst) + (j * powerOfTen(i)))*(j * powerOfTen(i)); //find value of z

			rst += j * powerOfTen(i);	// find sum of a

			j = 1.0;


		}

	}

	for (i = 0; i >= 0 - max; i--) {
		if (z - ((2 * rst) + (j * powerOfTen(i)))*(j * powerOfTen(i)) >= 0)
		{
			while (z - ((2 * rst) + (j * powerOfTen(i)))*(j * powerOfTen(i)) >= 0)
			{
				j++;
				if (j >= 10) break;
			}
			j--;
			z -= ((2 * rst) + (j * powerOfTen(i)))*(j * powerOfTen(i)); //find value of z

			rst += j * powerOfTen(i);	// find sum of a			
			j = 1.0;
		}
	}
	// find the number on each digit
	return rst;
}

static unsigned long int next = 1;

int rand(void) // RAND_MAX assumed to be 32767
{
	next = next * 1103515245 + 12345;
	return (unsigned int)(next / 65536) % 32768;
}

void srand(unsigned int seed)
{
	next = seed;
}