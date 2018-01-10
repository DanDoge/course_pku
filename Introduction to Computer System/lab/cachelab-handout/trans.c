/*
 * trans.c - Matrix transpose B = A^T
 *
 * Each transpose function must have a prototype of the form:
 * void trans(int M, int N, int A[N][M], int B[M][N]);
 *
 * A transpose function is evaluated by counting the number of misses
 * on a 1KB direct mapped cache with a block size of 32 bytes.
 *
 * Name      : 
 * NickName  :
 * userID    :
 * reference : "http://csapp.cs.cmu.edu/public/waside/waside-blocking.pdf"
 *             "http://www.cs.cmu.edu/afs/cs/academic/class/15213-s17
 *              /www/recitations/recitation07-cachelab.pdf"
 */
#include <stdio.h>
#include "cachelab.h"
#include "contracts.h"

int is_transpose(int M, int N, int A[N][M], int B[M][N]);

/*
 * transpose_submit - This is the solution transpose function that you
 *     will be graded on for Part B of the assignment. Do not change
 *     the description string "Transpose submission", as the driver
 *     searches for that string to identify the transpose function to
 *     be graded. The REQUIRES and ENSURES from 15-122 are included
 *     for your convenience. They can be removed if you like.
 */
char transpose_submit_desc[] = "Transpose submission";
void transpose_submit(int M, int N, int A[N][M], int B[M][N])
{
    int i = 0, j = 0, k = 0, l = 0, tmp, save;

    REQUIRES(M > 0);
    REQUIRES(N > 0);

    /* for 32 * 32 matrix, choose blocksize = 8
     * date : 11/11   misses : 287
     */
    if(M == 32 && N == 32){
        for(i = 0 ; i < N; i += 8){
            for(j = 0 ; j < M; j += 8){
                for(k = 0; k < 8 && (i + k) < N; k += 1){
                    for(l = 0; l < 8 && (j + l) < M; l += 1){
                        tmp = A[i + k][j + l];
                        if(i + k != j + l){
                            B[j + l][i + k] = tmp;
                        }else{
                            save = tmp;
                        }
                    }
                    /* elements on diagnal of A and B falls in same cache line
                     * doing assignment later will avoid some evictions
                     */
                    if(i == j){
                        B[i + k][i + k] = save;
                    }
                }
            }
        }
    /* for 64 * 64 matrix, choose blocksize = 8,
     * and further divide into 4 * 4 sub-matrix,
     * because each 4 * 4 block falls in the same cache line
     * with what's below it(4 * 64 * 4 = 1024)
     * date : 11/11   misses : 2000+  (blocksize = 8)
     * date : 11/12   misses : 1635   (blocksize = 4)
     * date : 11/13   misses : 1147   (blocksize = 8, divide into sub-matrix,
     *                                 and use some of the technics yesterday)
     */
    }else if(M == 64 && N == 64){
        for(i = 0 ; i < N; i += 8){
            for(j = 0 ; j < M; j += 8){
                /* A = a  b  B = 0  0
                 *     c  d      0  0
                 */
                int tmp_1, tmp_2, tmp_3, tmp_4, tmp_5, tmp_6, tmp_7;
                for(int k = i; k < i + 4; k += 1){
                    tmp   = A[k][j + 0];
                    tmp_1 = A[k][j + 1];
                    tmp_2 = A[k][j + 2];
                    tmp_3 = A[k][j + 3];
                    tmp_4 = A[k][j + 4];
                    tmp_5 = A[k][j + 5];
                    tmp_6 = A[k][j + 6];
                    tmp_7 = A[k][j + 7];

                    B[j + 0][k] = tmp;
                    B[j + 1][k] = tmp_1;
                    B[j + 2][k] = tmp_2;
                    B[j + 3][k] = tmp_3;

                    B[j + 0][k + 4] = tmp_4;
                    B[j + 1][k + 4] = tmp_5;
                    B[j + 2][k + 4] = tmp_6;
                    B[j + 3][k + 4] = tmp_7;
                }

                /* A = a  b  B = at bt
                 *     c  d      0  0
                 */
                for(int k = j; k < j + 4; k += 1){
                    tmp   = A[i + 4][k];
                    tmp_1 = A[i + 5][k];
                    tmp_2 = A[i + 6][k];
                    tmp_3 = A[i + 7][k];

                    tmp_4 = B[k][i + 4];
                    tmp_5 = B[k][i + 5];
                    tmp_6 = B[k][i + 6];
                    tmp_7 = B[k][i + 7];

                    B[k][i + 4] = tmp;
                    B[k][i + 5] = tmp_1;
                    B[k][i + 6] = tmp_2;
                    B[k][i + 7] = tmp_3;

                    B[k + 4][i + 0] = tmp_4;
                    B[k + 4][i + 1] = tmp_5;
                    B[k + 4][i + 2] = tmp_6;
                    B[k + 4][i + 3] = tmp_7;
                }

                /* A = a  b  B = at ct
                 *     c  d      bt 0
                 */
                 k = i + 4; l = j + 4;
                 B[l + 3][k]     = A[k][l + 3];
                 B[l + 3][k + 1] = A[k + 1][l + 3];
                 B[l + 3][k + 2] = A[k + 2][l + 3];
                 /* A[k + 2][*] are in cache now */
                 tmp_1   = A[k + 2][l];
                 tmp_2   = A[k + 2][l + 1];
                 tmp_3   = A[k + 2][l + 2];
                 tmp_4   = A[k + 1][l];
                 tmp_5   = A[k + 2][l];
                 /* B[l + 2][*] done */
                 B[l + 2][k]     = A[k][l + 2];
                 B[l + 2][k + 1] = A[k + 1][l + 2];
                 B[l + 2][k + 2] = tmp_3;
                 /* B[l + 1][*] done */
                 tmp_3           = A[k + 1][l + 1];
                 B[l + 1][k]     = A[k][l + 1];
                 B[l + 1][k + 1] = tmp_3;
                 B[l + 1][k + 2] = tmp_2;
                 /* B[l][*] done */
                 tmp_1           = A[k][l];
                 B[l][k]         = tmp_1;
                 B[l][k + 1]     = tmp_4;
                 B[l][k + 2]     = tmp_5;
                 /* B[*][k + 3] done */
                 B[l][k + 3]     = A[k + 3][l];
                 B[l + 1][k + 3] = A[k + 3][l + 1];
                 B[l + 2][k + 3] = A[k + 3][l + 2];
                 /* one eviction here, seems inevitable... */
                 B[l + 3][k + 3] = A[k + 3][l + 3];
            }
        }
    /* for 61 * 67 matrix, set blocksize to 16
     * (still use 8 as blocksize will only get 8.8 out of 10)
     * date : 11/11   misses : 2000+   (blocksize = 8 )
     * date : 11/12   misses : 1989    (blocksize = 16)
     */
    }else{
        for(i = 0 ; i < N; i += 16){
            for(j = 0 ; j < M; j += 16){
                /* bound check */
                for(k = 0; k < 16 && (i + k) < N; k += 1){
                    for(l = 0; l < 16 && (j + l) < M; l += 1){
                        tmp = A[i + k][j + l];
                        if(i + k != j + l){
                            B[j + l][i + k] = tmp;
                        }else{
                            save = tmp;
                        }
                    }
                    /* elements on diagnal of A and B may fall in same cache line
                     * doing assignment later will avoid some evictions
                     */
                    if(i == j){
                        B[i + k][i + k] = save;
                    }
                }
            }
        }
    }
    ENSURES(is_transpose(M, N, A, B));
}

