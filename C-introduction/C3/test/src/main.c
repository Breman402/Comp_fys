#include <stdio.h>  // printf
#include <stdlib.h>  // malloc
#include "vector.h"


void
print_vector(
        double *vec,  // Vector to print
        unsigned int ndims  // Number of dimensions
        )
{
    for (int i = 0; i < ndims; i++) {
        printf("%10.5f ", vec[i]);
    }
}

void
solve_general(
        double *pos,  // Position vector. Initialize with initial position
        double *vel,  // Velocity vector. Initialize with initial velocity
        double *gravity,  // Vector with the opposite direction of gravity
        unsigned int ndims  // Number of dimensions of vectors
        )
{
    // Constants
    const double dt = 0.01f;  // Simulation time step
    const double alpha = 1.0f;  // Proportional to drag coefficient
    const double beta = 1.0f;  // Proportional to mass

    double *tmp = (double *)malloc(sizeof(double)*ndims);  // Buffer for storing intermediate work
    double time = 0.0f;  // Simulation time

    // Print header
    printf("# Column 0: Time \n");
    printf("# Columns %i-%i: Position \n", 1, ndims);
    printf("# Columns %i-%i: Velocity \n", ndims + 1, ndims * 2);

    // Print positions and velocity
    print_vector(&time, 1);
    print_vector(pos, ndims);
    print_vector(vel, ndims);
    printf("\n");

    while(1) {
        // Update position by adding velocity times dt
        constant_multiplication(tmp, vel, dt, ndims);  // tmp <- v*dt
        elementwise_addition(pos, pos, tmp, ndims);    // pos <- pos + v*dt

        // Compute acceleration from drag, multiply with dt and add to velocity
        constant_multiplication(tmp, vel, -alpha, ndims);
        constant_multiplication(tmp, tmp, dt, ndims);
        elementwise_addition(vel, vel, tmp, ndims);

        // Compute acceleration from gravity, multiply with dt and add to velocity
        constant_multiplication(tmp, gravity, -beta, ndims);
        constant_multiplication(tmp, tmp, dt, ndims);
        elementwise_addition(vel, vel, tmp, ndims);

        // Update time
        time += dt;

        // Print positions and velocity
        print_vector(&time, 1);
        print_vector(pos, ndims);
        print_vector(vel, ndims);
        printf("\n");

        // Break the simulation when below 0 in the dimension of gravity
        if (dot_product(pos, gravity, ndims) < 0) {
            break;
        }
    }

    free(tmp);
    tmp = NULL;
}

void
solve_3d() {
    double pos[] = {0.f, 0.f, 0.f};  // Position
    double vel[] = {1.f, 1.f, 0.3f};  // Velocity
    double gravity[] = {0.f, 1.f, 0.f};  // Gravity is directed in negative y

    solve_general(pos, vel, gravity, 3);
}

int
main()
{
    solve_3d();

    return 0;
}

