#ifndef MPU_H
#define MPU_H

#define VLEN                    1024
#define NREGS                   32
#define MPU_CONF_REGS_FIELDS    3

/***************
 * 
 * MAX NUMBER OF ELEMENTS PER REGISTER
 *   
***************/
#define DOUBLE_PER_REG   VLEN/64
#define INTS_PER_REG     VLEN/32
#define SHORTS_PER_REG   VLEN/16
#define BYTES_PER_REG    VLEN/8
#define NIBBLE_PER_REG   VLEN/4

/***************
 * 
 * Instructions simulation
 * 
***************/
int csrr_vlen();

int get_l_min_standard();
int get_p_standard();

void mcpk(int mlmul, int nlmul, int shape);
void mzero(unsigned int rd);

void mld_s(unsigned int rd, void *base_address, unsigned int row_stride);
void mst_s(unsigned int rd, void *base_address, unsigned int row_stride);

void fmmac_s(unsigned int rd, unsigned int rs1, unsigned int rs2, int x);


/************
 * 
 * Register definition
 * 
************/
#define vr0 0
#define vr1 1
#define vr2 2
#define vr3 3
#define vr4 4
#define vr5 5
#define vr6 6
#define vr7 7
#define vr8 8
#define vr9 9
#define vr10 10
#define vr11 11
#define vr12 12
#define vr13 13
#define vr14 14
#define vr15 15
#define vr16 16
#define vr17 17
#define vr18 18
#define vr19 19
#define vr20 20
#define vr21 21
#define vr22 22
#define vr23 23
#define vr24 24
#define vr25 25
#define vr26 26
#define vr27 27
#define vr28 28
#define vr29 29
#define vr30 30
#define vr31 31


/************
 * 
 * Shape definition
 *
************/
#define horizontal 0
#define vertical 1

#endif