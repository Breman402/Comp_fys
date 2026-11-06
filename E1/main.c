#include <stdio.h>
#include <math.h>

#define eV 1.60218e-19 // J
#define å 1e-10 // m
#define conversionFactorAtomicUnits 9649

#define springConst (1600/(eV/(å*å))) // 1.6kN/m
#define carbonMass 12.011/conversionFactorAtomicUnits
#define oxygenMass 15.9994/conversionFactorAtomicUnits

// This is just cause I want main to be up top
void calculate_acceleration(double *accelerations, double *positions, double *masses, double kappa);
void velocity_verlet_one_step(double *accelerations, double *positions, double *velocities, double *masses, double kappa, double timestep);
double calculate_potential_energy(double *positions, double kappa);
double calculate_kinetic_energy(double *velocities, double *masses);
void write_simulation_data_to_csv(const char *filename1, const char *filename2);
void compute_power_spectrum();

// Globals
// Each particle has a position, velocity and acceleration
// Each vector contains the values for the three particles in the CO_2 molecule
double accelerations[3] = {};
double velocities[3]; // in Å/fs
double positions[3] = {0.01, 0.005, -0.005}; // in Å
double masses[3] = {oxygenMass, carbonMass, oxygenMass};

// Arrays for storing each atoms simulations data

#define timeStep 0.000001 // fs default: 0.00025 also tested: 0.000001 and 0.001
#define totalTime 1.0 // fs default: 0.25, for the FFT results to be good this value had to be increased to 1.0!
#define sampleSize ((const unsigned int)(totalTime / timeStep)) // Number of samples to take

struct SimulationData {
    double time;
    double potentialEnergy;
    double kineticEnergy;
    double totalEnergy;
} simulationData;

struct AtomData {
    double time;
    double displacement;
} atomData;

struct PowerSpectrumData {
    double frequency;
    double power;
} powerSpectrumData;

static struct SimulationData simulationDataArray[sampleSize];
static struct AtomData atomDataArray[sampleSize][3]; // 3 atoms

static struct PowerSpectrumData powerSpectrumArray[sampleSize];

void main() {
    // 1)
    printf("Carbon mass: %f\n", carbonMass);
    printf("Oxygen mass: %f\n", oxygenMass);
    printf("Spring constant: %f\n", springConst);

    // 2) and 3)
    // velocity verlet algorithm for CO_2 in one dimension
    
    // Calculate initial accelerations
    calculate_acceleration(accelerations, positions, masses, springConst);
    
    // Simulate
    for (unsigned int i = 0; i < sampleSize; i++) {
        // Perform one velocity verlet step
        velocity_verlet_one_step(accelerations, positions, velocities, masses, springConst, timeStep);
        // Store the simulation data for each atom
        simulationDataArray[i].time = i * timeStep;
        simulationDataArray[i].potentialEnergy = calculate_potential_energy(positions, springConst);
        simulationDataArray[i].kineticEnergy = calculate_kinetic_energy(velocities, masses);
        simulationDataArray[i].totalEnergy = simulationDataArray[i].potentialEnergy + simulationDataArray[i].kineticEnergy;
        // Store the data for each atom
        for (unsigned int j = 0; j < 3; j++) {
            atomDataArray[i][j].time = i * timeStep;
            atomDataArray[i][j].displacement = positions[j];
        }
    }

    // Compute the power spectrum of the CO_2 molecule using Discrete Fourier transform
    compute_power_spectrum();

    // Write the simulation data to a csv file
    write_simulation_data_to_csv("simulation_data.csv", "atom_data.csv");
}

/**
 * Write the simulation data to a csv file
 *
 * Parameters
 * ----------
 *  filename1 - The name of the first file to write simulation data to
 *  filename2 - The name of the second file to write atom data to
 */
void write_simulation_data_to_csv(const char *filename1, const char *filename2) {
    FILE *file1 = fopen(filename1, "w");
    FILE *file2 = fopen(filename2, "w");
    
    // Make sure the files were opened successfully
    if (file1 == NULL || file2 == NULL) {
        printf("Error opening file for writing: %s or %s\n", filename1, filename2);
        return;
    }

    // Empty the files before writing to them
    freopen(filename1, "w", file1);
    freopen(filename2, "w", file2);


    for (unsigned int i = 0; i < sampleSize; i++) {
        fprintf(file1, "%.15g,%.15g,%.15g,%.15g\n", simulationDataArray[i].time, simulationDataArray[i].potentialEnergy, simulationDataArray[i].kineticEnergy, simulationDataArray[i].totalEnergy);
    }

    fclose(file1);

    for (unsigned int i = 0; i < sampleSize; i++) {
        for (unsigned int j = 0; j < 3; j++) {
            fprintf(file2, "%.15g,%.15g\n", atomDataArray[i][j].time, atomDataArray[i][j].displacement);
        }
    }

    fclose(file2);
}

