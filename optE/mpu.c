#include <stdio.h>
#include <string.h>
#include <math.h>
#include "mpu.h"

void *Reg_Bank[BYTES_PER_REG * NREGS]; 
int Conf_Reg_bank[MPU_CONF_REGS_FIELDS]; 
/*
Conf_Reg_bank:
mconf[4:0] -> mlmul: Vertical - M idex Grouping   
mconf[9:5] -> nlmul: Horizontal - N idex Grouping
mconf[10]  -> shape: Shape of the matrix  (0: horizontal, 1: vertical)
*/

void *reg(int ms) {
    return Reg_Bank + BYTES_PER_REG * ms;
}

void *reg_row_int(int ms, int row, int shape) {
    if (shape == 0) {
        return Reg_Bank + BYTES_PER_REG * ms + row * get_p_standard();
    } else {
        return Reg_Bank + BYTES_PER_REG * ms + row * get_l_min_standard();
    }
}

int get_l_min_standard() {
    return 2;
}

int get_p_standard() {
    return INTS_PER_REG/get_l_min_standard();
}

void print_reg_as_float(int nreg, int shape) {
    printf("Reg %d\n", nreg);
    int l_min = get_l_min_standard();
    int p = get_p_standard();
    if (shape == 0) {
        for(int i = 0; i < l_min; i++) {
            float *r = (float *) reg_row_int(nreg, i,shape);
            for(int j = 0; j < p; j++) {
                printf("%.4f ", r[j]);
            }
            printf("\n");
        }
    } else {
        for(int i = 0; i < p; i++) {
            float *r = (float *) reg_row_int(nreg, i,shape);
            for(int j = 0; j < l_min; j++) {
                printf("%.4f ", r[j]);
            }
            printf("\n");
        }
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
 *  shape: Shape of the matrix  (0: horizontal, 1: vertical)
 * 
***********/
void mcpk( int mlmul, int nlmul, int shape) {
    Conf_Reg_bank[0] = mlmul;
    Conf_Reg_bank[1] = nlmul;
    Conf_Reg_bank[2] = shape;
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

int get_shape() {
    return Conf_Reg_bank[2];
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
    int l_min = get_l_min_standard();
    int p = get_p_standard();
    int shape = get_shape();

    int elem_width = sizeof(int);

    int mlmul = get_mlmul();
    int nlmul = get_nlmul();

    void *base_address_origin = base_address;

    for (int x = 0; x < mlmul*nlmul; x++) {

        int mlmul_idx = x / nlmul;
        int nlmul_idx = x % nlmul;

        if (shape == 0) {
            base_address = base_address_origin + (mlmul_idx * row_stride * l_min + nlmul_idx*p) * elem_width;
            for (int i = 0; i < l_min; i++) {
                int *r = (int *) reg_row_int(md+x, i,shape);
                for (int j = 0; j < p; j++) {
                    r[j] = *(int *)(base_address + (i * row_stride + j) * elem_width);
                }
            }
        } else{
            base_address = base_address_origin + (mlmul_idx * row_stride * p + nlmul_idx*l_min) * elem_width;
            for (int i = 0; i < p; i++) {
                int *r = (int *) reg_row_int(md+x, i,shape);
                for (int j = 0; j < l_min; j++) {
                    r[j] = *(int *)(base_address + (i * row_stride + j) * elem_width);
                }
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
    int l_min = get_l_min_standard();
    int p = get_p_standard();
    int shape = get_shape();

    int elem_width = sizeof(int);

    int mlmul = get_mlmul();
    int nlmul = get_nlmul();

    void *base_address_origin = base_address;

    for (int x = 0; x < mlmul*nlmul; x++) {

        int mlmul_idx = x % mlmul;
        int nlmul_idx = x / mlmul;

        if (shape == 0) {
            base_address = base_address_origin + (mlmul_idx * row_stride * l_min + nlmul_idx*p) * elem_width;
            for (int i = 0; i < l_min; i++) {
                int *r = (int *) reg_row_int(ms+x, i,shape);
                for (int j = 0; j < p; j++) {
                    *(int *)(base_address + (i * row_stride + j) * elem_width) = r[j];
                }
            }
        } else{
            base_address = base_address_origin + (mlmul_idx * row_stride * p + nlmul_idx*l_min) * elem_width;
            for (int i = 0; i < p; i++) {
                int *r = (int *) reg_row_int(ms+x, i,shape);
                for (int j = 0; j < l_min; j++) {
                    *(int *)(base_address + (i * row_stride + j) * elem_width) = r[j];
                }
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
 *  SEGS: Index of the segment
 * 
***********/
void fmmac_s(unsigned int md, unsigned int ms1, unsigned int ms2, int SEGS) {
    int l_min = get_l_min_standard();
    int p = get_p_standard();

    int N_SEGS = p/l_min;

    int step_h = (int *) reg_row_int(md, 1, horizontal) - (int *) reg_row_int(md, 0, horizontal); 
    int step_v = (int *) reg_row_int(md, 1, vertical) - (int *) reg_row_int(md, 0, vertical); 

    int mlmul = get_mlmul();
    int nlmul = get_nlmul();

    for (int x = 0; x < mlmul*nlmul; x++) {

        int nlmul_idx = x / mlmul;
        int mlmul_idx = x % mlmul;

        float *C = (float *) reg(md+N_SEGS*x+SEGS);
        float *A = (float *) reg(ms1+mlmul_idx);
        float *B = (float *) reg(ms2+nlmul_idx); 

        //print_reg_as_float(ms1+mlmul_idx, vertical);
        //print_reg_as_float(ms2+nlmul_idx, horizontal);

        for (int i = 0; i < l_min; i++) {
            for (int j = 0; j < p; j++) {   
                for (int k = 0; k < l_min; k++) {
                    C[i * step_h + j] += A[(i+ (l_min*SEGS)) * step_v + k] * B[k * step_h + j];
                }
            }
        } 
        //print_reg_as_float(md+N_SEGS*x+SEGS, horizontal);
    }
}