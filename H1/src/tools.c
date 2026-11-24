#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <math.h>
#include <gsl/gsl_fft_real.h>
#include <gsl/gsl_fft_halfcomplex.h>
#include <complex.h>

#include "tools.h"

void
elementwise_addition(
    double *res,
    double *v1,
    double *v2,
    unsigned int len
    )
{   
    //#pragma omp parallel for
    for (unsigned int i = 0; i < len; i++) {
        res[i] = v1[i] + v2[i];
    }
}

void
elementwise_multiplication(
    double *res,
    double *v1,
    double *v2,
    unsigned int len
    )
{   
    //#pragma omp parallel for
    for (unsigned int i = 0; i < len; i++) {
        res[i] = v1[i] * v2[i];
    }
}

void
addition_with_constant(
    double *res,
    double *v,
    double constant,
    unsigned int len
    )
{   
    //#pragma omp parallel for
    for (unsigned int i = 0; i < len; i++) {
        res[i] = v[i] + constant;
    }
}

void
multiplication_with_constant(
    double *res,
    double *v,
    double constant,
    unsigned int len
    )
{
    //#pragma omp parallel for
    for (unsigned int i = 0; i < len; i++) {
        res[i] = v[i] * constant;
    }
}

double
dot_product(
    double *v1,
    double *v2,
    unsigned int len
    )
{   
    double result = 0.0;
    //#pragma omp parallel for reduction(+:result)
    for (unsigned int i = 0; i < len; i++) {
        result += v1[i] * v2[i];
    }
    return result;
}

double **
create_2D_array(
    unsigned int rows,
    unsigned int cols
    )
{   
    if (rows == 0 || cols == 0) return NULL; // Check for zero dimensions
    size_t row_count = rows;
    size_t col_count = cols;
    if (row_count > SIZE_MAX / sizeof(double *)) return NULL; // Check for overflow
    if (row_count > SIZE_MAX / sizeof(double) / col_count) return NULL; // Check for overflow

    double **array = malloc(rows * sizeof(double *));
    if (array == NULL) return NULL;

    // Contiguous block of memory for the 2D array
    double *data = malloc((size_t)rows * cols * sizeof(double));
    if (data == NULL) {
        free(array);
        return NULL;
    }

    // Point each row pointer to the beginning of each row in the contiguous block
    for (unsigned int i = 0; i < rows; i++) {
        array[i] = data + (size_t)i * cols;
    }

    return array;
}

void
destroy_2D_array(
    double **array
    )
{
    if (array == NULL) return;

    free(array[0]);  // Free the contiguous block of data
    free(array);     // Free the array of pointers
}

void
matrix_vector_multiplication(
    double *y,
    double **A,
    double *b,
    unsigned int rows,
    unsigned int inner
    )
{   
    //#pragma omp parallel for
    for (unsigned int i = 0; i < rows; i++) {
        double sum = 0.0;

        for (unsigned int j = 0; j < inner; j++) {
            sum += A[i][j] * b[j];
        }

        y[i] = sum;
    }
}

void
matrix_matrix_multiplication(
    double **result,
    double **A,
    double **B,
    unsigned int rows,
    unsigned int inner,
    unsigned int cols
    )
{

    //#pragma omp parallel for
    for (unsigned int i = 0; i < rows; i++) {

        for (unsigned int j = 0; j < cols; j++) {
            result[i][j] = 0.0;
        }

        for (unsigned int k = 0; k < inner; k++) {
            for (unsigned int j = 0; j < cols; j++) {
                result[i][j] += A[i][k] * B[k][j];
            }
        }
    }
}

double
vector_norm(
    double *v1,
    unsigned int len
    )
{
    double sum = 0.0;
    //#pragma omp parallel for reduction(+:sum)
    for (unsigned int i = 0; i < len; i++) {
        sum += v1[i] * v1[i];
    }
    return sqrt(sum);
}


void
normalize_vector(
    double *v1,
    unsigned int len
    )
{
    double norm = vector_norm(v1, len);
    if (norm == 0.0) {
        fprintf(stderr, "Error: Cannot normalize a zero vector.\n");
        return;
    }

    //#pragma omp parallel for
    for (unsigned int i = 0; i < len; i++) {
        v1[i] /= norm;
    }
}

double
average(
    double *v1,
    unsigned int len
    )
{   
    if (len == 0) return NAN;

    double sum = 0.0;
    //#pragma omp parallel for reduction(+:sum)
    for (unsigned int i = 0; i < len; i++) {
        sum += v1[i];
    }
    return sum / len;
}


