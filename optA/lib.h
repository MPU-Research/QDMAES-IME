#ifndef MPU_LIB_H
#define MPU_LIB_H

#include "basic_helpers.h"
#include "mpu.h"

#include <math.h>

/************************************************************
 * 
 *  sgemm
 * 
 *  This function is a high-level model of the MPU accelerator. It receives 
 *  the configuration of the accelerator, the dimensions of the matrices, 
 *  the matrices themselves, and the leghts of the lines of the matrices.
 * 
 *  The function is specialized for single precision floating point matrices, 
 *  where the matrices are divided in groupings of size mlmul x nlmul, and 
 *  the multiplication is done in a grouping.
 * 
 ************************************************************/
void sgemm(int M, int N, int K, float *C, float *B, float *A, int ldc, int ldb, int lda, int mlmul, int nlmul)
{
    float *C_ptr = C;
    float *B_ptr = B;
    float *A_ptr = A;

    int lambda = get_lambda_standard();

    printf("VLEN: %d\n", csrr_vlen());
    printf("Register size: %d x %d\n", lambda, lambda);
    printf("mlmul: %d nlmul: %d\n", mlmul, nlmul);

    int total_registers = mlmul*nlmul + mlmul + nlmul;
    printf("Total registers: %d\n", total_registers);

    if (total_registers > NREGS) {
        printf("Error: Not enough registers\n");
        return;
    }
    
    printf("Compute intensity: %.02f\n", (float)(lambda*mlmul*nlmul)/(mlmul+nlmul));

    int N_fmmac = 0;
    for (int i = 0; i < M; i+=lambda*mlmul) {
        for (int j = 0; j < N; j+=lambda*nlmul) {

            // Load C
            mcpk(mlmul, nlmul);
            mzero(vr0);

            for (int k = 0; k < K; k+=lambda) {
               
                // Load A
                mcpk(mlmul, 1);
                A_ptr = A + k + i*lda;
                mld_s(vr20, A_ptr, lda);

                // Load B
                mcpk(1, nlmul);
                B_ptr = B + j + k*ldb;
                mld_s(vr25, B_ptr, ldb);

                // Execute the matrix multiplication and accumulate instruction
                mcpk(mlmul, nlmul);
                fmmac_s(vr0, vr20, vr25);
                N_fmmac +=1;
            }

            // Store C
            C_ptr = C + j + i*ldc;
            mst_s(vr0, C_ptr, ldc);
        }
    }
    printf("Total fmmac_s: %d\n", N_fmmac);
    printf("Total MACs: %d\n", N_fmmac*lambda*lambda*lambda*mlmul*nlmul);
}

#endif