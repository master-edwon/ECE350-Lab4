#ifndef MATRIXMUL_H
#define MATRIXMUL_H

#include "ap_int.h"

#define MAT_A_ROWS  4
#define MAT_A_COLS  4
#define MAT_B_ROWS  MAT_A_COLS
#define MAT_B_COLS  4

typedef ap_int<16> mat_t;     // 16-bit signed integer for inputs/outputs
typedef ap_int<34> acc_t;     // 34-bit accumulator (16+16=32, +2 bits for 4 sums)

void matrixmul(
    mat_t a[MAT_A_ROWS][MAT_A_COLS],
    mat_t b[MAT_B_ROWS][MAT_B_COLS],
    mat_t res[MAT_A_ROWS][MAT_B_COLS]
);

#endif // MATRIXMUL_H
