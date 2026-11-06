#include "prototypes.h"

/** Compute the scalar product of two vectors.
 * @param x: pointer to the first vector
 * @param y: pointer to the second vector
 * @param n: the number of elements in the vectors
 * @return: the scalar product of the two vectors
 */
double computeScalarProduct(double *x, double *y, int n) {
    double result = 0.0f;
    for (int i = 0; i < n; i++) {
        result += x[i] * y[i];
    }
    return result;
}

/** Fill a vector with random numbers using GSL.
 * @param vector: pointer to the vector to be filled
 * @param n: the number of elements in the vector
 */
void fillVectorWithRandomNumbers(double *vector, int n) {
    const gsl_rng_type * T;
    gsl_rng * r;

    gsl_rng_env_setup(); // Initialize the GSL random number generator environment
    T = gsl_rng_default;
    r = gsl_rng_alloc(T);

    static int seed = 42;
    seed++;
    gsl_rng_set(r, seed); 

    for (int i = 0; i < n; i++) {
        vector[i] = gsl_rng_uniform(r);
    }

    gsl_rng_free(r); // Free the allocated random number generator
}

/** 
 * Read the length of the vectors from the command line using scanf.
 */
int vectorLengthFromCommandLine() {
    int n;
    printf("Enter the length of the vectors: ");
    scanf("%d", &n);
    return n;
}