/**
 * Compute the power spectrum of the CO_2 molecule using Discrete Fourier transform.
 * This is probably best done in Python, since the plots are made there later. I just
 * want to try it here for fun.
 * 
 * In the end this did not end up working well at all due to being too slow
*/
void compute_power_spectrum() {

    const unsigned int N = sampleSize;
    const double PI = acos(-1.0);

    // Calculate mean displacement for each atom
    double meanDisplacement[3] = {0.0, 0.0, 0.0};

    for (unsigned int j = 0; j < 3; j++) {
        for (unsigned int i = 0; i < N; i++) {
            meanDisplacement[j] += atomDataArray[i][j].displacement;
        }
        meanDisplacement[j] /= N;
    }

    // Calculate the positive-frequency half of the DFT
    for (unsigned int k = 0; k <= N/2; k++) {

        double totalPower = 0.0;

        for (unsigned int j = 0; j < 3; j++) {

            double realPart = 0.0;
            double imagPart = 0.0;

            for (unsigned int n = 0; n < N; n++) {
                double angle = 2 * PI * k * n / N;

                realPart += (atomDataArray[n][j].displacement
                            - meanDisplacement[j]) * cos(angle);

                imagPart -= (atomDataArray[n][j].displacement
                            - meanDisplacement[j]) * sin(angle);
            }

            // Calculate power for this atom
            totalPower += realPart * realPart + imagPart * imagPart;
        }

        powerSpectrumArray[k].frequency = k / (N * timeStep);
        powerSpectrumArray[k].power = totalPower / ((double)N * N);
    }
}


/**
 * Calculate the acceleration.
 *
 * All vectors are assumed to be of length 3
 *
 * Parameters
 * ----------
 *  accelerations - Vector where accelerations are to be written
 *  positions - Vector of positions
 *  masses - Vector of masses
 *  kappa - Spring constant
 *
*/
void calculate_acceleration(double *accelerations, double *positions, double *masses, double kappa)
{
    const unsigned int N = 3;  // There are three particles

    for (unsigned int i = 0; i < N; i++) {
        // The force on particle i is the sum of the forces from its neighbors
        // Those forces are given by Hooke's law: F = -k * (q_i - q_j)
        
        double force = 0.0;

        // Force from the particle on the left
        if (i > 0) {
            force += kappa * (positions[i-1] - positions[i]);
        }

        // Force from the particle on the right
        if (i < N-1) {
            force += kappa * (positions[i+1] - positions[i]);
        }

        accelerations[i] = force / masses[i];
    }
}

/**
 * Calculate the potential energy
 *
 * All vectors are assumed to be of length 3
 *
 * Parameters
 * ----------
 *  positions - Vector of positions
 *  kappa - Spring constant
 * Returns
 * -------
 *  Potential energy
 *
*/
double calculate_potential_energy(double *positions, double kappa)
{
    const unsigned int N = 3;  // There are three particles

    double potentialEnergy = 0.0;

    for (unsigned int i = 0; i < N-1; i++) {
        // The potential energy of a spring is given by the formula: PE = 0.5 * k * x^2, where k is the spring constant and x is the displacement from equilibrium.
        
        double displacement = positions[i+1] - positions[i];

        potentialEnergy += 0.5 * kappa * displacement * displacement;
    }

    return potentialEnergy;
}

/**
 * Calculate the kinetic energy
 *
 * All vectors are assumed to be of length 3
 *
 * Parameters
 * ----------
 *  velocities - Vector of velocities
 *  masses - Vector of masses
 *
 * Returns
 * -------
 *  Kinetic energy
 *
*/
double calculate_kinetic_energy(double *velocities, double *masses)
{
    const unsigned int N = 3;  // There are three particles

    double kineticEnergy = 0.0;

    for (unsigned int i = 0; i < N; i++) {
        // Calculate kinetic energy for particle i
        // The kinetic energy of a particle is given by the formula: KE = 0.5 * m * v^2, where m is the mass and v is the velocity.
        kineticEnergy += 0.5 * masses[i] * velocities[i] * velocities[i];
    }

    return kineticEnergy;
}

/**
 * Perform one velocity Verlet step
 *
 * All vectors are assumed to be of length 3
 *
 * Parameters
 * ----------
 *  accelerations - Vector of accelerations
 *  positions - Vector of positions
 *  velocities - Vector of velocities
 *  masses - Vector of masses
 *  kappa - Spring constant
 *  timestep - Time step
 *
*/
void velocity_verlet_one_step(double *accelerations, double *positions,
                              double *velocities, double *masses,
                              double kappa, double timestep)
{
    const unsigned int N = 3;
    double oldAccelerations[3];

    for (unsigned int i = 0; i < N; i++) {
        oldAccelerations[i] = accelerations[i];

        positions[i] += velocities[i] * timestep + 0.5 * accelerations[i] * timestep * timestep;
    }

    // Calculate accelerations at new positions
    calculate_acceleration(accelerations, positions, masses, kappa);

    // Update ALL velocities
    for (unsigned int i = 0; i < N; i++) {
        velocities[i] += 0.5 * (oldAccelerations[i] + accelerations[i]) * timestep;
    }
}