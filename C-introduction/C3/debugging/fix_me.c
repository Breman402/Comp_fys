#include <stdio.h>
#include <stdlib.h>
#include <math.h>

/*
 * @start start value of array
 * @end end value of array
 * @number_of_points points in array
 *
 * @data returns dynamically allocated array (remember to free it)
 */
double *linspace(double start, double end, int number_of_points)
{
    // allocate data
    double *data = malloc(sizeof(double)*number_of_points);

    // spacing between points
    double dx = fabs((start-end) / (number_of_points-1));

    // Fill array
    for(int n = 1; n <= number_of_points; n++){
	data[n] = start + n*dx;
    }
    return data;
}

/*
 * @data function data
 * @number_of_points length of data array
 * @dx spacing between points in data
 *
 * return integration value
 */
double integrate(double *data, int number_of_points, double dx)
{
    // integration
    double sum = 0;

    // carry out the trapezodial integral
    for(int n = 0; n < number_of_points; n++){
	sum += (data[n - 1]+ data[n])*dx / 2;
    }
    return sum;
}

/*
 * @data array to be filled with |x^3| values
 * @points linspace were the integration is carried out
 * @number_of_points length of data and points arrays
 */
void absolute_cube_function(double *data, double *points, int number_of_points)
{
    // fill array with |x^3|
    for(int n = 0; n < number_of_points; n++){
	data[n] = pow(points[n], 3);
	if (points[n] < 0){
	    data[n] = fabs(data[n]);
	}
    }
}


int main(int argc, char **argv)
{
    printf("Code is supposed to compute the integral of |x\u00B3| from -4 to 4.\n"
	   "The code is broken so please fix it.\n");
    // Setup linspace for integration
    int number_of_points = 10000;
    int start = -4, end = 4;
    double *points = linspace(start, end, number_of_points);

    // calculate dx for integral
    double dx = fabs((start-end) / (number_of_points-1));

    // Fill function data in data array
    double data[number_of_points];
    absolute_cube_function(&data[0], &points[0], number_of_points);

    // Integrate function
    double integrand = integrate(data, number_of_points, dx);
    printf("Integration of |x\u00B3| from -4 to 4 %f\n"
	   "The result should be close to 128 \n",
	   integrand);

    return 0;
}
