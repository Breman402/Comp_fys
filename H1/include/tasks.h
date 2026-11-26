#pragma once

#include <gsl/gsl_randist.h>
#include <gsl/gsl_rng.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>

#include "lattice.h"
#include "potential.h"
#include "tools.h"

#define N_CELLS 4                            // Number of unit cells in each direction
#define N_ATOMS (N_CELLS * N_CELLS * N_CELLS * 4)  // Number of atoms in the lattice
#define LATTICE_STEP_SIZE 0.001
#define LATTICE_START 3.9
#define LATTICE_END 4.08

#define TASK2_DEVIATION_FACTOR 0.065        // Maximum deviation factor for random lattice deviations
#define TASK2_TIMESTEP 0.01                // Time step for velocity Verlet integration (started at 0.001 ps, tested 0.01 ps and 0.1 ps)
#define TASK2_TOTAL_TIME 5.0                  // Total simulation time in ps
#define TASK2_NUM_STEPS (int)(TASK2_TOTAL_TIME / TASK2_TIMESTEP)  // Number of steps for velocity Verlet integration
#define TASK2_MASS (26.9815 / 9649.0)       // Mass of each atom (arbitrary units)
#define EQUILIBRIUM_LATTICE_PARAMETER 4.03  // This was obtained from task 1, and is used for task 2


int task1(const double lattice_start, const double lattice_end, const double lattice_step_size);
int task2(const double lattice_param);
int task3(void);


