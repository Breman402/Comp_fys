#include "prototypes.h"

int main (int argc, char *argv[]) {
    // Read length of vectors from command line
    int n = vectorLengthFromCommandLine();

    // Create two vectors of the read length
    double *x = (double *)malloc(n * sizeof(double));
    double *y = (double *)malloc(n * sizeof(double));

    // Fill the vectors with random numbers
    fillVectorWithRandomNumbers(x, n);
    fillVectorWithRandomNumbers(y, n);

    // Compute the scalar product
    double result = computeScalarProduct(x, y, n);

    // Print the result
    printf("The scalar product is: %f\n", result);

    // Store one of the vectors in a csv file
    FILE *file = fopen("vector.csv", "w");
    if (file != NULL) {
        for (int i = 0; i < n; i++) {
            fprintf(file, "%f\n", x[i]);
        }
        fclose(file);
    } else {
        printf("Error opening file for writing.\n");
    }

    // Free allocated memory
    free(x);
    free(y);

    // Create an n x 3 array of arrays (matrix)
    struct matrix m = createNx3Array(n);

    // Fill the matrix with random numbers
    fillMatrixWithRandomNumbers(&m);

    // Calculate the distance between the first two points in the matrix
    double dist = distance(&m, 0, 1);
    printf("The distance between the first two points is: %f\n", dist);

    // Print the matrix to a csv file
    FILE *file2 = fopen("matrix.csv", "w");
    if (file2 != NULL) {
        for (int i = 0; i < m.rows; i++) {
            for (int j = 0; j < m.cols; j++) {
                fprintf(file2, "%f", m.data[i][j]);
                if (j < m.cols - 1) {
                    fprintf(file2, ",");
                }
            }
            fprintf(file2, "\n");
        }
        fclose(file2);
    } else {
        printf("Error opening file for writing.\n");
    }

    return 0;
}
