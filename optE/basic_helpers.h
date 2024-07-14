#ifndef BASIC_HELPERS_H
#define BASIC_HELPERS_H

#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#define MAX_DIM 1024

float **allocateMatrix(int rows, int columns) {
    float **matrix = (float **)malloc(rows * sizeof(float *));
    if (matrix == NULL) {
        perror("Error allocating memory for rows");
        exit(EXIT_FAILURE);
    }
    for (int i = 0; i < rows; i++) {
        matrix[i] = (float *)malloc(columns * sizeof(float));
        if (matrix[i] == NULL) {
            perror("Error allocating memory for columns");
            exit(EXIT_FAILURE);
        }
    }
    return matrix;
}

void freeMatrix(int rows, float **matrix) {
    for (int i = 0; i < rows; i++) {
        free(matrix[i]);
    }
    free(matrix);
}

void printMatrix_float(char *s, int rows, int columns, float **matrix) {
    printf("%s\n", s);
    for (int i = 0; i < rows; i++) {
        for (int j = 0; j < columns; j++) {
            printf("%.4f ", matrix[i][j]);
        }
        printf("\n");
    }
}

void fillMatrix_float(int rows, int columns, float **matrix) {
    for (int i = 0; i < rows; i++) {
        for (int j = 0; j < columns; j++) {
            matrix[i][j] = (float)rand() / (float)(RAND_MAX / 10);
        }
    }
}


void multiplyMatrices_float(int M, int N, int K, float **C, float **B, float **A) {

    for (int i = 0; i < M; i++) {
        for (int j = 0; j < N; j++) {
            C[i][j] = 0.0;
            for (int k = 0; k < K; k++) {
                C[i][j] += A[i][k] * B[k][j];
            }
        }
    }
    return;
}

int compareMatrices_float(int rows, int columns, float **matrix1, float **matrix2) {
    for (int i = 0; i < rows; i++) {
        for (int j = 0; j < columns; j++) {
            if (matrix1[i][j] != matrix2[i][j]) {
                return 0; // Matrices are not equal
            }
        }
    }
    return 1; // Matrices are equal
}



#endif