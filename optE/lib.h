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
    int Consumed_M, Consumed_N, Consumed_K;

    float *C_ptr = C;
    float *B_ptr = B;
    float *A_ptr = A;

    float *C_ptr_AUX, *B_ptr_AUX, *A_ptr_AUX;

    int elements_per_column = get_p_standard();
    int elements_per_row = get_l_min_standard();

    int SEGS = elements_per_column/elements_per_row;

    printf("VLEN: %d\n", csrr_vlen());
    printf("Register size (A): %d x %d\n", elements_per_column, elements_per_row);
    printf("SEGS: %d\n", SEGS);
    printf("mlmul: %d nlmul: %d\n", mlmul, nlmul);

    int total_registers = SEGS*mlmul*nlmul + mlmul + nlmul;
    printf("Total registers: %d\n", total_registers);

    if (total_registers > NREGS) {
        printf("Error: Not enough registers\n");
        return;
    }

    printf("Compute intensity: %.02f\n", (float)(mlmul*nlmul*elements_per_column)/(nlmul + mlmul));

    int N_fmmac = 0;
    for (int i = 0; i < M; i+=elements_per_column*mlmul) {
        for (int j = 0; j < N; j+=elements_per_column*nlmul) {

            // Load C
            mcpk(SEGS*mlmul, nlmul, horizontal);
            mzero(vr0);

            for (int k = 0; k < K; k+=elements_per_row) {
               
                // Load A
                mcpk(mlmul, 1, vertical);
                A_ptr = A + k + i*lda;
                mld_s(vr24, A_ptr, lda);

                // Load B
                mcpk(1, nlmul, horizontal);
                B_ptr = B + j + k*ldb;
                mld_s(vr28, B_ptr, ldb);

                for (int x = 0; x < SEGS; x++) {
                    // Execute the matrix multiplication and accumulate instruction
                    mcpk(mlmul, nlmul, horizontal);
                    fmmac_s(vr0, vr24, vr28, x);
                    N_fmmac++;
                }
            }

            // Store C
            mcpk(SEGS*mlmul, nlmul, horizontal);
            C_ptr = C + j + i*ldc;
            mst_s(vr0, C_ptr, ldc);
        }
    }
    printf("Total fmmac_s: %d\n", N_fmmac);
    printf("Total MACs: %d\n", N_fmmac*elements_per_column*elements_per_row*elements_per_row*mlmul*nlmul);
}

#endif