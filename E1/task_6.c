
/**
 * Implement a function for calculating the acceleration of each atom in the FPUT problem, and implement a function that takes one step of the velocity Verlet algorithm for this system. Since we can set the mass and kappa to 1 (defining a new system of units) we do not need to use them in the code.
 *
 * Note that we assume that the vectors have length N
 * N, i.e., positions[0]
 * positions[0] is equivalent to u1
 * u1
 * ​ in the description of E1.
 *
 * Press check to verify that your code is correct. You can check your code even before you are finished, and the tests should tell you which functions are incomplete or wrong.
 */


double accelerations[3] = {0.0, 0.0, 0.0};
double positions[3] = {0.0, 0.0, 0.0};
double velocities[3] = {0.0, 0.0, 0.0};
const double alpha = 1.0; // Anharmonicity constant
const double timestep = 0.01; // Time step
const unsigned int N = 3; // Number of atoms

void calculate_acceleration(double *accelerations, double *positions, double alpha, const unsigned int N);
void velocity_verlet_one_step(double *accelerations, double *positions, double *velocities, double alpha, double timestep, const unsigned int N);

int main() {

    calculate_acceleration(accelerations, positions, alpha, N);
    velocity_verlet_one_step(accelerations, positions, velocities, alpha, timestep, N);
    return 0;
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
