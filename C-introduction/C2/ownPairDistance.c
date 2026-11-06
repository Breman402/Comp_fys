#include "prototypes.h"


void fillMatrixWithRandomNumbers(struct matrix *m) {
    for (int i = 0; i < m->rows; i++) {
        fillVectorWithRandomNumbers(m->data[i], m->cols);
    }
}

/** Create an n x 3 array of arrays of doubles.
 * @param n: the number of rows in the array of arrays
 * @return: the created array of arrays (matrix struct)
 */
struct matrix createNx3Array(int n) {
    struct matrix m;
    m.rows = n;
    m.cols = 3;
    m.data = (double **)malloc(n * sizeof(double *));
    for (int i = 0; i < n; i++) {
        m.data[i] = (double *)malloc(3 * sizeof(double));
    }
    return m;
}

/** Calculate the distance between two points in a 3D space.
 * @param m: pointer to the matrix containing the points
 * @param i: index of the first point
 * @param j: index of the second point
 * @return: the distance between the two points
 */
double distance(struct matrix *m, int i, int j) {
    double distanceX = m->data[i][0] - m->data[j][0];
    double distanceY = m->data[i][1] - m->data[j][1];
    double distanceZ = m->data[i][2] - m->data[j][2];
    double distance = sqrt(distanceX * distanceX + distanceY * distanceY + distanceZ * distanceZ);
    
    return distance;
}