#include "tasks.h"

#define eV 1.602176634e-19
#define angstrom 1e-10

#define AUfactor 9649.0
#define kB 8.617333262e-5
#define AlMass (26.9815 / AUfactor)

#define BAR_TO_INTERNAL (1e5 * angstrom * angstrom * angstrom / eV)
#define GPA_TO_INTERNAL (1e9 * angstrom * angstrom * angstrom / eV)

#define BULK_MODULUS (76.0 * GPA_TO_INTERNAL)
#define COMPRESSIBILITY (1.0 / BULK_MODULUS)

#define T_EQ 773.15
#define P_EQ (1.0 * BAR_TO_INTERNAL)

#define timeStep 0.001 // ps

#define TAU_T (500.0 * timeStep)
#define TAU_P (500.0 * timeStep)

#define EQUILIBRATION_TIME 15.0 // ps
#define CONSTANT_TIME 20.0   // ps

#define EQUILIBRATION_STEPS ((unsigned int)(EQUILIBRATION_TIME / timeStep))

#define CONSTANT_STEPS ((unsigned int)(CONSTANT_TIME / timeStep))

static double calculate_kinetic_energy(double **velocities)
{
    return AlMass / 2 * dot_product(velocities[0], velocities[0], N_ATOMS * 3);
}

static double calculate_temperature(double kinetic_energy)
{
    return 2.0 * kinetic_energy / (3.0 * N_ATOMS * kB);
}

static double calculate_pressure(
    double kinetic_energy, 
    double virial, 
    double cell_length
)
{
    double volume = cell_length * cell_length * cell_length;

    return (2.0 * kinetic_energy / 3.0 + virial) / volume;
}

static double calculate_alpha_T(
    double temperature, 
    double target_temperature, 
    double timestep, 
    double tau_T
)
{
    return 1.0 + (2.0 * timestep / tau_T) * (target_temperature - temperature) / temperature;
}

static double calculate_alpha_P(
    double pressure, 
    double target_pressure, 
    double timestep, 
    double tau_P
)
{
    return 1.0 - COMPRESSIBILITY * (timestep / tau_P) * (target_pressure - pressure);
}

static void scale_system(
    double **positions, 
    double **velocities, 
    double *cell_length, 
    double alpha_T, 
    double alpha_P
)
{
    double position_scale = cbrt(alpha_P);
    double velocity_scale = sqrt(alpha_T) * position_scale;

    multiplication_with_constant(positions[0], positions[0], position_scale, N_ATOMS * 3);

    multiplication_with_constant(velocities[0], velocities[0], velocity_scale, N_ATOMS * 3);

    *cell_length *= position_scale;
}

static void velocity_verlet_one_step(
    double **forces, 
    double **positions, 
    double **velocities, 
    double *potential_energy, 
    double *virial, 
    double cell_length, 
    double timestep
)
{   
    double half_step = timestep / (2.0 * AlMass);

    for (int i = 0; i < N_ATOMS; i++) {
        for (int d = 0; d < 3; d++) {
            velocities[i][d] += forces[i][d] * half_step;
            positions[i][d] += velocities[i][d] * timestep;
        }
    }

    calculate(potential_energy, virial, forces, positions, cell_length, N_ATOMS);

    for (int i = 0; i < N_ATOMS; i++) {
        for (int d = 0; d < 3; d++) {
            velocities[i][d] += forces[i][d] * half_step;
        }
    }
    
}

