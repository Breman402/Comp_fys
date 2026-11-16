#pragma once

#include <gsl/gsl_rng.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>

#include "lattice.h"
#include "potential.h"

#define N 4                            // Number of unit cells in each direction
#define numberOfAtoms (N * N * N * 4)  // Number of atoms in the lattice
#define LATTICE_STEP_SIZE 0.001
#define LATTICE_START 3.9
#define LATTICE_END 4.08

#define DEVIATION_FACTOR 0.065  // Maximum deviation factor for random lattice deviations

int task1(const double lattice_param);
void write_task1_CSV(const double E_pot, const double lattice_param, const char* filename);
