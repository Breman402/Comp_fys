#include <stdio.h>

#include "vector.h"

void
elementwise_addition(
		     double *res,
		     double *v1,
		     double *v2,
		     unsigned int len
	            )
{	// This test failed initially. This is how the code used to look like:
    /*
	for(int i = 0; i < len; ++i){
	res[i] = v1[i] + v1[i];
    }
	*/

	// And this is the correct implementation:
	for(int i = 0; i < len; ++i){
	res[i] = v1[i] + v2[i];
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
	// This test failed initially. This is how the code used to look like:
	/*
    double result = 0;
    for(int i = 0; i < len; ++i){
	result = v1[i] * v2[i];
    }
    return result;
	*/

	// And this is the correct implementation:
	double result = 0;
	for(int i = 0; i < len; ++i){
		result += v1[i] * v2[i];
	}
	return result;
}
