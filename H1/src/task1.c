#include "tasks.h"

void write_task1_CSV(const double E_pot, const double lattice_param, const char* filename) {
  static int first_call = 1;

  FILE* file = fopen(filename, first_call ? "w" : "a");
  if (file == NULL) {
    fprintf(stderr, "Error opening file %s for writing\n", filename);
    return;
  }

  // Write header only on the first call
  if (first_call) {
    fprintf(file, "E_pot,Lattice_Param\n");
    first_call = 0;
  }

  fprintf(file, "%.8f,%.8f\n", E_pot, lattice_param);

  fclose(file);
}

int task1(const double lattice_param) {
  // Side length of one FCC unit cell in (Å)

  // Matrix in which atomic positions are stored
  // Allocate memory for the positions of the atoms
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