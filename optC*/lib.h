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
 *  The function is specialized for floating matrices, where the matrices are 
 *  divided in groupings of size mlmul x nlmul, and the multiplication is done in a grouping.
 * 
 ************************************************************/
void sgemm(int M, int N, int K, float *C, float *B, float *A, int ldc, int ldb, int lda, int mlmul, int nlmul)
{
    float *C_ptr = C;
    float *B_ptr = B;
    float *A_ptr = A;

    int elements_per_row = get_lambda_standard();
    int elements_per_column = get_L_standard() ;

    int L = elements_per_column/elements_per_row;

    printf("VLEN: %d\n", csrr_vlen());
    printf("⟨λ, L⟩: %d, %d\n", elements_per_row, L);
    printf("mlmul: %d nlmul: %d\n", mlmul, nlmul);

    int total_registers = mlmul*nlmul + mlmul + nlmul;
    printf("Total registers: %d\n", total_registers);

    if (total_registers > NREGS) {
        printf("Error: Not enough registers\n");
        return;
    }

    printf("Compute intensity: %.02f\n", (float)(mlmul*nlmul*elements_per_column)/(L*nlmul + mlmul));

    int N_fmmac = 0;
    for (int i = 0; i < M; i+=elements_per_row * mlmul) {
        for (int j = 0; j < N; j+=elements_per_column * nlmul) {
            
            // Reset the accumulator
            mcpk(mlmul, nlmul);
            mzero(vr0);

            for (int k = 0; k < K; k+=elements_per_column) {
                A_ptr = A + k + i*lda;

                // Load A - mload⟨fp32, 5, ml, 1, kl⟩(vr20, A(i, k), lda)
                mcpk(mlmul, 1);
                mld_s(vr20, A_ptr, lda);

                for (int x = 0; x < L; x++) {
                    B_ptr = B + j + (k+x*elements_per_row)*ldb;

                    // Load B - mload⟨fp32, 1, kl, 5, nl⟩(v26, B(k, j), ldb)
                    mcpk(1, nlmul);
                    mld_s(vr26, B_ptr, ldb);

                    // Execute the matrix multiplication and accumulate instruction
                    // mgemmx⟨fp32, ×, +⟩(v00, v20, v26, x)
                    mcpk(mlmul, nlmul);
                    fmmac_s(vr0, vr20, vr26, x);
                    N_fmmac +=1;
                }
            }
            C_ptr = C + j + i*ldc;

            // Store C
            mst_s(vr0, C_ptr, ldc);
        }
    }
    printf("Total fmmac_s: %d\n", N_fmmac);
    printf("Total MACs: %d\n", N_fmmac*elements_per_column*elements_per_row*elements_per_row*mlmul*nlmul);
}

#endif