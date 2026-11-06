#include <stdio.h>

#include "vector.h"

void
elementwise_addition(
		     double *res,
		     double *v1,
		     double *v2,
		     unsigned int len
	            )
{
    for(int i = 0; i < len; ++i){
	res[i] = v1[i] + v1[i];
    }
}

void
constant_multiplication(
			   double *res,
			   double *v1,
			   double a,
			   unsigned int len
	                  )
{
    for(int i = 0; i < len; ++i){
	res[i] = v1[i] * a;
    }
}

double
dot_product(
	    double *v1,
	    double *v2,
	    unsigned int len
	   )
{
    double result = 0;
    for(int i = 0; i < len; ++i){
	result = v1[i] * v2[i];
    }
    return result;
}
