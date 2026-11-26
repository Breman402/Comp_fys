#include "tasks.h"

void write_task1_header(FILE* file) {
  fprintf(file, "Lattice_Param,E_pot_per_unit_cell\n");
}

void write_task1_CSV(FILE* file, const double E_pot, const double lattice_param) {
  fprintf(file, "%.8f,%.8f\n", lattice_param, E_pot);
}

int task1(const double lattice_start, const double lattice_end, const double lattice_step_size) {
  FILE* file = fopen("task1_results.csv", "w");
  if (file == NULL) {
    fprintf(stderr, "Error opening task1_results.csv for writing\n");
    return 1;
  }

  write_task1_header(file);

  double** pos_ptrs = create_2D_array(N_ATOMS, 3);
  if (pos_ptrs == NULL) {
    fprintf(stderr, "Memory allocation failed for atom positions\n");
    fclose(file);
    return 1;
  }

  const int num_steps = (int)((lattice_end - lattice_start) / lattice_step_size + 0.5);

  for (int i = 0; i <= num_steps; i++) {
    const double lattice_param = lattice_start + i * lattice_step_size;
    init_fcc(pos_ptrs, N_CELLS, lattice_param);

    const double E_pot = get_energy_AL(pos_ptrs, N_CELLS * lattice_param, N_ATOMS);

    write_task1_CSV(file, E_pot / (N_CELLS * N_CELLS * N_CELLS), lattice_param);
  }

  destroy_2D_array(pos_ptrs);
  fclose(file);
  return 0;
}
