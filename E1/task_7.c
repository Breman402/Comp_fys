
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
#define total_time 25000.0 // 25000.0 for the OG, then 1000000.0 for task 10
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

struct energy_record {
    double time;
    double energies[Np];
};

int main() {

    const unsigned int steps = (unsigned int)(total_time / time_step);

    struct energy_record *energy_records = malloc((steps + 1) * sizeof(struct energy_record)); // This one is actually too big for the stack, so we allocate it on the heap

    if (energy_records == NULL) {
        fprintf(stderr, "Memory allocation failed\n");
        return EXIT_FAILURE;
    }

    // Set the inital momenta
    double P[Np] = {0.0};
    P[0] = sqrt(2.0 * E0); // Set the initial momentum of the first particle

    // Transform modal momenta to particle velocities
    transform_to_normal_modes(P, velocities, Np);

    // Calculate initial accelerations
    calculate_acceleration(accelerations, positions, a, Np);

    for (unsigned int step = 0; step <= steps; step++) {

        double elapsed_time = step * time_step;

        // Calculate the normal mode energies
        calculate_normal_mode_energies(energies, positions, velocities, Np);

        // Store the energies in the energy_records array
        energy_records[step].time = elapsed_time;
        for (unsigned int i = 0; i < Np; i++) {
            energy_records[step].energies[i] = energies[i];
        }

        // Advance the simulation
        if (step < steps) { // No need to advance on the last step
            velocity_verlet_one_step(accelerations, positions, velocities, a, time_step, Np);
        }
    }

    // Write the energy records to a CSV file
    FILE *file = fopen("energy_records.csv", "w");
    if (file == NULL) {
        fprintf(stderr, "Error opening file\n");
        return 1;
    }

    // Write CSV header
    fprintf(file, "time");
    for (unsigned int j = 0; j < Np; j++) {
        fprintf(file, ",E%u", j + 1);
    }
    fprintf(file, "\n");

    // Write simulation data
    for (unsigned int i = 0; i <= steps; i++) {
        fprintf(file, "%.10f", energy_records[i].time);

        for (unsigned int j = 0; j < Np; j++) {
            fprintf(file, ",%.15g", energy_records[i].energies[j]);
        }
        fprintf(file, "\n");
    }

    fclose(file);

    free(energy_records);

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
