
/*
 * Implement the functions in this file.
 * Utilize the fact that the transformation can be written as
 * a matrix multiplication. To this end, you can use the following
 * functions from C4. No need to declare them, Yata already knows about them
 *

double **create_2D_array(unsigned int n, unsigned int m);

void destroy_2D_array(double **array, unsigned int n);

void matrix_vector_multiplication(double *result, double **A, double *b, unsigned int n, unsigned int m);

 **/

// Placeholder functions:
#include <stdlib.h>
#include <math.h>
void destroy_2D_array(double **array, unsigned int n) {
    for (unsigned int i = 0; i < n; i++) {
        free(array[i]);
    }
    free(array);
}

double **create_2D_array(unsigned int n, unsigned int m) {
    double **array = (double **)malloc(n * sizeof(double *));
    for (unsigned int i = 0; i < n; i++) {
        array[i] = (double *)malloc(m * sizeof(double));
    }
    return array;
}

void matrix_vector_multiplication(double *result, double **A, double *b, unsigned int n, unsigned int m) {
    for (unsigned int i = 0; i < n; i++) {
        result[i] = 0.0;
        for (unsigned int j = 0; j < m; j++) {
            result[i] += A[i][j] * b[j];
        }
    }
}

#define Np 32 // Number of particles
double positions[Np];
double Q[Np];
double energies[Np];
double velocities[Np];
const double time_step = 0.1;
const double total_time = 25000.0;
const double E0 = Np;
void transform_to_normal_modes(double *positions, double *Q, const unsigned int N);
void calculate_normal_mode_energies(double *energies, double *positions, double *velocities, const unsigned int N);

int main() {

    calculate_normal_mode_energies(energies, positions, velocities, Np);
    return 0;
}

/**
 * Transform to normal modes.
 *
 * Parameters
 * ----------
 *  positions - Vector of positions
 *  Q - Vector of Q-coordinates
 *  N - Number of particles (length of vectors)
 *
*/
void transform_to_normal_modes(double *positions, double *Q, const unsigned int N)
{
    // This function should perform the transformation from the positions of the particles to the normal mode coordinates (Q-coordinates).
    // The transformation can be represented as a matrix multiplication, where the transformation matrix is based on the normal mode eigenvectors.
    // Write to Q
    
    // Create the transformation matrix for the normal modes of a 3-particle system
    double **transformation_matrix = create_2D_array(N, N);

    // Populate it acordingly
    #pragma omp parallel for collapse(2) // collapse 2 makes the two loops into one, allowing for better parallelization
    for (unsigned int i = 0; i < N; i++) {
        for (unsigned int j = 0; j < N; j++) {
            double ikpi = (double)((i + 1) * (j + 1)) * M_PI;
            transformation_matrix[i][j] = sqrt(2.0 / (N + 1)) * sin(ikpi/(N + 1));
        }
    }

    matrix_vector_multiplication(Q, transformation_matrix, positions, N, N);
    destroy_2D_array(transformation_matrix, N);
}



/**
 * Calculate the normal mode energies
 *
 * Parameters
 * ----------
 *  energies - Vector where energies will be written
 *  positions - Vector of positions
 *  velocities - Vector of positions
 *  N - Number of particles (length of vectors)
 *
*/
void calculate_normal_mode_energies(double *energies, double *positions, double *velocities, const unsigned int N)
{

    double P[N];
    double Q[N];

    // Transform postions and velocities to normal mode coordinates
    transform_to_normal_modes(positions, Q, N);
    transform_to_normal_modes(velocities, P, N);

    // Write to energies
    for (int i = 0; i < N; i++) {
        // Angular frequency of mode i
        double w = 2.0 * sin((double)(i + 1) * M_PI / (2.0 * (N + 1)));

        // Energy of mode i
        energies[i] = 0.5 * (P[i] * P[i] + w * w * Q[i] * Q[i]);

    }

}
