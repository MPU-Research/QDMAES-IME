#include <stdio.h>
#include <string.h>
#include <math.h>
#include "mpu.h"

void *Reg_Bank[BYTES_PER_REG * NREGS]; 
int Conf_Reg_bank[MPU_CONF_REGS_FIELDS]; 
/*
Conf_Reg_bank:
mconf[15:0]  -> mlmul: Vertical - M idex Grouping   
mconf[31:16] -> nlmul: Horizontal - N idex Grouping
*/

void *reg(int ms) {
    return Reg_Bank + BYTES_PER_REG * ms;
}

void *reg_row_int(int ms, int row) {
    return Reg_Bank + BYTES_PER_REG * ms + row * get_lambda_standard();
}

int get_lambda_standard() {
    int aux = sqrt(INTS_PER_REG);

    if ((aux * aux) == INTS_PER_REG) {
        return aux;
    } else {
        return sqrt(DOUBLE_PER_REG);
    }
}

int get_L_standard() {
    return INTS_PER_REG/get_lambda_standard();
}

void print_reg_as_float(int nreg) {
    printf("Reg %d\n", nreg);
    int lambda = get_lambda_standard();
    int L = get_L_standard();
    for(int i = 0; i < lambda; i++) {
        float *r = (float *) reg_row_int(nreg, i);
        for(int j = 0; j < L; j++) {
            printf("%.4f ", r[j]);
        }
        printf("\n");
    }
    
}

/**************
 * 
 * csrr_vlen: Return the VLEN in bits
 * 
**************/
int csrr_vlen() {
    return VLEN;
}

/***********
 * 
 *  mcpk: Matrix Configuration Panel Kernel
 * 
 *  mlmul: Vertical - M idex Grouping   
 *  nlmul: Horizontal - N idex Grouping
 * 
***********/
void mcpk( int mlmul, int nlmul) {
    Conf_Reg_bank[0] = mlmul;
    Conf_Reg_bank[1] = nlmul;
}

/***********
 * 
 *  mset_ele auxiliar functions
 * 
***********/
int get_mlmul() {
    return Conf_Reg_bank[0];
}

int get_nlmul() {
    return Conf_Reg_bank[1];
}

/***********
 * 
 *  mld_s: Load the matrix data into the matrix register 
 * 
 *  md: Register number for the data
 *  base_address: Base address for the data
 *  row_stride: Row stride
 *
***********/
void mld_s(unsigned int md, void *base_address, unsigned int row_stride) {
    int lambda = get_lambda_standard();
    int L = get_L_standard();

    int elem_width = sizeof(int);

    int mlmul = get_mlmul();
    int nlmul = get_nlmul();

    int *base_address_origin = base_address;

    for (int x = 0; x < mlmul*nlmul; x++) {

        int mlmul_idx = x / nlmul;
        int nlmul_idx = x % nlmul;

        base_address = base_address_origin + (mlmul_idx * row_stride * lambda + nlmul_idx*L);

        for (int i = 0; i < lambda; i++) {
            int *r = (int *) reg_row_int(md+x, i);
            for (int j = 0; j < L; j++) {
                r[j] = *(int *)(base_address + (i * row_stride + j) * elem_width);
            }
        }
    }
}

/***********
 * 
 *  mst_s: Store the matrix data into the matrix register 
 * 
 *  ms: Register number for the data
 *  base_address: Base address for the data
 *  row_stride: Row stride
 *
***********/
void mst_s(unsigned int ms, void *base_address, unsigned int row_stride) {
    int lambda = get_lambda_standard();
    int L = get_L_standard();

    int elem_width = sizeof(int);

    int mlmul = get_mlmul();
    int nlmul = get_nlmul();

    void *base_address_origin = base_address;

    for (int x = 0; x < mlmul*nlmul; x++) {

        int mlmul_idx = x / nlmul;
        int nlmul_idx = x % nlmul;

        base_address = base_address_origin + (mlmul_idx * row_stride * lambda + nlmul_idx*L) * elem_width;
        
        for (int i = 0; i < lambda; i++) {
            int *r = (int *) reg_row_int(ms+x, i);
            for (int j = 0; j < L; j++) {
                *(int *)(base_address + (i * row_stride + j) * elem_width) = r[j];
            }
        }
    }
    
}

/***********
 * 
 *  mzero: Zero the matrix
 * 
 *  md: Register number for the data
 *
***********/
void mzero(unsigned int md) {
    int mlmul = get_mlmul();
    int nlmul = get_nlmul();

    for (int x = 0; x < mlmul*nlmul; x++) {
    
        int *r = (int *) reg(md+x);
        for (int j = 0; j < BYTES_PER_REG; j++) {
            r[j] = 0;
        }
    }
}

/***********
 * 
 *  fmmac_s: Floating point Matrix Multiply and Accumulate Standard
 * 
 *  md: Register number for the data
 *  ms1: Register number for the data
 *  ms2: Register number for the data
 *  tile: Tile index
 * 
***********/
void fmmac_s(unsigned int md, unsigned int ms1, unsigned int ms2, int tile) {
    int lambda = get_lambda_standard();
    int L = get_L_standard();

    int step = (int *) reg_row_int(md, 1) - (int *) reg_row_int(md, 0); 

    int mlmul = get_mlmul();
    int nlmul = get_nlmul();

    for (int x = 0; x < mlmul*nlmul; x++) {

        int mlmul_idx = x / nlmul;
        int nlmul_idx = x % nlmul;

        float *C = (float *) reg(md+x);
        float *A = (float *) reg(ms1+mlmul_idx);
        float *B = (float *) reg(ms2+nlmul_idx);  

        //print_reg_as_float(ms1+mlmul_idx);
        //print_reg_as_float(ms2+nlmul_idx);

        for (int i = 0; i < lambda; i++) {
            for (int j = 0; j < L; j++) {   
                for (int k = 0; k < lambda; k++) {
                    C[i * step + j] += A[i * step + k+(lambda*tile)] * B[k * step + j];
                }
            }
        } 
        //print_reg_as_float(md+x);
    }

}