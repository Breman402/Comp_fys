#include "tasks.h"

void write_task2_CSV(FILE* file, double time, double E_pot, double E_kin, double temperature) {
  double E_total = E_pot + E_kin;

  fprintf(file, "%.6f,%.10f,%.10f,%.10f,%.4f\n", time, E_pot, E_kin, E_total, temperature);
}

/**
 * Performs one step of the velocity Verlet integration algorithm with periodic boundary conditions.
 * @param positions The array of atom positions.
 * @param velocities The array of atom velocities.
 * @param forces The array of current forces acting on the atoms.
 * @param new_forces The array to store the newly calculated forces after updating positions.
 * @param box_length The length of the simulation box.
 * @param mass The mass of the atoms.
 * @param num_atoms The number of atoms.
 * @param timestep The time step for the integration.
 */
void velocity_verlet_one_step(double** positions, double** velocities, double** forces, double** new_forces, double box_length, double mass, const int num_atoms, double timestep) {
  // 1. Update the positions of ALL atoms
  for (int i = 0; i < num_atoms; i++) {
    for (int j = 0; j < 3; j++) {
      positions[i][j] += velocities[i][j] * timestep + 0.5 * (forces[i][j] / mass) * timestep * timestep;

      // Apply periodic boundary conditions (make sure the positions stay within the box)
      // Not needed here, as periodic boundary conditions are handled in potential.c
      // positions[i][j] -= box_length * floor(positions[i][j] / box_length);
    }
  }

  // 2. Calculate new forces at updated positions
  get_forces_AL(new_forces, positions, box_length, num_atoms);

  // 3. Update velocities
  for (int i = 0; i < num_atoms; i++) {
    for (int j = 0; j < 3; j++) {
      velocities[i][j] += 0.5 * (forces[i][j] + new_forces[i][j]) / mass * timestep;
    }
  }

  // 4. Copy new forces into forces for the next step
  for (int i = 0; i < num_atoms; i++) {
    for (int j = 0; j < 3; j++) {
      forces[i][j] = new_forces[i][j];
    }
  }
}

/*
 * Introduces random deviations to the lattice positions.
 * @param lattice_param The lattice parameter.
 * @param positions The array of atom positions.
 * @param num_atoms The number of atoms.
 * @param deviation_factor The factor by which to scale the deviations.
 */
void random_lattice_deviations(const double lattice_param, double** positions, const int num_atoms, const double deviation_factor) {
  gsl_rng* rng = gsl_rng_alloc(gsl_rng_default);

  const double max_deviation = deviation_factor * lattice_param;

  for (int i = 0; i < num_atoms; i++) {
    for (int j = 0; j < 3; j++) {
      double deviation = (2.0 * gsl_rng_uniform(rng) - 1.0) * max_deviation;

      positions[i][j] += deviation;

      // Periodic boundary conditions are handeled iun potential.c, so we don't need to apply them here.
      // positions[i][j] -= box_length * floor(positions[i][j] / box_length);
    }
  }

  gsl_rng_free(rng);
}

/*
 * Solves the equations of motion using velocity Verlet with periodic boundary conditions.
 * @param positions The array of atom positions.
 * @param box_length The length of the simulation box.
 * @param num_atoms The number of atoms.
 */
