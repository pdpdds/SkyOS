#include <math.h>
#include <stdlib.h>
#include <internal.h>

double
frexp(double __x, int *exptr)
{
	double_t *x = (double_t *)&__x;
	
	if ( exptr != NULL )
		*exptr = x->exponent - 0x3FE;
		
	
	x->exponent = 0x3FE;
	
	return __x; 
}



