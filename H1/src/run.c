#include "tasks.h"

int run(int argc, char* argv[]) {
  if (task1(LATTICE_START, LATTICE_END, LATTICE_STEP_SIZE) != 0) {
    fprintf(stderr, "Error occurred while running task1\n");
    return 1;
  } else {
    fprintf(stdout, "Task1 completed successfully\n");
  }

  fprintf(stdout, "Starting task2 with equilibrium lattice parameter %.3f\n", EQUILIBRIUM_LATTICE_PARAMETER);
  if (task2(EQUILIBRIUM_LATTICE_PARAMETER) != 0) {
    fprintf(stderr, "Error occurred while running task2\n");
    return 1;
  }

  fprintf(stdout, "Starting task3\n");
  if (task3() != 0) {
    fprintf(stderr, "Error occurred while running task3\n");
    return 1;
  }

  return 0;
}
