#include "matrix_mul.h"

// Top function
void matrixmul(
    mat_t a[MAT_A_ROWS][MAT_A_COLS],
    mat_t b[MAT_B_ROWS][MAT_B_COLS],
    mat_t res[MAT_A_ROWS][MAT_B_COLS]
) {

    // Partition 'a' rows to allow reading an
    //    entire row in a single clock cycle.
    #pragma HLS ARRAY_PARTITION variable=a complete dim=2

    // Partition 'b' columns to allow reading an
    //    entire column in a single clock cycle.
    #pragma HLS ARRAY_PARTITION variable=b complete dim=1


ROW:
    for (int i = 0; i < MAT_A_ROWS; i++) {
    COL:
        for (int j = 0; j < MAT_B_COLS; j++) {
            #pragma HLS PIPELINE II=1
            acc_t acc = 0;

        PROD:
            for (int k = 0; k < MAT_B_ROWS; k++) {
                #pragma HLS UNROLL
                acc += a[i][k] * b[k][j];
            }

            res[i][j] = (mat_t)acc; // Cast 34-bit acc back to 16-bit
        }
    }
}