void solve_equations_of_motion(double** positions, double box_length, const int num_atoms) {
  // Allocate matrices
  double** velocities = calloc(num_atoms, sizeof(double*));
  double** forces = calloc(num_atoms, sizeof(double*));
  double** new_forces = calloc(num_atoms, sizeof(double*));

  if (velocities == NULL || forces == NULL || new_forces == NULL) {
    fprintf(stderr, "Failed to allocate matrices\n");
    free(velocities);
    free(forces);
    free(new_forces);
    return;
  }

  // Allocate each row
  for (int i = 0; i < num_atoms; i++) {
    velocities[i] = calloc(3, sizeof(double));
    forces[i] = malloc(3 * sizeof(double));
    new_forces[i] = malloc(3 * sizeof(double));

    if (velocities[i] == NULL || forces[i] == NULL || new_forces[i] == NULL) {
      fprintf(stderr, "Memory allocation failed\n");

      for (int j = 0; j <= i; j++) {
        free(velocities[j]);
        free(forces[j]);
        free(new_forces[j]);
      }

      free(velocities);
      free(forces);
      free(new_forces);
      return;
    }
  }

  // Initialize velocities to zero
  for (int i = 0; i < num_atoms; i++) {
    for (int j = 0; j < 3; j++) {
      velocities[i][j] = 0.0;
    }
  }

  // Calculate initial forces
  get_forces_AL(forces, positions, box_length, num_atoms);

  const double timestep = TASK2_TIMESTEP;  // ps (1 fs)
  const int num_steps = TASK2_NUM_STEPS;
  const double mass = TASK2_MASS;  // Aluminium mass in atomic simulation units

  double total_temperature = 0.0;

  FILE* file = fopen("H1_task2_results.csv", "w");
  if (file == NULL) {
    perror("Failed to open task2_results.csv");
    // Free the allocated matrices before returning
    for (int i = 0; i < num_atoms; i++) {
      free(velocities[i]);
      free(forces[i]);
      free(new_forces[i]);
    }
    free(velocities);
    free(forces);
    free(new_forces);
    return;
  }
  // Write CSV header
  fprintf(file, "Time_ps,E_pot,E_kin,E_total,Temperature\n");
  // Record the initial state (time = 0)
  double E_pot_initial = get_energy_AL(positions, box_length, num_atoms);
  write_task2_CSV(file, 0.0, E_pot_initial, 0.0, 0.0);  // Initial kinetic energy and temperature are zero since velocities start at 0

  // Simulation loop
  for (int step = 0; step < num_steps; step++) {
    printf("Task 2 running step %d/%d\n", step + 1, num_steps);
    // Perform one velocity Verlet step
    velocity_verlet_one_step(positions, velocities, forces, new_forces, box_length, mass, num_atoms, timestep);

    // Calculate potential energy
    double E_pot = get_energy_AL(positions, box_length, num_atoms);

    // Calculate kinetic energy
    double E_kin = 0.0;

    for (int i = 0; i < num_atoms; i++) {
      for (int j = 0; j < 3; j++) {
        E_kin += 0.5 * mass * velocities[i][j] * velocities[i][j];
      }
    }

    // Calculate instantaneous temperature
    const double kB = 8.617333262145e-5;  // eV/K
    double temperature = (2.0 * E_kin) / ((3 * num_atoms - 3) * kB);

    total_temperature += temperature;

    write_task2_CSV(file, (step + 1) * timestep, E_pot, E_kin, temperature);
  }

  printf("Average temperature: %.2f K\n", total_temperature / num_steps);

  // Free allocated memory
  for (int i = 0; i < num_atoms; i++) {
    free(velocities[i]);
    free(forces[i]);
    free(new_forces[i]);
  }

  free(velocities);
  free(forces);
  free(new_forces);
  fclose(file);
}

/**
 * Task 2
 * @param lattice_param The lattice parameter.
 * @return 0 on success, 1 on failure.
 */
int task2(double lattice_param) {
  double** pos_ptrs = create_2D_array(N_ATOMS, 3);
  if (pos_ptrs == NULL) {
    fprintf(stderr, "Memory allocation failed for atom positions\n");
    return 1;
  }
  // Initialize the fcc lattice
  init_fcc(pos_ptrs, N_CELLS, lattice_param);

  // Introduce random deviations
  random_lattice_deviations(lattice_param, pos_ptrs, N_ATOMS, TASK2_DEVIATION_FACTOR);

  // Solve the equations of motion using velocity Verlet with periodic boundary conditions
  solve_equations_of_motion(pos_ptrs, N_CELLS * lattice_param, N_ATOMS);

  destroy_2D_array(pos_ptrs);
  return 0;
}
