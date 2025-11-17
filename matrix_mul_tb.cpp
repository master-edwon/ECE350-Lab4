    #include <cstdio>
#include <cstdlib>
#include <ctime>
#include <cmath>
#include "matrix_mul.h"

void matrixmul(
    mat_t a[MAT_A_ROWS][MAT_A_COLS],
    mat_t b[MAT_B_ROWS][MAT_B_COLS],
    mat_t  res[MAT_A_ROWS][MAT_B_COLS]
);

// ---------- Reference model (software) ----------
static void matmul_ref(
    mat_t a[MAT_A_ROWS][MAT_A_COLS],
    mat_t b[MAT_B_ROWS][MAT_B_COLS],
    mat_t r[MAT_A_ROWS][MAT_B_COLS]
) {
    for (int i = 0; i < MAT_A_ROWS; ++i) {
        for (int j = 0; j < MAT_B_COLS; ++j) {
            long long acc = 0; // wider accumulator to match DUT
            for (int k = 0; k < MAT_A_COLS; ++k) { // == MAT_B_ROWS
                acc += a[i][k] * b[k][j];
            }
            r[i][j] = (mat_t)acc;
        }
    }
}

// ---------- Helpers ----------
static void fill_sequential(mat_t *ptr, int n, int start=1) {
    for (int i = 0; i < n; ++i) ptr[i] = (mat_t)(start + i);
}

static void fill_random(mat_t *ptr, int n, int lo=-3, int hi=3) {
    for (int i = 0; i < n; ++i) {
        int v = lo + (std::rand() % (hi - lo + 1));
        ptr[i] = (mat_t)v;
    }
}

static bool compare_mats(
    const mat_t *A, const mat_t *B, int rows, int cols, bool verbose=false
) {
    bool ok = true;
    for (int i = 0; i < rows; ++i) {
        for (int j = 0; j < cols; ++j) {
            mat_t a = A[i*cols + j];
            mat_t b = B[i*cols + j];
            if (a != b) {
                ok = false;
                if (verbose) {
                    std::printf("Mismatch at (%d,%d): got=%d exp=%d\n",
                                i, j, (int)a, (int)b);
                }
            }
        }
    }
    return ok;
}

static void print_mat(const char* name, const mat_t* M, int r, int c) {
    std::printf("%s =\n", name);
    for (int i = 0; i < r; ++i) {
        for (int j = 0; j < c; ++j) {
            std::printf("%6d ", (int)M[i*c + j]);
        }
        std::printf("\n");
    }
}

// ---------- Testbench ----------
int main() {
    std::srand(0xC0FFEE); // deterministic

    // Allocate matrices
    static mat_t A[MAT_A_ROWS][MAT_A_COLS];
    static mat_t B[MAT_B_ROWS][MAT_B_COLS];
    static mat_t R_dut[MAT_A_ROWS][MAT_B_COLS];
    static mat_t R_ref[MAT_A_ROWS][MAT_B_COLS];

    int pass_cnt = 0;
    int test_cnt = 0;

    // --- Test 1: Sequential small integers (good for first sanity check)
    {
        ++test_cnt;
        fill_sequential(&A[0][0], MAT_A_ROWS*MAT_A_COLS, 1);
        fill_sequential(&B[0][0], MAT_B_ROWS*MAT_B_COLS, 1);

        matrixmul(A, B, R_dut);
        matmul_ref(A, B, R_ref);

        bool ok = compare_mats(&R_dut[0][0], &R_ref[0][0], MAT_A_ROWS, MAT_B_COLS, true);
        std::printf("[Test %d] Sequential data: %s\n", test_cnt, ok ? "PASS" : "FAIL");
        pass_cnt += ok;
    }

    
    
    // --- Tests 4..N: Randomized sweeps
    const int RANDOM_ITERS = 10;
    for (int t = 0; t < RANDOM_ITERS; ++t) {
        ++test_cnt;
        fill_random(&A[0][0], MAT_A_ROWS*MAT_A_COLS, -8, 8);
        fill_random(&B[0][0], MAT_B_ROWS*MAT_B_COLS, -8, 8);

        matrixmul(A, B, R_dut);
        matmul_ref(A, B, R_ref);

        bool ok = compare_mats(&R_dut[0][0], &R_ref[0][0], MAT_A_ROWS, MAT_B_COLS, false);
        std::printf("[Test %d] Random %d: %s\n", test_cnt, t+1, ok ? "PASS" : "FAIL");
        if (!ok) {
            // Optional debug dump on first failure
            print_mat("A", &A[0][0], MAT_A_ROWS, MAT_A_COLS);
            print_mat("B", &B[0][0], MAT_B_ROWS, MAT_B_COLS);
            print_mat("R_dut", &R_dut[0][0], MAT_A_ROWS, MAT_B_COLS);
            print_mat("R_ref", &R_ref[0][0], MAT_A_ROWS, MAT_B_COLS);
        }
        pass_cnt += ok;
    }

    std::printf("\nSummary: %d/%d tests passed.\n", pass_cnt, test_cnt);
    return (pass_cnt == test_cnt) ? 0 : 1;
}
