#include "tasks.h"

int run(int argc, char* argv[]) {
  // Write your code here
  // This makes it possible to test
  // 100% of you code

  // Task 1: -----------------------------------------------------------------------------------------------------------

  // run task1 for diffrent lattice parameters

  for (double lattice_param = LATTICE_START; lattice_param <= LATTICE_END; lattice_param += LATTICE_STEP_SIZE) {
    if (task1(lattice_param) != 0) {
      fprintf(stderr, "Error occurred while running task1 for lattice parameter %.3f\n", lattice_param);
      return 1;
    } else {
      printf("Successfully completed task1 for lattice parameter %.3f\n", lattice_param);
    }
  }

  return 0;
}

int task2(double lattice_param) {
  double** pos_ptrs = (double**)malloc(numberOfAtoms * sizeof(double*));
  if (pos_ptrs == NULL) {
    fprintf(stderr, "Memory allocation failed for pos_ptrs\n");
    return 1;
  }

  // Allocate memory for each atom's position (3 coordinates)
  for (int i = 0; i < numberOfAtoms; i++) {
    pos_ptrs[i] = (double*)malloc(3 * sizeof(double));
    if (pos_ptrs[i] == NULL) {
      fprintf(stderr, "Memory allocation failed for pos_ptrs[%d]\n", i);
      // Free previously allocated memory before returning
      for (int j = 0; j < i; j++) {
        free(pos_ptrs[j]);
      }
      free(pos_ptrs);
      return 1;
    }
  }
  // Initialize the fcc lattice
  init_fcc(pos_ptrs, N, lattice_param);

  // Introduce random deviations
  random_lattice_deviations(lattice_param, pos_ptrs, numberOfAtoms, DEVIATION_FACTOR);

  // Solve the equations of motion using velocity Verlet with periodic boundary conditions
  solve_equations_of_motion(pos_ptrs, N * lattice_param, numberOfAtoms);

  const double E_pot = get_energy_AL(pos_ptrs, N * lattice_param, numberOfAtoms);

  // Write the results to a CSV file
  write_task1_CSV(E_pot / (N * N * N), lattice_param, "task1_results.csv");

  // Free allocated memory
  for (int i = 0; i < numberOfAtoms; i++) {
    free(pos_ptrs[i]);
  }
  free(pos_ptrs);
  return 0;
}

void random_lattice_deviations(const double lattice_param, double** positions, const int num_atoms, const double deviation_factor) {
  gsl_rng* rng = gsl_rng_alloc(gsl_rng_default);

  const double max_deviation = deviation_factor * lattice_param;
  const double box_length = N * lattice_param;

  for (int i = 0; i < num_atoms; i++) {
    for (int j = 0; j < 3; j++) {
      double deviation = (2.0 * gsl_rng_uniform(rng) - 1.0) * max_deviation;

      positions[i][j] += deviation;

      // Periodic boundary conditions
      positions[i][j] -= box_length * floor(positions[i][j] / box_length);
    }
  }

  gsl_rng_free(rng);
}

void solve_equations_of_motion(double** positions, double box_length, const int num_atoms) {
  
  // Alocate velocities, forces and new_forces
  double** velocities = (double**)malloc(num_atoms * sizeof(double*));
  double** forces = (double**)malloc(num_atoms * sizeof(double*));
  double** new_forces = (double**)malloc(num_atoms * sizeof(double*));
  for (int i = 0; i < num_atoms; i++) {
    velocities[i] = (double*)malloc(3 * sizeof(double));
    forces[i] = (double*)malloc(3 * sizeof(double));
    new_forces[i] = (double*)malloc(3 * sizeof(double));
  }

  // Initialize velocities to zero
  for (int i = 0; i < num_atoms; i++) {
    for (int j = 0; j < 3; j++) {
      velocities[i][j] = 0.0;
    }
  }

  // Calculate initial forces
  get_forces_AL(forces, positions, box_length, num_atoms);
  
  return;
}

void velocity_verlet_one_step(double** positions, double** velocities, double** forces, double** new_forces, double box_length, double mass, double timestep) {
  // 1. Update the positions of ALL atoms
  for (int i = 0; i < numberOfAtoms; i++) {
    for (int j = 0; j < 3; j++) {
      positions[i][j] += velocities[i][j] * timestep + 0.5 * (forces[i][j] / mass) * timestep * timestep;

      // Apply periodic boundary conditions (make sure the positions stay within the box)
      positions[i][j] -= box_length * floor(positions[i][j] / box_length);
    }
  }

  // 2. Calculate new forces at updated positions
  get_forces_AL(new_forces, positions, box_length, numberOfAtoms);

  // 3. Update velocities
  for (int i = 0; i < numberOfAtoms; i++) {
    for (int j = 0; j < 3; j++) {
      velocities[i][j] += 0.5 * (forces[i][j] + new_forces[i][j]) / mass * timestep;
    }
  }

  // 4. Copy new forces into forces for the next step
  for (int i = 0; i < numberOfAtoms; i++) {
    for (int j = 0; j < 3; j++) {
      forces[i][j] = new_forces[i][j];
    }
  }
}