static int run_simulation(
    FILE *output_file,
    double **positions,
    double **velocities,
    double **forces,
    double *cell_length,
    double timestep,
    unsigned int steps,
    double start_time,
    int thermostat,
    int barostat
)
{
    double potential_energy;
    double virial;

    // Calculate initial forces
    calculate(&potential_energy, &virial, forces, positions, *cell_length, N_ATOMS);

    for (unsigned int step = 0; step < steps; step++) {
        printf("Task 3 running step %d/%d\n", step + 1, steps);

        velocity_verlet_one_step(
            forces,
            positions,
            velocities,
            &potential_energy,
            &virial,
            *cell_length,
            timestep
        );

        double kinetic_energy = calculate_kinetic_energy(velocities);

        double total_energy = potential_energy + kinetic_energy;

        double temperature = calculate_temperature(kinetic_energy);

        double pressure = calculate_pressure(kinetic_energy, virial, *cell_length);

        double lattice_parameter = *cell_length / N_CELLS;

        double time = start_time + (step + 1) * timestep;

        // Save physical quantities
        fprintf(
            output_file,
            "%.17g,%.17g,%.17g,%.17g,"
            "%.17g,%.17g,%.17g,%.17g",
            time,
            potential_energy,
            kinetic_energy,
            total_energy,
            temperature,
            pressure / BAR_TO_INTERNAL,
            *cell_length,
            lattice_parameter
        );

        // Save positions of the first three atoms
        for (int i = 0; i < 3; i++) {
            for (int d = 0; d < 3; d++) {
                fprintf(output_file, ",%.17g", positions[i][d]);
            }
        }

        fprintf(output_file, "\n");

        // No scaling during NVE simulation
        if (!thermostat && !barostat) {
            continue;
        }

        double alpha_T = 1.0;
        double alpha_P = 1.0;

        if (thermostat) {
            if (!isfinite(temperature) || temperature <= 0.0) {
                fprintf(stderr, "Invalid temperature\n");
                return 1;
            }

            alpha_T = calculate_alpha_T(
                temperature,
                T_EQ,
                timestep,
                TAU_T
            );
        }

        if (barostat) {
            alpha_P = calculate_alpha_P(
                pressure,
                P_EQ,
                timestep,
                TAU_P
            );
        }

        if (!isfinite(alpha_T) || alpha_T <= 0.0 ||
            !isfinite(alpha_P) || alpha_P <= 0.0) {
            fprintf(stderr, "Invalid scaling factors\n");
            return 1;
        }

        scale_system(
            positions,
            velocities,
            cell_length,
            alpha_T,
            alpha_P
        );

        // Recalculate forces if the positions have changed
        if (barostat && alpha_P != 1.0) {
            calculate(
                &potential_energy,
                &virial,
                forces,
                positions,
                *cell_length,
                N_ATOMS
            );
        }
    }

    return 0;
}

static void write_csv_header(FILE *output_file)
{
    fprintf(
        output_file,
        "time,"
        "potential_energy,"
        "kinetic_energy,"
        "total_energy,"
        "temperature,"
        "pressure_bar,"
        "cell_length,"
        "lattice_parameter"
    );

    for (int i = 0; i < 3; i++) {
        fprintf(
            output_file,
            ",x_%d,y_%d,z_%d",
            i, i, i
        );
    }

    fprintf(output_file, "\n");
}

static void initialize_system(
    double **positions,
    double **velocities,
    double lattice_parameter
)
{
    init_fcc(positions, N_CELLS, lattice_parameter);

    double max_displacement = 0.065 * lattice_parameter;

    for (int i = 0; i < N_ATOMS; i++) {
        for (int d = 0; d < 3; d++) {
            double random = (double)rand() / RAND_MAX;

            positions[i][d] +=
                (2.0 * random - 1.0) * max_displacement;

            velocities[i][d] = 0.0;
        }
    }
}

int task3(void)
{
    int status = 1;

    double cell_length = N_CELLS * EQUILIBRIUM_LATTICE_PARAMETER;

    double **positions = create_2D_array(N_ATOMS, 3);
    double **velocities = create_2D_array(N_ATOMS, 3);
    double **forces = create_2D_array(N_ATOMS, 3);

    FILE *equilibration_file = NULL;
    FILE *constant_file = NULL;

    if (!positions || !velocities || !forces) {
        fprintf(stderr, "Memory allocation failed\n");
        goto cleanup;
    }

    // Reproducible random displacements
    srand(42);

    initialize_system(
        positions,
        velocities,
        EQUILIBRIUM_LATTICE_PARAMETER
    );

    /*
     * EQUILIBRATION
     */

    equilibration_file = fopen("H1_task3_equ.csv", "w");

    if (equilibration_file == NULL) {
        goto cleanup;
    }

    write_csv_header(equilibration_file);

    if (run_simulation(
        equilibration_file,
        positions,
        velocities,
        forces,
        &cell_length,
        timeStep,
        EQUILIBRATION_STEPS,
        0.0,
        1, // Thermostat enabled
        1  // Barostat enabled
    ) != 0) {
        goto cleanup;
    }

    printf(
        "Equilibrated lattice parameter: %.8f Å\n",
        cell_length / N_CELLS
    );

    /*
     * CONSTANT
     */

    constant_file = fopen("H1_task3_const.csv", "w");

    if (constant_file == NULL) {
        goto cleanup;
    }

    write_csv_header(constant_file);

    if (run_simulation(
        constant_file,
        positions,
        velocities,
        forces,
        &cell_length,
        timeStep,
        CONSTANT_STEPS,
        EQUILIBRATION_STEPS * timeStep,
        0, // Thermostat disabled
        0  // Barostat disabled
    ) != 0) {
        goto cleanup;
    }

    status = 0;

cleanup:

    if (equilibration_file != NULL) {
        fclose(equilibration_file);
    }

    if (constant_file != NULL) {
        fclose(constant_file);
    }

    destroy_2D_array(positions);
    destroy_2D_array(velocities);
    destroy_2D_array(forces);

    return status;
}
