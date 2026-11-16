#include <stdio.h>
#include <stdlib.h>
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

    for(int i = 0; i < len; i++){
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

    for(int i = 0; i < len; i++){
        res[i] = v1[i] * v2[i];
    }

}

void
addition_with_constant(
                       double *res,
                       double *v,
                       double constant,
                       unsigned int len)
{

    for(int i = 0; i < len; i++){
        res[i] = v[i] + constant;
    }

}

void
multiplication_with_constant(
                             double *res,
                             double *v,
                             double constant,
                             unsigned int len)
{

    for(int i = 0; i < len; i++){
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

    for(int i = 0; i < len; i++){
        result += v1[i] * v2[i];
    }

    return result;
}

double **
create_2D_array(
    unsigned int row_size,
    unsigned int column_size
)
{
    if (row_size == 0 || column_size == 0)
        return NULL;

    double **array = malloc(row_size * sizeof(double *));
    if (array == NULL)
        return NULL;

    // Allocate all elements in one contiguous block
    double *data = calloc(
        (size_t)row_size * column_size,
        sizeof(double)
    );
    if (data == NULL) {
        free(array);
        return NULL;
    }

    // Point each row to its position in the block
    for (unsigned int i = 0; i < row_size; i++) {
        array[i] = data + i * column_size;
    }

    return array;
}

void 
destroy_2D_array(double **array, unsigned int n)
{
    (void)n;

    if (array == NULL)
        return;

    free(array[0]);
    free(array);
}

void
matrix_vector_multiplication(
                             double *result,
                             double **A,
                             double *b,
                             unsigned int n,
                             unsigned int m
                            )
{

    for (unsigned int i = 0; i < n; i++) {
        result[i] = dot_product(A[i], b, m);
    }
    
}

void
matrix_matrix_multiplication(
                             double **result,
                             double **A,
                             double **B,
                             unsigned int n,
                             unsigned int m,
                             unsigned int k
                            )
{

    for (unsigned int i = 0; i < n; i++) {
        for (unsigned int j = 0; j < k; j++) {
            result[i][j] = 0.0;

            for (unsigned int l = 0; l < m; l++) {
                result[i][j] += A[i][l] * B[l][j];
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

    double result = 0.0;

    for(int i = 0; i < len; i++){
        result += v1[i] * v1[i];
    }

    return sqrt(result);
}


void
normalize_vector(
                 double *v1,
                 unsigned int len
                )
{

    double norm = vector_norm(v1, len);

    for(int i = 0; i < len; i++){
        v1[i] /= norm;
    }

}

double
average(
        double *v1,
        unsigned int len
       )
{
    double sum = 0.0;
    for(int i = 0; i < len; i++){
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
    double mean = average(v1, len);
    double sum = 0.0;
    for(int i = 0; i < len; i++){
        sum += (v1[i] - mean) * (v1[i] - mean);
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
    for(int i = 0; i < len; i++){
        sum += (v1[i] - v2[i]) * (v1[i] - v2[i]);
    }
    
    return sqrt(sum);
}

void
cumulative_integration(
                       double *res,
                       double *v,
                       double dx,
                       unsigned int v_len
                      )
{
    
    res[0] = 0.0;

    for (unsigned int i = 1; i < v_len; i++) {
        res[i] = res[i-1] + (v[i-1] + v[i]) * dx / 2.0;
    }

}


void
write_xyz(
          FILE *fp,
          char *symbol,
          double **positions,
          double **velocities,
          double alat,
          int natoms)
{
    fprintf(fp, "%i\nLattice=\"%f 0.0 0.0 0.0 %f 0.0 0.0 0.0 %f\" ", natoms, alat, alat, alat);
    fprintf(fp, "Properties=species:S:1:pos:R:3:vel:R:3 pbc=\"T T T\"\n");
    for(int i = 0; i < natoms; ++i){
        fprintf(fp, "%s %f %f %f %f %f %f\n", symbol, positions[i][0], positions[i][1], positions[i][2], velocities[i][0], velocities[i][1], velocities[i][2]);

    }
}

void fft_freq(
          double *res,
              int n,
              double timestep)
{

    for (int k = 0; k < n; k++) {
        
        // Ternary operator: if k < (n + 1) / 2, assign k; otherwise, assign k - n.
        int freq_k = (k < (n + 1) / 2) ? k : k - n;
        
        
        res[k] = 2 * M_PI * freq_k / (n * timestep);
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
         double *alat)
{
    int natoms;
    if(fscanf(fp, "%i\nLattice=\"%lf 0.0 0.0 0.0 %lf 0.0 0.0 0.0 %lf\" ", &natoms, alat, alat, alat) == 0){
        perror("Error");
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
                   double timestep)
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