/*
 * You can define additional transpose functions below. We've defined
 * a simple one below to help you get started.
 */

/*
 * trans - A simple baseline transpose function, not optimized for the cache.
 */
char trans_desc[] = "Simple row-wise scan transpose";
void trans(int M, int N, int A[N][M], int B[M][N])
{
    int i, j, tmp;

    REQUIRES(M > 0);
    REQUIRES(N > 0);

    for (i = 0; i < N; i++) {
        for (j = 0; j < M; j++) {
            tmp = A[i][j];
            B[j][i] = tmp;
        }
    }

    ENSURES(is_transpose(M, N, A, B));
}

/*
 * registerFunctions - This function registers your transpose
 *     functions with the driver.  At runtime, the driver will
 *     evaluate each of the registered functions and summarize their
 *     performance. This is a handy way to experiment with different
 *     transpose strategies.
 */
void registerFunctions()
{
    /* Register your solution function */
    registerTransFunction(transpose_submit, transpose_submit_desc);

    /* Register any additional transpose functions */
    registerTransFunction(trans, trans_desc);

}

/*
 * is_transpose - This helper function checks if B is the transpose of
 *     A. You can check the correctness of your transpose by calling
 *     it before returning from the transpose function.
 */
int is_transpose(int M, int N, int A[N][M], int B[M][N])
{
    int i, j;

    for (i = 0; i < N; i++) {
        for (j = 0; j < M; ++j) {
            if (A[i][j] != B[j][i]) {
                return 0;
            }
        }
    }
    return 1;
}
