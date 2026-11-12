#include <math.h>
#include <stdio.h>
#include <stdlib.h>

#include "lattice.h"
#include "potential.h"

#define N 4                            // Number of unit cells in each direction
#define numberOfAtoms (N * N * N * 4)  // Number of atoms in the lattice

int task1(const double lattice_param);
void write_task1_CSV(const double E_pot, const double lattice_param, const char* filename);

int run(int argc, char* argv[]) {
  // Write your code here
  // This makes it possible to test
  // 100% of you code

  // Task 1: -----------------------------------------------------------------------------------------------------------

  // run task1 for diffrent lattice parameters

  #define LATTICE_STEP_SIZE 0.001
  #define LATTICE_START 3.9
  #define LATTICE_END 4.08

  for (double lattice_param = LATTICE_START; lattice_param <= LATTICE_END; lattice_param += LATTICE_STEP_SIZE) {
    if (task1(lattice_param) != 0) {
      fprintf(stderr, "Error occurred while running task1 for lattice parameter %.3f\n", lattice_param);
      return 1;
    }
    else {
      printf("Successfully completed task1 for lattice parameter %.3f\n", lattice_param);
    }
  }
  



  return 0;
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