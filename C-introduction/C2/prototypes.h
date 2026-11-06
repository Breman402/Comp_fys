#pragma once

#include <gsl/gsl_rng.h>
#include <gsl/gsl_randist.h>
#include <math.h>

struct matrix {
    int rows;
    int cols;
    double **data; // ** means pointer to pointer, which is used to create a 2D array (array of arrays)
};

double computeScalarProduct(double *x, double *y, int n);
void fillVectorWithRandomNumbers(double *vector, int n);
int vectorLengthFromCommandLine();
struct matrix createNx3Array(int n);
double distance(struct matrix *m, int i, int j);
void fillMatrixWithRandomNumbers(struct matrix *m);