double
standard_deviation(
    double *v1,
    unsigned int len
    )
{   
    if (len == 0) return NAN;

    double avg = average(v1, len);
    double sum = 0.0;
    //#pragma omp parallel for reduction(+:sum)
    for (unsigned int i = 0; i < len; i++) {
        double diff = v1[i] - avg;
        sum += diff * diff;
    }
    return sqrt(sum / len);
}

double
distance_between_vectors(
    double *v1,
    double *v2,
    unsigned int len
    )
{
    double sum = 0.0;
    //#pragma omp parallel for reduction(+:sum)
    for (unsigned int i = 0; i < len; i++) {
        double diff = v1[i] - v2[i];
        sum += diff * diff;
    }
    return sqrt(sum);
}

void
cumulative_integration(
    double *res,
    double *v,
    double dx,
    unsigned int len
    )
{   
    if (len == 0) return;
    res[0] = 0.0;
    for (unsigned int i = 1; i < len; i++) {
        res[i] = res[i - 1] + 0.5 * (v[i - 1] + v[i]) * dx;
    }
}


void
write_xyz(
    FILE *fp,
    char *symbol,
    double **positions,
    double **velocities,
    double alat,
    int natoms
    )
{
    fprintf(fp, "%i\nLattice=\"%f 0.0 0.0 0.0 %f 0.0 0.0 0.0 %f\" ", natoms, alat, alat, alat);
    fprintf(fp, "Properties=species:S:1:pos:R:3:vel:R:3 pbc=\"T T T\"\n");
    for(int i = 0; i < natoms; ++i){
        fprintf(fp, "%s %lf %lf %lf ",
            symbol, positions[i][0], positions[i][1], positions[i][2]);
        fprintf(fp, "%lf %lf %lf\n",
            velocities[i][0], velocities[i][1], velocities[i][2]);
    }
}

void fft_freq(
    double *res,
    int n,
    double timestep
    )
{
    if (n <= 0 || timestep <= 0.0) return;

    double factor = 2.0 * M_PI / (n * timestep);

    // #pragma omp parallel for
    for (int i = 0; i < n; i++) {
        if (i < (n + 1) / 2) {
            res[i] = i * factor;
        } else {
            res[i] = (i - n) * factor;
        }
    }
}

/* Freely given functions */
void
skip_line(FILE *fp)
{
    int c;
    while (c = fgetc(fp), c != '\n' && c != EOF);
}

void
read_xyz(
    FILE *fp,
    char *symbol,
    double **positions,
    double **velocities,
    double *alat
    )
{   
    // Note: natoms is known at read, velocities/positons space neads to be allocated corectly before calling this function
    int natoms;
    if(fscanf(fp, "%i\nLattice=\"%lf 0.0 0.0 0.0 %lf 0.0 0.0 0.0 %lf\" ", &natoms, alat, alat, alat) != 4){
        fprintf(stderr, "Error reading XYZ header\n");
        return;
    }
    skip_line(fp);
    for(int i = 0; i < natoms; ++i){
        fscanf(fp, "%s %lf %lf %lf ",
            symbol, &positions[i][0], &positions[i][1], &positions[i][2]);
        fscanf(fp, "%lf %lf %lf\n",
            &velocities[i][0], &velocities[i][1], &velocities[i][2]);
    }
}

void powerspectrum(
    double *res,
    double *signal,
    int n,
    double timestep
    )
{
    /* Declaration of variables */
    double *complex_coefficient = malloc(sizeof(double) * 2*n); // array for the complex fft data
    double *data_cp = malloc(sizeof(double) * n);

    /*make copy of data to avoid messing with data in the transform*/
    for (int i = 0; i < n; i++) {
        data_cp[i] = signal[i];
    }

    /* Declare wavetable and workspace for fft */
    gsl_fft_real_wavetable *real;
    gsl_fft_real_workspace *work;

    /* Allocate space for wavetable and workspace for fft */
    work = gsl_fft_real_workspace_alloc(n);
    real = gsl_fft_real_wavetable_alloc(n);

    /* Do the fft*/
    gsl_fft_real_transform(data_cp, 1, n, real, work);

    /* Unpack the output into array with alternating real and imaginary part */
    gsl_fft_halfcomplex_unpack(data_cp, complex_coefficient,1,n);

    /*fill the output powspec_data with the powerspectrum */
    for (int i = 0; i < n; i++) {
        res[i] = (complex_coefficient[2*i]*complex_coefficient[2*i]+complex_coefficient[2*i+1]*complex_coefficient[2*i+1]);
        res[i] *= timestep / n;
    }

    /* Free memory of wavetable and workspace */
    gsl_fft_real_wavetable_free(real);
    gsl_fft_real_workspace_free(work);
    free(complex_coefficient);
    free(data_cp);
}
