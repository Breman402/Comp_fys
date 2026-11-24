#include "tasks.h"

int run(int argc, char* argv[]) {
  // Write your code here
  // This makes it possible to test
  // 100% of you code

  for (double lattice_param = LATTICE_START; lattice_param <= LATTICE_END; lattice_param += LATTICE_STEP_SIZE) {
    if (task1(lattice_param) != 0) {
      fprintf(stderr, "Error occurred while running task1 for lattice parameter %.3f\n", lattice_param);
      return 1;
    } else {
      printf("Successfully completed task1 for lattice parameter %.3f\n", lattice_param);
    }
  }

  fprintf(stdout, "Starting task2 with equilibrium lattice parameter %.3f\n", EQUILIBRIUM_LATTICE_PARAMETER);
  if (task2(EQUILIBRIUM_LATTICE_PARAMETER) != 0) {
    fprintf(stderr, "Error occurred while running task2\n");
    return 1;
  }

  return 0;
}
