#include <stdio.h>
#include <stdlib.h>
#include "basic_helpers.h"
#include "mpu.h"
#include "lib.h"

int float_float_example(int M, int N, int K){

    int mlmul = 5;
    int nlmul = 4;

    float **matrixA = allocateMatrix(MAX_DIM, MAX_DIM);
    float **matrixB = allocateMatrix(MAX_DIM, MAX_DIM);
    float **matrixC = allocateMatrix(MAX_DIM, MAX_DIM);
    float **matrixCMMAC = allocateMatrix(MAX_DIM, MAX_DIM);
    fillMatrix_float(M, K, matrixA);
    fillMatrix_float(K, N, matrixB);

    //printMatrix_float("Matrix A:", M, K, matrixA);
    //printMatrix_float("Matrix B:", K, N, matrixB);

    multiplyMatrices_float(M, N, K, matrixC, matrixB, matrixA);
    //printMatrix_float("Golden:", M, N, matrixC);

    sgemm(M, N, K, &matrixCMMAC[0][0], &matrixB[0][0], &matrixA[0][0],
          MAX_DIM, MAX_DIM, MAX_DIM, mlmul, nlmul);
    //printMatrix_float("MMAC:", M, N, matrixCMMAC); 

    printf("Good? %d\n", compareMatrices_float(M, N, matrixC, matrixCMMAC));

    freeMatrix(MAX_DIM, matrixA);
    freeMatrix(MAX_DIM, matrixB);
    freeMatrix(MAX_DIM, matrixC);
    freeMatrix(MAX_DIM, matrixCMMAC);
    return 0;
}

int main() {
    srand(1234); 

    int M = 800;
    int N = 800;
    int K = 800;

    float_float_example(M, N, K);
}
