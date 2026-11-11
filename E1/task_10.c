
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
#include <stdio.h>

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
#define time_step 0.1
#define total_time 1000000.0 // 25000.0 for the OG, then 1000000.0 for task 10
#define sampling_interval 1000
#define a 0.1 // 0.0 for harmonic, 0.01 or 0.1 for anharmonic
#define E0 Np

double positions[Np] = {0.0};     // Initialize positions to zero
double Q[Np] = {0.0};             // Initialize Q-coordinates to zero
double energies[Np] = {0.0};      // Initialize energies to zero
double velocities[Np] = {0.0};    // Initialize velocities to zero
double accelerations[Np] = {0.0}; // Initialize accelerations to zero


void transform_to_normal_modes(double *positions, double *Q, const unsigned int N);
void calculate_normal_mode_energies(double *energies, double *positions, double *velocities, const unsigned int N);
void calculate_acceleration(double *accelerations, double *positions, double alpha, const unsigned int N);
void velocity_verlet_one_step(double *accelerations, double *positions, double *velocities, double alpha, double timestep, const unsigned int N);

int main(void) {

    const unsigned int steps = (unsigned int)(total_time / time_step);

    // Set the initial normal mode momenta
    double P[Np] = {0.0};
    P[0] = sqrt(2.0 * E0);

    // Transform modal momenta to particle velocities
    transform_to_normal_modes(P, velocities, Np);

    // Calculate initial accelerations
    calculate_acceleration(accelerations, positions, a, Np);

    // Arrays for calculating cumulative time averages
    double cumulative_energies[Np] = {0.0};
    unsigned int sample_count = 0;

    // Open CSV file
    FILE *file = fopen("time_averages.csv", "w");

    if (file == NULL) {
        fprintf(stderr, "Error opening file\n");
        return EXIT_FAILURE;
    }

    // Write CSV header
    fprintf(file, "time");

    for (unsigned int i = 0; i < Np; i++) {
        fprintf(file, ",E%u", i + 1);
    }

    fprintf(file, "\n");

    // Main simulation loop
    for (unsigned int step = 1; step <= steps; step++) {

        // Advance the simulation one timestep
        velocity_verlet_one_step(accelerations, positions, velocities, a, time_step, Np);

        // Calculate energies only every 1000 timesteps
        if (step % sampling_interval == 0) {

            double elapsed_time = step * time_step;

            calculate_normal_mode_energies(energies, positions, velocities, Np);

            sample_count++;

            // Write time
            fprintf(file, "%.10f", elapsed_time);

            // Calculate cumulative time averages
            for (unsigned int i = 0; i < Np; i++) {

                cumulative_energies[i] += energies[i];

                double average = cumulative_energies[i] / sample_count;

                fprintf(file, ",%.15g", average);
            }

            fprintf(file, "\n");
        }
    }

    fclose(file);

    return EXIT_SUCCESS;
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
 * Calculate the acceleration.
 *
 *
 * Parameters
 * ----------
 *  accelerations - Vector where accelerations are to be written
 *  positions - Vector of positions
 *  alpha - Anharmonicity constant
 *  N - Number of atoms
 *
*/
void calculate_acceleration(double *accelerations, double *positions, double alpha, const unsigned int N)
{
    for (unsigned int i = 0; i < N; i++) {
        // Calculate the acceleration for atom i using the FPUT equations of motion
        // The acceleration is given by the second derivative of the potential energy with respect to position
        // For the FPUT problem, this involves both linear and nonlinear terms
        // Here, we assume a simple form

        // Fixed boundary conditions
        double left = (i == 0) ? 0.0 : positions[i-1];
        double right = (i == N-1) ? 0.0 : positions[i+1];

        // Harmonic contribution
        double harmonic = right - 2*positions[i] + left;

        // Anharmonic contribution
        double anharmonic = alpha * (
            (right - positions[i]) * (right - positions[i])
            - (positions[i] - left) * (positions[i] - left)
        );

        accelerations[i] = harmonic + anharmonic;
    }
}

/**
 * Perform one velocity Verlet step
 *
 * Parameters
 * ----------
 *  accelerations - Vector of accelerations
 *  positions - Vector of positions
 *  velocities - Vector of velocities
 *  alpha - Anharmonicity constant
 *  timestep - Time step
 *  N - Number of atoms
 *
*/
void velocity_verlet_one_step(double *accelerations, double *positions, double *velocities, double alpha, double timestep, const unsigned int N)
{
    // Write to accelerations, positions and velocities vectors
    for (unsigned int i = 0; i < N; i++) {
        // Update positions using current velocities and accelerations
        positions[i] += velocities[i] * timestep + 0.5 * accelerations[i] * timestep * timestep;
    }
    for (unsigned int i = 0; i < N; i++) {
        // Update velocities using current accelerations
        velocities[i] += 0.5 * accelerations[i] * timestep;
    }
    // Calculate the new accelerations
    calculate_acceleration(accelerations, positions, alpha, N);
    for (unsigned int i = 0; i < N; i++) {
        // Update velocities using new accelerations
        velocities[i] += 0.5 * accelerations[i] * timestep;
    }
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
