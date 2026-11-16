#include <stdio.h>
#include <math.h>

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


// This function calculates the L2 norm of a vector v of length len
// Added by me to test the testing framework.
double l2_norm(
		double *v,
		unsigned int len
		)
{
	double result = 0;
	for(int i = 0; i < len; ++i){
		result += v[i] * v[i];
	}
	return sqrt(result);
}