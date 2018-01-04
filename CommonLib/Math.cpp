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